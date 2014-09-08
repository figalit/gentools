#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#define ASPERACMD "ascp -k 1 -i /net/eichler/vol3/home/calkan/asperaetc/asperaweb_id_dsa.putty -Q -Tr -l 300M anonftp@ftp-private.ncbi.nlm.nih.gov:/1000genomes/ftp/"
#define ASPERACMD_EBI "ascp -k 1 -i /net/eichler/vol3/home/calkan/asperaetc/asperaweb_id_dsa.putty -Q -Tr -l 300M fasp-g1k@fasp.1000genomes.ebi.ac.uk:vol1/ftp/"

#define MAXLANES 1000
//#define EMAIL "calkan@uw.edu"
#define EMAIL "NONE"
#define MRSFASTDB "/var/tmp/calkan/hg19"

#define ALLCHROM "/net/eichler/vol3/home/calkan/WSSD/Annotations/AllChro"
#define GAPFILE "/net/eichler/vol3/home/calkan/WSSD/Annotations/hg19.gap.bed"
#define SATFILE "/net/eichler/vol3/home/calkan/WSSD/Annotations/hg19_satellite.bed"
#define INITINFO "/net/eichler/vol3/home/calkan/WSSD/Annotations/initInfo"

#define DEFAULT_LINES "#! /bin/sh\n# Specify the shell for this job\n#$ -S /bin/sh\nulimit -c 0\n#Tell Sun Grid Engine to send an email when the job begins\n# and when it ends.\n"
#define DEFAULT_LINES2 "#send it when it started&finished\n#$ -m n\n#$ -hard\n\n#$ -l h_rss=30G\n# Specify the location of the output\n"
#define DEFAULT_LINES3 "# Location of executables\nprogpath=/mnt/local/bin\necho \"Got $NSLOTS slots\"\necho \"path=$PATH\"\necho \"P4_RSHCOMMAND=$P4_RSHCOMMAND\"\necho \"machine_file=$TMPDIR/machines\"\necho \"JOB_ID=$JOB_ID\"\necho \"TEMDPIR=$TMPDIR\"\necho \"HOSTNAME=$HOSTNAME\"\n"
#define FASTQDELHASH "/net/eichler/vol3/home/calkan/bin/fastqdelhash"
#define SAM2FQ "/net/eichler/vol3/home/calkan/bin/sam2fq-pe"

typedef struct lane{
  char samplename[100];
  char libname[100];
  int readlen;
  int readlen2;
  char runid[1000];
  int numreads;
}_lane;


int countdir (char *);
void mergelibs(char *, char *);
int  mytok(char *str, char delim, char *save, int start);

struct lane mylanes[MAXLANES];
int findlog (char *directory, char *samplename, char *libname, int readlen, char *maplogfile);
int findlog_sample (char *directory, char *samplename, char *libname, int readlen, char *maplogfile);
static int lane_compar(const void *p1, const void *p2);

int main(int argc, char **argv){
  int i;
  char indexname[100];
  char basname[100];
  char logname[100];
  FILE *index, *bas, *log;

  FILE *maplog, *vh;
  char maplogfile[1000];
  
  //char *token;
  char token[1000];

  char samplename[100];
  char libname[100];
  char runid[100];
  char command[10000];
  char dataloc[1000];
  char dummystr[1000];
  int fragsize;
  int iswithdrawn;
  char line[10000];
  long int numbases;
  int numreads;
  int thisnumreads;
  double coverage;
  int readlen;

  char dirname[1000];
  int retval;
  mode_t MKDIRMODE =  S_IRWXU|S_IRGRP|S_IXGRP;

  int lanecnt=0;

  int numfiles;

  char basstring[1000];
 
  char thislib[1000]; int thislen;

  float mean, stdev;
  float thismin;
  int median, meddev;
  
  int mincut, maxcut;
  int mincpu, maxcpu;

  int editdist;

  char indir[1000], outdir[1000], vhdir[1000], curdir[2000];
  char vhrunfile[2000];
  int isfinished;
  char queue[100];
  int skipbreak=0;
  int skipdown=0;
  int dlonly=0;
  int ebi=1;

  int maxmax;
  int minmin;

  int dlfail;
  int nomap=0;
  int finishup=0;
  int bz=0;

  time_t rawtime;

  int jobid;
  int hrss; int numreadsperfile;
  int tokenpos = 0;

  maxmax = 180;
  minmin = 50;
  median = -1;

  indexname[0]=0; basname[0]=0;
  sprintf(queue, "all.q");
  //sprintf(queue, "prod.q");


  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i")) strcpy(indexname, argv[i+1]);
    else if (!strcmp(argv[i], "-b")) strcpy(basname, argv[i+1]);
    else if (!strcmp(argv[i], "-q")) strcpy(queue, argv[i+1]);
    else if (!strcmp(argv[i], "-skipdown")) skipdown=1;
    else if (!strcmp(argv[i], "-skipbreak")) skipbreak=1;
    else if (!strcmp(argv[i], "-dlonly")) dlonly=1;
    else if (!strcmp(argv[i], "-nomap")) nomap=1;
    else if (!strcmp(argv[i], "-finishup")) finishup=1;
    else if (!strcmp(argv[i], "-bz")) bz=1;
    else if (!strcmp(argv[i], "-ebi")) ebi=1;
    else if (!strcmp(argv[i], "-maxmax")) maxmax = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-minmin")) minmin = atoi(argv[i+1]);
  }
  
  //  sprintf(queue, "rhel5.q");
  //ebi = 1;

  for (i=0;i<MAXLANES;i++) mylanes[i].numreads = 0;

  if (indexname[0]==0) {fprintf(stderr, "Index file required.\n"); return -1;}
  if (basname[0]==0) {fprintf(stderr, "BAS file required.\n"); return -1;}

  index = fopen(indexname, "r");
  bas = fopen(basname, "r");
  
  if (index == NULL){fprintf(stderr, "Index file %s not found.\n", indexname); return -1;}
  if (bas == NULL){fprintf(stderr, "BAS file %s not found.\n", basname); return -1;}

  sprintf(logname, "%s.log", indexname);
  log = fopen(logname, "w");

  /*

  FASTQ_FILE MD5 RUN_ID STUDY_ID STUDY_NAME CENTER_NAME SUBMISSION_ID SUBMISSION_DATE SAMPLE_ID SAMPLE_NAME POPULATION EXPERIMENT_ID INSTRUMENT_PLATFORM INSTRUMENT_MODEL LIBRARY_NAME 
  RUN_NAME RUN_BLOCK_NAME INSERT_SIZE LIBRARY_LAYOUT PAIRED_FASTQ WITHDRAWN WITHDRAWN_DATE COMMENT READ_COUNT BASE_COUNT ANALYSIS_GROUP

  */



  numreads = 0 ;

  while (1){
    fgets(line, 10000, index);
    if (feof(index)) break;

    if (strstr(line, "exome")) continue;
    
    //printf("%s", line);
    tokenpos = 0;

    //    token = strtok(line, "\t"); 
    tokenpos = mytok (line, '\t', token, tokenpos);
    strcpy(dataloc, token);

    tokenpos = mytok (line, '\t', token, tokenpos);
    //    token = strtok(NULL, "\t"); // md5
    tokenpos = mytok (line, '\t', token, tokenpos);
    //token = strtok(NULL, "\t"); // runid
    strcpy(runid, token);

    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);

    //    token = strtok(NULL, "\t"); // studyid
    //token = strtok(NULL, "\t"); // studyname
    //token = strtok(NULL, "\t"); // centername

    printf ("centername: %s\n", token);

    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);

    /*
    token = strtok(NULL, "\t"); // submission id
    token = strtok(NULL, "\t"); // submission date
    token = strtok(NULL, "\t"); // sample id

    token = strtok(NULL, "\t"); // samplename  
    */

    strcpy(samplename, token);
    printf ("samplename: %s\n", token);


    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    /*
    token = strtok(NULL, "\t"); // population
    token = strtok(NULL, "\t"); // experimentid
    token = strtok(NULL, "\t"); // instrument platform
    token = strtok(NULL, "\t"); // instrument model

    token = strtok(NULL, "\t"); // library name
    */
    strcpy(libname, token); 
    printf ("libname: %s\n", token);

    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    tokenpos = mytok (line, '\t', token, tokenpos);
    /*
    token = strtok(NULL, "\t"); // run name & run block name
    //token = strtok(NULL, "\t"); // run block name

    token = strtok(NULL, "\t"); // insert size
    */

    printf ("fragsize: %s\n", token);

    fragsize = atoi(token);
    
    tokenpos = mytok (line, '\t', token, tokenpos);

    //    token = strtok(NULL, "\t"); // library layout
    
    if (strstr(dataloc, "_1.filt.fastq.gz") ||strstr(dataloc, "_2.filt.fastq.gz"))
      //token = strtok(NULL, "\t"); // paired_fastq
      tokenpos = mytok (line, '\t', token, tokenpos);
    else
      continue;

    tokenpos = mytok (line, '\t', token, tokenpos);
    //    token = strtok(NULL, "\t"); // withdrawn
    iswithdrawn = atoi(token);
    printf ("iswithdrawn: %s\n", token);

    if (iswithdrawn == 1) continue;
    
    //token = strtok(NULL, "\t"); // withdrawn_date
    tokenpos = mytok (line, '\t', token, tokenpos);
    //comment
    tokenpos = mytok (line, '\t', token, tokenpos);

	//    token = strtok(NULL, "\t"); // comment or redadcnt

    //token = strtok(NULL, "\t"); // readcnt
    
    /*
    if (!isdigit(token[0])){
      printf("if !isdigit. prevtoken: %s\n", token);
      //      token = strtok(NULL, "\t"); // readcnt
      tokenpos = mytok (line, '\t', token, tokenpos);
      thisnumreads = atoi(token);    
    }
    else */
    tokenpos = mytok (line, '\t', token, tokenpos);
    thisnumreads = atoi(token);

    printf ("thisnumreads: %s\n", token);


    tokenpos = mytok (line, '\t', token, tokenpos);

    //    token = strtok(NULL, "\t"); // basecnt
    printf ("numbases: %s\n", token);

    numbases = atol(token);


    //token = strtok(NULL, "\t"); // analysisgroup

    readlen = numbases / thisnumreads;
   
    
    printf("reads %d bases %ld readlen %d\n", thisnumreads, numbases, readlen);
    if (readlen<=35) continue;
    numreads += thisnumreads;

    
    /*
    fprintf(stdout, "fastqfile: %s\n", dataloc);
    fprintf(stdout, "sample: %s\nlib: %s\nfrag: %d\n", samplename, libname, fragsize);
    */

    
    mkdir(samplename, MKDIRMODE);
    sprintf(dirname, "%s/%s", samplename, libname);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/%s/%d", samplename, libname, readlen);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/%s/%d/download", samplename, libname, readlen);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/%s/%d/fastq", samplename, libname, readlen);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/%s/%d/out", samplename, libname, readlen);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/%s/%d/fastq_sample", samplename, libname, readlen);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/%s/%d/out_sample", samplename, libname, readlen);
    mkdir(dirname, MKDIRMODE);
    sprintf(dirname, "%s/VH", samplename);
    mkdir(dirname, MKDIRMODE);
    strcpy(vhdir, dirname);

    sprintf(dirname, "%s/STATS", samplename);
    mkdir(dirname, MKDIRMODE);


    if (!skipdown){
      if (ebi)
	sprintf(command, "%s%s %s/%s/%d/download/", ASPERACMD_EBI, dataloc, samplename, libname, readlen);
      else
	sprintf(command, "%s%s %s/%s/%d/download/", ASPERACMD, dataloc, samplename, libname, readlen);
      
      printf("COMMAND: %s\n", command);
      
      retval = 1;
      
      dlfail = 0;

      while(retval !=0){
	if (dlfail >= 3){
	  ebi = ~ebi;
	  if (ebi)
	    sprintf(command, "%s%s %s/%s/%d/download/", ASPERACMD_EBI, dataloc, samplename, libname, readlen);
	  else
	    sprintf(command, "%s%s %s/%s/%d/download/", ASPERACMD, dataloc, samplename, libname, readlen);
	  dlfail = 0;
	}
	retval = system(command);
	dlfail++;
      }
    }

    if (dlonly)
      continue; 

    if (strstr(dataloc, "_1.filt.fastq.gz")){
      strcpy(mylanes[lanecnt].samplename, samplename);
      strcpy(mylanes[lanecnt].libname, libname);
      strcpy(mylanes[lanecnt].runid, runid);
      mylanes[lanecnt].readlen = readlen;
      mylanes[lanecnt].readlen2 = readlen;
      mylanes[lanecnt].numreads += numreads;
      lanecnt++;      
    }
    else if (strstr(dataloc, "_2.filt.fastq.gz")){
      if (lanecnt != 0 && !strcmp(mylanes[lanecnt-1].runid, runid)){
	mylanes[lanecnt-1].readlen2 = readlen;	
	mylanes[lanecnt-1].numreads += numreads;
      }
    }
    
  }
  

  
  printf("Total %d lanes.\n", lanecnt);

  
  fprintf(log, "Total %d lanes downloaded.\n", lanecnt);
  
  if (dlonly) 
    return;

  //  numreads = 0;

  qsort(mylanes, lanecnt, sizeof(struct lane), lane_compar);


  if (!skipbreak){
    for (i=0;i<lanecnt;i++){
      
      if (mylanes[i].readlen >= 75) numreadsperfile = 250000;
      else if (mylanes[i].readlen >= 50) numreadsperfile = 150000;
      else numreadsperfile = 75000;
      
      if (mylanes[i].readlen2 < mylanes[i].readlen){
	if (mylanes[i].readlen2 >= 75) numreadsperfile = 250000;
	else if (mylanes[i].readlen2 >= 50) numreadsperfile = 150000;
	else numreadsperfile = 75000;
      }

      sprintf(command, "mixcat2 -gz -f %s/%s/%d/download/%s_1.filt.fastq.gz -r %s/%s/%d/download/%s_2.filt.fastq.gz | fastqbreak -rmshit -n %d -o %s/%s/%d/fastq/%s_%s_", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].runid, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen2, mylanes[i].runid, numreadsperfile, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].runid);
      
      
      printf("COMMAND: %s\n", command);
      
      thisnumreads = system(command);
      printf("Got %d reads.\n", thisnumreads);

      //  numreads = numreads + thisnumreads;
      

    }
  }


  //numreads = numreads * 2;

  getcwd(curdir, 2000);

  sprintf(vhrunfile, "%s-vh.sge", mylanes[0].samplename);
  vh = fopen (vhrunfile, "w");

  fprintf(vh, "%s\n", DEFAULT_LINES);
  fprintf(vh, "%s\n", DEFAULT_LINES2);
  //fprintf(vh, "# pe request\n#$ -pe orte 1-1\n#$ -M %s\n", EMAIL); 
  fprintf(vh, "# pe request\n#$ -pe orte 1-1\n");
  fprintf(vh, "#$ -o %s\n#$ -e %s\n", curdir, curdir);
  fprintf(vh, "%s\n\n", DEFAULT_LINES3);
  
  fprintf(vh, ". /etc/profile.d/modules.sh\nmodule load modules modules-init modules-eichler modules-gs  pbzip2/1.1.6\n\n");



  thislib[0] = 0; thislen = 0;

  for (i=0;i<lanecnt;i++){
    if (strcmp(thislib, mylanes[i].libname) || thislen != mylanes[i].readlen){
      strcpy(thislib, mylanes[i].libname); thislen= mylanes[i].readlen;
      rewind (bas);
      
      /*
	bam_filename md5 study sample platform library readgroup #_total_bases #_mapped_bases #_total_reads #_mapped_reads #_mapped_reads_paired_in_sequencing
	#_mapped_reads_properly_paired %_of_mismatched_bases average_quality_of_mapped_bases mean_insert_size insert_size_sd median_insert_size
	insert_size_median_absolute_deviation #_duplicate_reads
      */
      
      mean = 0.0;



      while (fscanf(bas, "%s", basstring) > 0){ // bam_filename
	fscanf(bas, "%s", basstring); // md5
	fscanf(bas, "%s", basstring); // study
	fscanf(bas, "%s", basstring); // sample
	if (strcmp(basstring,  mylanes[i].samplename)){
	  fgets(line, 10000, bas); continue;
	}
	fscanf(bas, "%s", basstring); // platform
	fscanf(bas, "%s", basstring); // library

	if (strcmp(basstring,  mylanes[i].libname)){
	  fgets(line, 10000, bas); continue;
	}
	
	fscanf(bas, "%s", basstring); // readgroup
	fscanf(bas, "%s", basstring); // totalbases
	fscanf(bas, "%s", basstring); // mappedbases
	fscanf(bas, "%s", basstring); // totalreads
	fscanf(bas, "%s", basstring); // mappedreads
	fscanf(bas, "%s", basstring); // #_mapped_reads_paired_in_sequencing
	fscanf(bas, "%s", basstring); // #_mapped_reads_properly_paired
	fscanf(bas, "%s", basstring); // %_of_mismatched_bases
	fscanf(bas, "%s", basstring); // average_quality_of_mapped_bases

	fscanf(bas, "%f", &mean);	
	fscanf(bas, "%f", &stdev);	

	fscanf(bas, "%d", &median);	
	fscanf(bas, "%d", &meddev);	
	
	fgets(line, 10000, bas);

	break;
      }
      
      if (mean == 0.0){
	fprintf(log, "Sample: %s\tLibrary: %s\tNot in BAS file", mylanes[i].samplename, mylanes[i].libname);
	//continue;
      }


      if (mylanes[i].readlen <= 60) editdist = 2;
      else if (mylanes[i].readlen <= 80) editdist = 3;
      else if (mylanes[i].readlen <= 110) editdist = 4;
      else if (mylanes[i].readlen <= 130) editdist = 5;
      else editdist = 6;

      if (mylanes[i].readlen2 < mylanes[i].readlen){
	if (mylanes[i].readlen2 <= 60) editdist = 2;
	else if (mylanes[i].readlen2 <= 80) editdist = 3;
	else if (mylanes[i].readlen2 <= 110) editdist = 4;
	else if (mylanes[i].readlen2 <= 130) editdist = 5;
	else editdist = 6;
      }
      
      sprintf(indir, "%s/%s/%d/fastq",  mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      sprintf(outdir, "%s/%s/%d/out",  mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);



      numfiles = countdir(indir);
      printf("Total %d files for library %s readlength %d.\n", numfiles, mylanes[i].libname, mylanes[i].readlen);


      mylanes[i].numreads = numfiles * numreadsperfile;

      if (numfiles <= maxmax) maxcpu = numfiles+1; else if (numfiles >= maxmax*2+1) maxcpu = maxmax+1; else maxcpu = numfiles / 2;
      if (numfiles <= minmin) mincpu = numfiles+1; else mincpu = minmin;
      if (mincpu>maxcpu) mincpu=maxcpu;

      
      if (numfiles != 0 || finishup){

      /* SAMPLE */

	sprintf(dirname, "%s/%s/%d/fastq_sample", mylanes[i].samplename, libname, mylanes[i].readlen);
	mkdir(dirname, MKDIRMODE);
	sprintf(dirname, "%s/%s/%d/out_sample", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	mkdir(dirname, MKDIRMODE);

	printf("Sample fastqs.\n");
	
	if (!skipbreak  ){
	  sprintf(command, "cat %s/%s/%d/fastq/* | head -n 20000000 | fastqbreak -rmshit -n 50000 -o %s/%s/%d/fastq_sample/", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen,  mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	  
	  printf("COMMAND: %s\n", command);
	  system(command);
	}

	if (nomap) 
	  continue;

	if (mylanes[i].readlen >= 50) hrss=4;
	else hrss=5;

	if (mylanes[i].readlen2 < mylanes[i].readlen){
	  if (mylanes[i].readlen2 >= 50) hrss=4;
	  else hrss=5;
	}
	
	//sprintf(command, "mrmap -hrss %dG -submit  -email %s -g %s -e %d -pe -min 0 -max 1000 -mincpu 20 -maxcpu 51 -i %s_sample -o %s_sample -sge %s-%s-%d_sample.sge > %s-%s-%d.sampleID", hrss, EMAIL, MRSFASTDB, editdist, indir, outdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	
	sprintf(command, "mrmap -hvmem %dG -submit -q %s -email %s -g %s -e %d -pe -min 0 -max 1000 -mincpu 20 -maxcpu 51 -i %s_sample -o %s_sample -sge %s-%s-%d_sample.sge > %s-%s-%d.sampleID", hrss, queue, EMAIL, MRSFASTDB, editdist, indir, outdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	
	
	printf("COMMAND: %s\n", command);
	
	if (!finishup && !nomap)
	  system(command);
	
	strcpy(maplogfile, "");
	isfinished = 0;
	if (finishup || nomap)
	  isfinished = 1;

	while (!isfinished){
	  time ( &rawtime );
	  
	  printf("Sleeping for 5 minutes for sample. Time now: %s\n", asctime(localtime(&rawtime)));
	  sleep(300);
	  printf("Checking sample map log.\n");
	  if (!strcmp(maplogfile, ""))
	    jobid = findlog_sample(curdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen, maplogfile);	
	  if (strcmp(maplogfile, "")){
	    printf("Inspecting sample map log %s.\n", maplogfile);
	    maplog = fopen(maplogfile, "r");
	    if (maplog != NULL){
	      while (fscanf(maplog, "%s", line) > 0){
		if (!strcmp(line, "collected,")){
		  isfinished = 1;
		  sleep(60);
		  sprintf(command, "qdel %d", jobid);
		  system(command);
		  break;
		}
	      }
	      fclose(maplog);
	    }
	    else
	      printf("Sample job not yet started : %s %s %d.\n",  mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
	  }
	  else
	    printf("Sample job not yet started : %s %s %d.\n",  mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
	}
	
	if (mean == 0.0) median=500;
	sprintf(command, "cat %s_sample/*sam | awk \'{if ($9>0 && $9<%d) print $9}\' > %s/STATS/%s-%s-%d.len", outdir, (median*2+20), mylanes[i].samplename, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	
	printf("COMMAND: %s\n", command);
	if (!finishup)
	  system(command);
	
	sprintf(command, "stats -i %s/STATS/%s-%s-%d.len -revert -title; gnuplot %s/STATS/%s-%s-%d.len.gnuplot", mylanes[i].samplename, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	
	printf("COMMAND: %s\n", command);
	if (!finishup)
	  system(command);
	
	sprintf(maplogfile, "%s/STATS/%s-%s-%d.len.gnuplot", mylanes[i].samplename, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
	maplog = fopen(maplogfile, "r");
	
	/*
	  set label 'Mean:   309.68' at 22000, 5500
	  set label 'Median: 310.00' at 22000, 5300
	  set label 'Stdev:  20.48'at 22000, 4900
	*/
	
	while (fscanf(maplog, "%s", line) > 0){
	  if (!strcmp(line, "\'Mean:")){
	    fscanf(maplog, "%s", line);
	    line[strlen(line)-1] = 0;
	    mean = atof(line);
	    fprintf(stdout, "Mean: %f\n", mean);
	    fprintf(log, "Mean: %f\n", mean);
	  }
	  else if (!strcmp(line, "\'Median:")){
	    fscanf(maplog, "%s", line);
	    line[strlen(line)-1] = 0;
	    median = atof(line);
	  }
	  else if (!strcmp(line, "\'Stdev:")){
	    fscanf(maplog, "%s", line);
	    line[strlen(line)-1] = 0;
	    stdev = atof(line);
	    fprintf(stdout, "Stdev: %f\n", stdev);
	    fprintf(log, "Stdev: %f\n", stdev);
	  }
	  else if (!strcmp(line, "\'Min:")){
	    fscanf(maplog, "%s", line);
	    line[strlen(line)-1] = 0;
	    thismin = atof(line);
	    fprintf(stdout, "Min: %f\n", thismin);
	    fprintf(log, "Min: %f\n", thismin);
	  }
	}

	fclose(maplog);

	coverage = ((double)mylanes[i].numreads / 3000000000.0)*  mylanes[i].readlen;


	mincut = mean - 4.5*stdev - 1;
	maxcut = mean + 4.5*stdev + 1;
	if (mincut < mylanes[i].readlen*2) mincut = 0;
	if (median-mean >= 2.5) mincut = 0;
	if (thismin <= mylanes[i].readlen) mincut = 0;

	fprintf(log, "Sample: %s\tLibrary: %s\tReadlen: %d\tNumReads: %d\tCoverage: %f\tMean:%f\tStd:%f\tMedian:%d\tAbsDev:%d\tMin: %d\tMax: %d\tEdit_dist: %d\n", mylanes[i].samplename, mylanes[i].libname,  mylanes[i].readlen, mylanes[i].numreads, coverage, mean, stdev, median, meddev, mincut, maxcut, editdist);

	fprintf(stdout, "Sample: %s\tLibrary: %s\tReadlen: %d\tNumReads: %d\tCoverage: %f\tMean:%f\tStd:%f\tMedian:%d\tAbsDev:%d\tMin: %d\tMax: %d\tEdit_dist: %d\n", mylanes[i].samplename, mylanes[i].libname,  mylanes[i].readlen, mylanes[i].numreads, coverage, mean, stdev, median, meddev, mincut, maxcut, editdist);

      /* END SAMPLE */


	if (mylanes[i].readlen >= 75) hrss=5;
	else if (mylanes[i].readlen >= 50) hrss=6;
	else hrss=8;
	if (mylanes[i].readlen2 < mylanes[i].readlen){
	  if (mylanes[i].readlen2 >= 75) hrss=5;
	  else if (mylanes[i].readlen2 >= 50) hrss=7;
	  else hrss=8;
	}

	//sprintf(command, "mrmap -hrss %dG -submit -email %s -g %s -e %d -pe -min %d -max %d -mincpu %d -maxcpu %d -i %s -o %s -sge %s-%s-%d.sge > %s-%s-%d.ID", hvmem,  EMAIL, MRSFASTDB, editdist, mincut, maxcut, mincpu, maxcpu, indir, outdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);

	sprintf(command, "mrmap -hvmem %dG -submit -q %s -email %s -g %s -e %d -pe -min %d -max %d -mincpu %d -maxcpu %d -i %s -o %s -sge %s-%s-%d.sge > %s-%s-%d.ID", hrss, queue, EMAIL, MRSFASTDB, editdist, mincut, maxcut, mincpu, maxcpu, indir, outdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);

	printf("COMMAND: %s\n", command);
	
	if (!finishup && !nomap)
	  system(command);
      }

	//sprintf(maplogfile, "%s-%s-%d.sge.o", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      // i need the job id 

      strcpy(maplogfile, "");
      isfinished = 0;
      if (finishup || nomap)
	isfinished = 1;

      while (!isfinished){
	time ( &rawtime );
	printf("Sleeping for 5 minutes. Time now: %s\n", asctime(localtime(&rawtime)));
	sleep(300);
	printf("Checking map log.\n");
	if (!strcmp(maplogfile, ""))
	  jobid = findlog(curdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen, maplogfile);	
	if (strcmp(maplogfile, "")){
	  printf("Inspecting %s.\n", maplogfile);
	  maplog = fopen(maplogfile, "r");
	  if (maplog != NULL){
	    while (fscanf(maplog, "%s", line) > 0){
	      if (!strcmp(line, "collected,")){
		isfinished = 1 ;
		sleep(60);
		sprintf(command, "qdel %d", jobid);
		system(command);
		break;
	      }
	    }
	    fclose(maplog);
	  }
	  else
	    printf("Job not yet started : %s %s %d.\n",  mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
	}
	else
	  printf("Job not yet started : %s %s %d.\n",  mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
      }
      
      
      if (nomap) return 0;


      /*      
      printf("Removing split fastqs.\n");
      sprintf(command, "rm -fr %s", indir);
      system(command);
      */

      printf("Removing sampled fastqs.\n");
      fprintf(vh, "rm -fr %s/%s_sample\n", curdir, indir);
      //sprintf(command, "rm -fr %s_sample", indir);
      //      system(command);

      printf("Removing sampled out.\n");
      fprintf(vh, "rm -fr %s/%s/%s/%d/out_sample\n", curdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      //sprintf(command, "rm -fr %s/%s/%d/out_sample", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      //system(command);
      
      printf("Removing downloaded fastqs.\n");
      fprintf(vh, "rm -fr %s/%s/%s/%d/download\n", curdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      //sprintf(command, "rm -fr %s/%s/%d/download", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      //system(command);
      


      //      printf("Compressing SAMs.\n");
      //      fprintf(vh, "rm -f %s/*.sam\n", outdir);
      //system(command);

      
      printf("Concatenating DIVETs.\n");
      sprintf(command, "cat %s/*DIVET.vh |  awk '{if (($10==\"I\" || ($10==\"D\" && $3-$8>-100000 && $3-$8<100000) || ($10==\"V\" && $3-$8>-10000000 && $3-$8<10000000 && ($3-$8<%d || $3-$8>%d))) && $11<=%d) print}' > %s/%s-%s-%d.vh", outdir, (-1 * maxcut), maxcut,(2*editdist - 2), vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);

      if (bz)	
	fprintf(vh, "bzcat %s/%s/*DIVET.vh*   | awk '{if (($10==\"I\" || ($10==\"D\" && $3-$8>-100000 && $3-$8<100000) || ($10==\"V\" && $3-$8>-10000000 && $3-$8<10000000 && ($3-$8<%d || $3-$8>%d))) && $11<=%d) print}' > %s/%s/%s-%s-%d.vh\n\n", curdir, outdir, (-1 * maxcut), maxcut,(2*editdist - 2), curdir, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
      else{
	fprintf(vh, "cat %s/%s/*DIVET.vh   | awk '{if (($10==\"I\" || ($10==\"D\" && $3-$8>-100000 && $3-$8<100000) || ($10==\"V\" && $3-$8>-10000000 && $3-$8<10000000 && ($3-$8<%d || $3-$8>%d))) && $11<=%d) print}' > %s/%s/%s-%s-%d.vh\n\n", curdir, outdir, (-1 * maxcut), maxcut,(2*editdist - 2), curdir, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
	fprintf(vh, "bzcat %s/%s/*DIVET.vh.bz2   | awk '{if (($10==\"I\" || ($10==\"D\" && $3-$8>-100000 && $3-$8<100000) || ($10==\"V\" && $3-$8>-10000000 && $3-$8<10000000 && ($3-$8<%d || $3-$8>%d))) && $11<=%d) print}' >> %s/%s/%s-%s-%d.vh\n\n", curdir, outdir, (-1 * maxcut), maxcut,(2*editdist - 2), curdir, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
      }
      printf("COMMAND: %s\n", command);
      //      system(command);
      printf("Creating temp libs.\n");

      /*
      mincut = mean - 5*stdev - 1;
      maxcut = mean + 5*stdev + 1;
      */

      sprintf(command, "echo %s-%d %s %s/%s/%s-%s-%d.vh %d %d %d > %s/%s-%s-%d.tmplib", mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, curdir, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen, mincut, maxcut, mylanes[i].readlen, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
      system(command);

      //fprintf(vh, "pbzip2 -p4 %s/%s/*DANT\n", curdir, outdir);

      
      fprintf(vh, "#for i in `ls %s/%s/*.vh`; do cut -f 1 $i | sort -u > $i.disclist; done\n", curdir, outdir);
      fprintf(vh, "#for i in `ls %s/%s/`; do %s -i %s/%s/$i -r %s/%s/$i.sam_DIVET.vh.disclist -o %s/%s/$i.discordant.fastq; done\n", curdir, indir, FASTQDELHASH, curdir, indir, curdir, outdir, curdir, outdir);
      fprintf(vh, "#rm -f %s/%s/*disclist\n", curdir, outdir);
      
      
      fprintf(vh, "for i in `ls %s/%s/`; do cat %s/%s/$i.sam | %s -min %d -max %d > %s/%s/$i.discordant.fastq; done\n", curdir, indir, curdir, outdir, SAM2FQ, mincut, maxcut, curdir, outdir);
      
      fprintf(vh, "rm -fr %s/%s\n", curdir, indir);
      fprintf(vh, "pbzip2 -p4 -m2000 %s/%s/*.vh\n", curdir, outdir);
      fprintf(vh, "pbzip2 -p4 -m2000 %s/%s/*.unmapped.fastq\n", curdir, outdir);
      fprintf(vh, "pbzip2 -p4 -m2000 %s/%s/*.discordant.fastq\n", curdir, outdir);
      fprintf(vh, "pbzip2 -p4 -m2000 %s/%s/*.sam\n", curdir, outdir);


      /*  sleep and check if done
	 cat divet
	 create .lib file
	 modify mrfastbatch to rm .sam and pbzip2 -p4 others
	 create vh.sge file and submit to bigmem.q
      */

    }
  }

  /* this one assumes there is one sample in the index file */

  if (nomap)
    return 0;

  printf("Creating VH lib file.\n");
  mergelibs(vhdir, mylanes[0].samplename);
  
  sprintf(command, "echo %s : 1 > %s/%s/%s.weight", mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename);
  system(command);

  printf("Spawning VH job.\n");
  
  fprintf(vh, "/net/eichler/vol3/home/calkan/bin/VariationHunter-CR -p 0.001 -x 300 -c %s -i %s -l %s/%s/%s.lib -o %s/%s/%s.out -r %s -g %s -t %s/%s/%s.name\n\n", ALLCHROM, INITINFO, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, SATFILE, GAPFILE, curdir, vhdir, mylanes[0].samplename);

  fprintf(vh, "/net/eichler/vol3/home/calkan/bin/vh_setcover_pool -cr -ms 0.05 -l %s/%s/%s.lib -r %s/%s/%s.name -c %s/%s/%s.out -t 15000 -o %s/%s/%s.SV\n\n", curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename);

  //  fprintf(vh, "/net/eichler/vol3/home/calkan/bin/vh_setcover_pool -l %s/%s/%s.lib -r -c %s/%s/%s.name %s/%s/%s.out -w %s/%s/%s.weight -t 15000 -o %s/%s/%s.SV\n\n", curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename);


  fclose(vh);
  //sprintf(command, "qsub -q bigmem.q -l hostname puma %s", vhrunfile);
  sprintf(command, "qsub -q hm.q,bigmem.q %s", vhrunfile);
  system(command);
}


int countdir (char *directory){

  struct dirent *ep;
  DIR *dp;
  int  count  = 0;

  dp = opendir(directory);

  if (dp == NULL){
    fprintf(stderr, "Fastq directory %s not found.\n", directory);
    return 0;
  }

  while((ep=readdir(dp))){
    if (ep->d_name[0] == '.')
      continue;
    if (ep->d_type == DT_DIR)
      continue;
    count ++;
  }

  return count;
}


void mergelibs (char *directory, char *samplename){

  struct dirent *ep;
  DIR *dp;
  int  count  = 0;
  char command[2000];

  dp = opendir(directory);

  if (dp == NULL){
    fprintf(stderr, "VH directory %s not found.\n", directory);
    return;
  }

  while((ep=readdir(dp))){
    if (ep->d_name[0] == '.')
      continue;
    if (ep->d_type == DT_DIR)
      continue;
    if (strstr(ep->d_name, ".tmplib"))
      count ++;
  }

  rewinddir(dp);

  sprintf(command, "echo %d > %s/%s.lib", count, directory, samplename);
  system(command);

  while((ep=readdir(dp))){
    if (ep->d_name[0] == '.')
      continue;
    if (ep->d_type == DT_DIR)
      continue;
    if (strstr(ep->d_name, ".tmplib")){
      sprintf(command, "cat %s/%s >> %s/%s.lib", directory, ep->d_name, directory, samplename);
      system(command);  
    }
  }

}


int findlog (char *directory, char *samplename, char *libname, int readlen, char *maplogfile){

  struct dirent *ep;
  DIR *dp;
  char readlenstr[100];
  char idfile[100];
  FILE *idf;
  int jobid;

  printf("Searching for log file in %s\n", directory);

  strcpy(maplogfile, "");
  jobid = 0;

  sprintf(idfile, "%s-%s-%d.ID", samplename, libname, readlen);
  idf = fopen(idfile, "r");
  
  if (idf != NULL){
    while (fscanf(idf, "%s", readlenstr )> 0){
      if (!strcmp(readlenstr, "Your")){
	fscanf(idf, "%s", readlenstr); // job
	fscanf(idf, "%d", &jobid); // job id
	break;
      }
    }
    fclose(idf);
  }

  if (jobid!=0)
    sprintf(maplogfile, "%s-%s-%d.sge.o%d", samplename, libname, readlen, jobid);

  return jobid;
  /*
  
  dp = opendir(directory);



  if (dp == NULL){
    fprintf(stderr, "Fastq directory %s not found.\n", directory);
    return;
  }

  //  sprintf(readlenstr, "%d", readlen);
  sprintf(readlenstr, "%s-%s-%d.sge.o", samplename, libname, readlen);


  while((ep=readdir(dp))){
    //printf("findlog: filename: %s\n", ep->d_name);
    if (ep->d_name[0] == '.')
      continue;
    if (ep->d_type == DT_DIR)
      continue;
    //if (strstr(ep->d_name, samplename) && strstr(ep->d_name, libname) && strstr(ep->d_name, readlenstr) && strstr(ep->d_name, ".o")){
    if (strstr(ep->d_name, readlenstr)){
      strcpy(maplogfile, ep->d_name);
      break;
    }
  }
  */

}




int findlog_sample (char *directory, char *samplename, char *libname, int readlen, char *maplogfile){

  struct dirent *ep;
  DIR *dp;
  char readlenstr[100];
  char idfile[100];
  FILE *idf;
  int jobid;

  printf("Searching for sample log file in %s\n", directory);

  strcpy(maplogfile, "");
  jobid = 0;

  sprintf(idfile, "%s-%s-%d.sampleID", samplename, libname, readlen);
  idf = fopen(idfile, "r");
  
  if (idf != NULL){
    while (fscanf(idf, "%s", readlenstr )> 0){
      if (!strcmp(readlenstr, "Your")){
	fscanf(idf, "%s", readlenstr); // job
	fscanf(idf, "%d", &jobid); // job id
	break;
      }
    }
    fclose(idf);
  }

  if (jobid!=0)
    sprintf(maplogfile, "%s-%s-%d_sample.sge.o%d", samplename, libname, readlen, jobid);

  return jobid;
}



static int lane_compar(const void *p1, const void *p2){

  /*
  char samplename[100];
  char libname[100];
  int readlen;
  int readlen2;
  char runid[1000];
  int numreads;
  */

  struct lane *a, *b;

  int ret;

  a = (struct lane *)p1;
  b = (struct lane *)p2;

  if (!strcmp(a->libname, b->libname))
    ret = a->readlen > b->readlen;
  else
    ret = strcmp(a->libname, b->libname);

  return ret;
}



int  mytok(char *str, char delim, char *save, int start){
  int i,j;
  int len = strlen(str);
  save[0]=0; j=0;
  for (i=start; i<len; i++){
    if (delim == str[i]){
      save[j]=0;
      //printf("tok2: %s\n", save);                                                                                                                   
      break;
    }
    else
      save[j++] = str[i];
  }
  //  printf("tok: %s\ti:%d\tlen:%d\n", save, i, len);                                                                                                
  save[j]=0;
  return i+1;
}
