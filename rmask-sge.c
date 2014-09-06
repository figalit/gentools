#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEFAULT_LINES "#! /bin/sh\n# Specify the shell for this job\n#$ -S /bin/sh\nexport MPICH_PROCESS_GROUP=no\nulimit -c 0\n#Tell Sun Grid Engine to send an email when the job begins\n# and when it ends.\n"
#define DEFAULT_LINES2 "#send it when it started&finished\n#$ -m beas\n#$ -hard\n# Specify the location of the output\n"
#define DEFAULT_LINES3 "# Location of executables\nprogpath=/mnt/local/bin\necho \"Got $NSLOTS slots\"\necho \"path=$PATH\"\necho \"P4_RSHCOMMAND=$P4_RSHCOMMAND\"\necho \"machine_file=$TMPDIR/machines\"\necho \"JOB_ID=$JOB_ID\"\necho \"TEMDPIR=$TMPDIR\"\necho \"HOSTNAME=$HOSTNAME\"\n. /etc/profile.d/modules.sh\nmodule load modules modules-init modules-gs modules-eichler\nmodule load perl wu-blast phrap\nmodule load RepeatMasker/3.2.9\n"

int main(int argc, char **argv){
  int i;
  int mincpu=-1, maxcpu=-1;
  char email[1000]; 
  int SSH=0;
  FILE *fp;
  char currentdir[1000];
  char indir[1000];
  char outdir[1000];
  char txtswap[1000];
  char options[1000];
  char sgefile[1000];
  int SUBMIT=0;
  int retvalue;

  email[0]=0;
  indir[0]=0;
  outdir[0]=0;
  options[0]=0;

  strcpy(sgefile, "rmasker.sh");
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(indir, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outdir, argv[i+1]);
    else if (!strcmp(argv[i], "-email"))
      strcpy(email, argv[i+1]);
    else if (!strcmp(argv[i], "-maxcpu"))
      maxcpu = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-mincpu"))
      mincpu = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-ssh"))
      SSH=1;
    else if (!strcmp(argv[i], "-submit"))
      SUBMIT=1;
    else if (!strcmp(argv[i], "-options"))
      strcpy(options, argv[i+1]);
    else if (!strcmp(argv[i], "-sge"))
      strcpy(sgefile, argv[i+1]);
  }

  if (argc==1){
    fprintf(stdout, "RepeatMasker (v. 3.2.9) MPI/SGE script maker.\nLast update: October 12, 2010.\n\n");
    fprintf(stdout, "%s [options]\n\nCOMPULSORY PARAMETERS:\n", argv[0]);
    fprintf(stdout, "\t-i <input_directory>\n\t-o <output_directory>\n");
    fprintf(stdout, "\t-mincpu <min_cpu_to_allocate>\n\t-maxcpu <max_cpu_to_allocate>\n");
    fprintf(stdout, "\t-email <email_address>: To send notifications.\n");
    fprintf(stdout, "\n\nOPTIONAL PARAMETERS:\n");
    fprintf(stdout, "\t-ssh: Use SSH to run remote commands. Default is RSH if maxcpu<=100; SSH if maxcpu>100\n");
    fprintf(stdout, "\t-options:  Extra options for masking. Use quotes like:\n\t\t-options \"-xsmall -q -species human\"\n");
    fprintf(stdout, "\t-submit: Submit directly to the SGE.\n");
    fprintf(stdout, "\t-sge <filename>: Name the SGE script as <filename>. Default is \"rmask.sh\".\n");
    fprintf(stdout, "\n\n");
    return 0;
  }


  
  if (mincpu<2 || maxcpu<2){
    fprintf(stderr, "Set -mincpu and -maxcpu parameters. Both should be >= 2.\n");
    return 0;
  }

  if (mincpu>maxcpu){
    fprintf(stderr, "maxcpu should be > mincpu. You entered mincpu:%d maxcpu:%d. Fix it.\n", mincpu, maxcpu);
    return 0;
  }
  
  if (email[0]==0){
    fprintf(stderr, "Set the -email parameter.\n");
    return 0;
  }

  if (indir[0]==0){
    fprintf(stderr, "Set the -i parameter to identify the directory that contains input fastq/fasta files.\n");
    return 0;
  }

  if (outdir[0]==0){
    fprintf(stderr, "Set the -o parameter to identify the directory to dump the output data.\n");
    return 0;
  }

  retvalue = mkdir (outdir, S_IRWXU|S_IRGRP|S_IXGRP);

  if (retvalue == 0){
    fprintf(stderr, "Output directory %s created.\n", outdir);
  }
  else{
    if (errno == EEXIST)
      fprintf(stderr, "Output directory %s already exists.\n", outdir);
    else 
      fprintf(stderr, "Cannot create output directory %s for some reason.\n", outdir);

  }

  if (maxcpu>100) SSH=1;

  getcwd(currentdir, 1000);

  if (indir[0] != '/'){
    sprintf(txtswap, "%s/%s", currentdir, indir);
    strcpy(indir, txtswap);
  }
  
  if (outdir[0] != '/'){
    sprintf(txtswap, "%s/%s", currentdir, outdir);
    strcpy(outdir, txtswap);
  }

  fp = fopen(sgefile, "w");
  fprintf(fp, "%s\n", DEFAULT_LINES);

  if (SSH)
    fprintf(fp, "export P4_RSHCOMMAND=/usr/bin/ssh\nexport PRINT_SEQUENCES=2\nexport C3_RSH=\'ssh -q\'\n");
  else  
    fprintf(fp, "export P4_RSHCOMMAND=/usr/bin/rsh\nexport PRINT_SEQUENCES=2\nexport C3_RSH=\'rsh\'\n");

  fprintf(fp, "%s\n", DEFAULT_LINES2);
  
  fprintf(fp, "# pe request\n#$ -pe orte %d-%d\n#$ -M %s\n", mincpu, maxcpu, email);

  
  fprintf(fp, "#$ -o %s\n#$ -e %s\n", currentdir, currentdir);

  fprintf(fp, "%s\n\n", DEFAULT_LINES3);

  fprintf(fp, "RMASKER=`which RepeatMasker`\n\n");


  fprintf(stdout, "%s is created.\n", sgefile);

  fprintf(fp, "/usr/lib64/openmpi/1.4-gcc/bin/mpirun -mca btl ^openib  -n $NSLOTS /net/eichler/vol2/local/inhousebin/mpiprograms/rmasker44-openmpi \"$RMASKER -engine wublast %s \" %s %s $JOB_ID\n",  options, indir, outdir);



  fclose(fp);

  if (SUBMIT){
    sprintf(options, "qsub -q all.q %s", sgefile);
    system(options);
  }
  else
    printf("Type/copy-paste:\n\t\tqsub -q all.q %s\n", sgefile);
  

  
  /*
  if (PAIRED_END)
    fprintf(fp,"/net/eichler/vol6/software/mpich-1.2.7-amd64/bin/mpirun -np $NSLOTS -machinefile $TMPDIR/machines /net/eichler/vol2/local/inhousebin/mpiprograms/mrfast2batch \"/net/eichler/vol3/home/calkan/bin/mrfast2 --search %s --pe --max %d --min %d -e %d --discordant-vh\" %s %s $JOB_ID\n", genome, max, min, ed, indir, outdir);
  else
      fprintf(fp,"/net/eichler/vol6/software/mpich-1.2.7-amd64/bin/mpirun -np $NSLOTS -machinefile $TMPDIR/machines /net/eichler/vol2/local/inhousebin/mpiprograms/mrfast2batch \"/net/eichler/vol3/home/calkan/bin/mrfast2 --search %s -e %d\" %s %s $JOB_ID\n", genome, ed, indir, outdir);
  */


}
