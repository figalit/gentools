#include <stdio.h>
#include <string.h>

#define DEFAULT_LINES "#! /bin/sh\n# Specify the shell for this job\n#$ -S /bin/sh\nexport MPICH_PROCESS_GROUP=no\nulimit -c 0\n#Tell Sun Grid Engine to send an email when the job begins\n# and when it ends.\n"
#define DEFAULT_LINES2 "#send it when it started&finished\n#$ -m beas\n#$ -hard\n#$ -l h_vmem=4G\n#Specify the location of the output\n"
#define DEFAULT_LINES3 "# Location of executables\necho \"Got $NSLOTS slots\"\necho \"path=$PATH\"\necho \"P4_RSHCOMMAND=$P4_RSHCOMMAND\"\necho \"machine_file=$TMPDIR/machines\"\necho \"JOB_ID=$JOB_ID\"\necho \"TEMPDIR=$TMPDIR\"\necho \"HOSTNAME=$HOSTNAME\"\n"

int main(int argc, char **argv){
  int SINGLE_END=0;
  int PAIRED_END=0;
  int max=-1, min=-1;
  int ed=-1;
  int i;
  int mincpu=-1, maxcpu=-1;
  char genome[1000]; 
  char email[1000]; 
  int SSH=0;
  FILE *fp;
  char currentdir[1000];
  char indir[1000];
  char outdir[1000];
  char txtswap[1000];
  int BITCH=0;
  int COLOR=0;
  char program[100];
  char options[1000];
  char peseopt[1000];
  char sgefile[1000];
  int SUBMIT=0;
  char queue[100];
  int MRF2006=0;
  int openmpi=1;

  genome[0]=0;
  email[0]=0;
  indir[0]=0;
  outdir[0]=0;
  options[0]=0;
  peseopt[0]=0;
  strcpy(sgefile, "mrmap.sh");
  strcpy(queue, "all.q");

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-pe"))
      PAIRED_END=1;
    else if (!strcmp(argv[i], "-se"))
      SINGLE_END=1;
    else if (!strcmp(argv[i], "-max"))
      max = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-min"))
      min = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      ed = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-g"))
      strcpy(genome, argv[i+1]);
    else if (!strcmp(argv[i], "-i"))
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
    else if (!strcmp(argv[i], "-s"))
      BITCH=1;
    else if (!strcmp(argv[i], "-cs"))
      COLOR=1;
    else if (!strcmp(argv[i], "-old"))
      MRF2006=1;
    else if (!strcmp(argv[i], "-submit"))
      SUBMIT=1;
    else if (!strcmp(argv[i], "-options"))
      strcpy(options, argv[i+1]);
    else if (!strcmp(argv[i], "-sge"))
      strcpy(sgefile, argv[i+1]);
    else if (!strcmp(argv[i], "-q"))
      strcpy(queue, argv[i+1]);
    else if (!strcmp(argv[i], "-openmpi"))
      openmpi = 1;   
    else if (!strcmp(argv[i], "-mpich"))
      openmpi = 0;
  }

  if (argc==1){
    fprintf(stdout, "mr(s)FAST version 2 MPI/SGE script maker.\nLast update: July 20, 2011.\n\n");
    fprintf(stdout, "%s [options]\n\nCOMPULSORY PARAMETERS:\n", argv[0]);
    fprintf(stdout, "\t-i <input_directory>\n\t-o <output_directory>\n");
    fprintf(stdout, "\t-mincpu <min_cpu_to_allocate>\n\t-maxcpu <max_cpu_to_allocate>\n");
    fprintf(stdout, "\t-g <genome_file>: Genome file to map. \".index\" file should be in the same directory with this file.\n");
    fprintf(stdout, "\t-e <edit_dist_cutoff>\n");
    fprintf(stdout, "\t-email <email_address>: To send notifications.\n\n\tMAPPING MODES -- PICK ONE:\n");
    fprintf(stdout, "\t-se: Single end mode.\n\t-pe: Paired end mode.\n\t\t-min <min>: Minimum span threshold for PE mode.\n\t\t-max <max>: Maximum span threshold for PE mode.\n\nOPTIONAL PARAMETERS:\n");
    fprintf(stdout, "\t-ssh: Use SSH to run remote commands. Default is RSH if maxcpu<=100; SSH if maxcpu>100\n");
    fprintf(stdout, "\t-s: Use mrsfast instead of mrfast. It will be faster, but not really recommended for PE yet.\n");
    fprintf(stdout, "\t-options:  Extra options for mapping. Use quotes like:\n\t\t-options \"--outcomp --seqcomp\"\n");
    fprintf(stdout, "\t-submit: Submit directly to the SGE.\n");
    fprintf(stdout, "\n\n");
    return 0;
  }



  if (SINGLE_END && PAIRED_END){
    fprintf(stderr, "Single end mode or paired end mode? Make up your mind.\n");
    return 0;
  }

  SINGLE_END = !PAIRED_END;

  if (PAIRED_END && (min==-1|| max==-1)){
    fprintf(stderr, "Set -min and -max parameters for the PAIRED_END mode.\n");
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
  
  if (PAIRED_END && min>max){
    fprintf(stderr, "PE mode max should be > min. You entered min:%d max:%d. Fix it.\n", min, max);
    return 0;
  }
  
  if (email[0]==0){
    fprintf(stderr, "Set the -email parameter.\n");
    return 0;
  }

  if (genome[0]==0){
    fprintf(stderr, "Set the -g parameter to identify which index to use.\n");
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

  if (ed==-1){
    fprintf(stderr, "**WARNING***\tEdit distance threshold (-e) is not set. Resetting to ed=2\n");
    ed = 2;
  }
  
  if (BITCH==1 && COLOR==1){
    fprintf(stderr, "Select either -s (mrsfast) or -cs (drfast). Not both!\n");
    return 0;
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
  
  if (openmpi)
    fprintf(fp, "# pe request\n#$ -pe orte %d-%d\n#$ -M %s\n", mincpu, maxcpu, email);
  else
    fprintf(fp, "# pe request\n#$ -pe mpich %d-%d\n#$ -M %s\n", mincpu, maxcpu, email);

  
  fprintf(fp, "#$ -o %s\n#$ -e %s\n", currentdir, currentdir);

  fprintf(fp, "%s\n\n", DEFAULT_LINES3);

  if (BITCH) strcpy(program, "/net/eichler/vol3/home/calkan/bin/mrsfast");
  else if (COLOR)
    strcpy(program, "/net/eichler/vol3/home/calkan/bin/drfast");
  else if (MRF2006)
    strcpy(program, "/net/eichler/vol3/home/calkan/bin/mrfast-2.0.0.6");
  else 
    strcpy(program, "/net/eichler/vol3/home/calkan/bin/mrfast-2.1.0.2");

  if (PAIRED_END){
    if (BITCH || MRF2006)
      sprintf(peseopt, "--pe --max %d --min %d --discordant-vh", max, min);
    else
      sprintf(peseopt, "--pe --max %d --min %d --discordant-vh --maxoea 500 --maxdis 500", max, min);
  }
  else
    strcpy(peseopt, "");

  if (openmpi){
    fprintf(fp, "/usr/lib64/openmpi/1.4-gcc/bin/mpirun -mca btl ^openib  -n $NSLOTS /net/eichler/vol2/local/inhousebin/mpiprograms/mrfast2batch-openmpi \"%s --search %s %s -e %d %s\" %s %s $JOB_ID\n", program, genome, peseopt, ed, options, indir, outdir);
  }
  
  else
    fprintf(fp, "/net/eichler/vol6/software/mpich-1.2.7-amd64/bin/mpirun -np $NSLOTS -machinefile $TMPDIR/machines /net/eichler/vol2/local/inhousebin/mpiprograms/mrfast2batch \"%s --search %s %s -e %d %s\" %s %s $JOB_ID\n", program, genome, peseopt, ed, options, indir, outdir);

  fclose(fp);

  fprintf(stdout, "%s is created. I will run:\n\t %s --search %s %s -e %d %s\n", sgefile, program, genome, peseopt, ed, options);


  if (SUBMIT){
    sprintf(options, "qsub -l disk_free=20G -q %s %s", queue, sgefile);
    fprintf(stdout, "SUBMITTING qsub -q %s %s\n", queue, sgefile);
    system(options);
  }
  else
    printf("Type/copy-paste:\n\t\tqsub -q %s %s\n", queue, sgefile);
  

  
  /*
  if (PAIRED_END)
    fprintf(fp,"/net/eichler/vol6/software/mpich-1.2.7-amd64/bin/mpirun -np $NSLOTS -machinefile $TMPDIR/machines /net/eichler/vol2/local/inhousebin/mpiprograms/mrfast2batch \"/net/eichler/vol3/home/calkan/bin/mrfast2 --search %s --pe --max %d --min %d -e %d --discordant-vh\" %s %s $JOB_ID\n", genome, max, min, ed, indir, outdir);
  else
      fprintf(fp,"/net/eichler/vol6/software/mpich-1.2.7-amd64/bin/mpirun -np $NSLOTS -machinefile $TMPDIR/machines /net/eichler/vol2/local/inhousebin/mpiprograms/mrfast2batch \"/net/eichler/vol3/home/calkan/bin/mrfast2 --search %s -e %d\" %s %s $JOB_ID\n", genome, ed, indir, outdir);
  */


}
