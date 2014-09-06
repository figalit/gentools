#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define ASPERACMD "ascp -k 1 -i /net/eichler/vol3/home/calkan/asperaetc/asperaweb_id_dsa.putty -l 100M anonftp@ftp-private.ncbi.nlm.nih.gov:/1000genomes/ftp/"

#define MAXLANES 1000
#define EMAIL "calkan@uw.edu"
#define MRSFASTDB "/var/tmp/calkan/MRSFAST/hg19"

#define ALLCHROM "/net/eichler/vol3/home/calkan/WSSD/Annotations/AllChro"
#define GAPFILE "/net/eichler/vol3/home/calkan/WSSD/Annotations/hg19.gap.bed"
#define SATFILE "/net/eichler/vol3/home/calkan/WSSD/Annotations/hg19_satellite.bed"
#define INITINFO "/net/eichler/vol3/home/calkan/WSSD/Annotations/initInfo"

#define DEFAULT_LINES "#! /bin/sh\n# Specify the shell for this job\n#$ -S /bin/sh\nexport MPICH_PROCESS_GROUP=no\nulimit -c 0\n#Tell Sun Grid Engine to send an email when the job begins\n# and when it ends.\n"
#define DEFAULT_LINES2 "#send it when it started&finished\n#$ -m beas\n#$ -hard\n# Specify the location of the output\n"
#define DEFAULT_LINES3 "# Location of executables\nprogpath=/mnt/local/bin\necho \"Got $NSLOTS slots\"\necho \"path=$PATH\"\necho \"P4_RSHCOMMAND=$P4_RSHCOMMAND\"\necho \"machine_file=$TMPDIR/machines\"\necho \"JOB_ID=$JOB_ID\"\necho \"TEMDPIR=$TMPDIR\"\necho \"HOSTNAME=$HOSTNAME\"\n"


typedef struct lane{
  char samplename[100];
  char libname[100];
  int readlen;
  char runid[1000];
}_lane;


int countdir (char *);
void mergelibs(char *, char *);

struct lane mylanes[MAXLANES];
void findlog (char *directory, char *samplename, char *libname, int readlen, char *maplogfile);

int main(int argc, char **argv){
  int i;
  char indexname[100];
  char basname[100];
  char logname[100];
  FILE *index, *bas, *log;

  FILE *maplog, *vh;
  char maplogfile[1000];
  
  char *token;

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
  float coverage;
  int readlen;

  char dirname[1000];
  int retval;
  mode_t MKDIRMODE =  S_IRWXU|S_IRGRP|S_IXGRP;

  int lanecnt=0;

  int numfiles;

  char basstring[1000];
 
  char thislib[1000]; int thislen;

  float mean, stdev;
  int median, meddev;
  
  int mincut, maxcut;
  int mincpu, maxcpu;

  int editdist;

  char indir[1000], outdir[1000], vhdir[1000], curdir[2000];
  char vhrunfile[2000];
  int isfinished;
  

  indexname[0]=0; basname[0]=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i")) strcpy(indexname, argv[i+1]);
    else if (!strcmp(argv[i], "-b")) strcpy(basname, argv[i+1]);
  }
  
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

  while (1){
    fgets(line, 10000, index);
    if (feof(index)) break;

    if (strstr(line, "exome")) continue;
    
    //printf("%s", line);
    
    token = strtok(line, "\t"); 
    strcpy(dataloc, token);

    token = strtok(NULL, "\t"); // md5
    token = strtok(NULL, "\t"); // runid
    strcpy(runid, token);

    token = strtok(NULL, "\t"); // studyid
    token = strtok(NULL, "\t"); // studyname
    token = strtok(NULL, "\t"); // centername
    token = strtok(NULL, "\t"); // submission id
    token = strtok(NULL, "\t"); // submission date
    token = strtok(NULL, "\t"); // sample id

    token = strtok(NULL, "\t"); // samplename  
    strcpy(samplename, token);

    token = strtok(NULL, "\t"); // population
    token = strtok(NULL, "\t"); // experimentid
    token = strtok(NULL, "\t"); // instrument platform
    token = strtok(NULL, "\t"); // instrument model

    token = strtok(NULL, "\t"); // library name
    strcpy(libname, token); 

    token = strtok(NULL, "\t"); // run name & run block name
    //token = strtok(NULL, "\t"); // run block name

    token = strtok(NULL, "\t"); // insert size
    fragsize = atoi(token);
    
    token = strtok(NULL, "\t"); // library layout
    
    if (strstr(dataloc, "_1.filt.fastq.gz") ||strstr(dataloc, "_2.filt.fastq.gz"))
      token = strtok(NULL, "\t"); // paired_fastq
    else
      continue;

    token = strtok(NULL, "\t"); // withdrawn
    iswithdrawn = atoi(token);

    if (iswithdrawn == 1) continue;
    
    //token = strtok(NULL, "\t"); // withdrawn_date
    
    token = strtok(NULL, "\t"); // comment or readcnt

    //token = strtok(NULL, "\t"); // readcnt
    
    
    if (!isdigit(token[0])){
      token = strtok(NULL, "\t"); // readcnt
      numreads = atoi(token);    
    }
    else 
      numreads = atoi(token);

    token = strtok(NULL, "\t"); // basecnt

    numbases = atol(token);


    //token = strtok(NULL, "\t"); // analysisgroup

    readlen = numbases / numreads;
    
    printf("reads %d bases %ld readlen %d\n", numreads, numbases, readlen);
    if (readlen<=51) continue;
    
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
    sprintf(dirname, "%s/VH", samplename);
    mkdir(dirname, MKDIRMODE);
    strcpy(vhdir, dirname);

    sprintf(command, "%s%s %s/%s/%d/download/", ASPERACMD, dataloc, samplename, libname, readlen);
    
    printf("COMMAND: %s\n", command);
    
    retval = 1;
    
    
    while(retval !=0)
      retval = system(command);
    

    if (strstr(dataloc, "_1.filt.fastq.gz")){

      strcpy(mylanes[lanecnt].samplename, samplename);
      strcpy(mylanes[lanecnt].libname, libname);
      strcpy(mylanes[lanecnt].runid, runid);
      mylanes[lanecnt].readlen = readlen;
      lanecnt++;
      
    }
    
  }
  
  printf("Total %d lanes.\n", lanecnt);


  fprintf(log, "Total %d lanes downloaded.\n", lanecnt);
  
 
  
  for (i=0;i<lanecnt;i++){

    sprintf(command, "mixcat2 -gz -f %s/%s/%d/download/%s_1.filt.fastq.gz -r %s/%s/%d/download/%s_2.filt.fastq.gz | fastqbreak -n 500000 -o %s/%s/%d/fastq/%s_%s_", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].runid, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].runid, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].runid);


    printf("COMMAND: %s\n", command);
    
    numreads = system(command);
    

  }


  getcwd(curdir, 2000);

  sprintf(vhrunfile, "%s-vh.sge", mylanes[0].samplename);
  vh = fopen (vhrunfile, "w");

  fprintf(vh, "%s\n", DEFAULT_LINES);
  fprintf(vh, "export P4_RSHCOMMAND=/usr/bin/ssh\nexport C3_RSH=\'ssh -q\'\n");
  fprintf(vh, "%s\n", DEFAULT_LINES2);
  fprintf(vh, "# pe request\n#$ -pe mpich 1-1\n#$ -M %s\n", EMAIL);
  fprintf(vh, "#$ -o %s\n#$ -e %s\n", curdir, curdir);
  fprintf(vh, "%s\n\n", DEFAULT_LINES3);
  



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
	continue;
      }

      if (stdev <= 15){
	mincut = mean - 10*stdev - 1;
	maxcut = mean + 10*stdev + 1;
      }
      else if (stdev <= 30){
	mincut = mean - 7*stdev - 1;
	maxcut = mean + 7*stdev + 1;
      }
      else if (stdev <=45){
	mincut = mean - 5*stdev - 1;
	maxcut = mean + 5*stdev + 1;
      }
      else{
	mincut = mean - 4*stdev - 1;
	maxcut = mean + 4*stdev + 1;
      }
      

      if (mincut < mylanes[i].readlen) mincut = 0;

      if (readlen <= 40) editdist = 2;
      else if (readlen <= 70) editdist = 3;
      else if (readlen <= 110) editdist = 4;
      else if (readlen <= 120) editdist = 5;
      else editdist = 6;

      sprintf(indir, "%s/%s/%d/fastq",  mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      sprintf(outdir, "%s/%s/%d/out",  mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);

      coverage = (numreads *  mylanes[i].readlen) / 3000000000.0;

      fprintf(log, "Sample: %s\tLibrary: %s\tReadlen: %d\tNumReads; %d\tCoverage: %f\tMean:%f\tStd:%f\tMedian:%d\tAbsDev:%d\tMin: %d\tMax: %d\tEdit_dist: %d\n", mylanes[i].samplename, mylanes[i].libname,  mylanes[i].readlen, numreads, coverage, mean, stdev, median, meddev, mincut, maxcut, editdist);

      numfiles = countdir(indir);
      printf("Total %d files for library %s readlength %d.\n", numfiles, mylanes[i].libname, mylanes[i].readlen);

      if (numfiles <= 280) maxcpu = numfiles+1; else if (numfiles >= 560) maxcpu = 280; else maxcpu = numfiles / 2;
      if (numfiles <= 120) mincpu = numfiles+1; else mincpu = 120;

      if (numfiles != 0){
	sprintf(command, "mrmap -submit -q all.q -ssh -options \"--maxoea 1000\" -email %s -g %s -e %d -pe -min %d -max %d -mincpu %d -maxcpu %d -i %s -o %s -sge %s-%s-%d.sge > %s-%s-%d.ID", EMAIL, MRSFASTDB, editdist, mincut, maxcut, mincpu, maxcpu, indir, outdir, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);

	printf("COMMAND: %s\n", command);
	
	system(command);
      }

	//sprintf(maplogfile, "%s-%s-%d.sge.o", mylanes[i].samplename, mylanes[i].libname, mylanes[i].readlen);
      // i need the job id 

      strcpy(maplogfile, "");
      isfinished = 0;
      
      while (!isfinished){
	printf("Sleeping for 10 minutes.\n");
	sleep(600);
	printf("Checking map log.\n");
	if (!strcmp(maplogfile, ""))
	  findlog(curdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen, maplogfile);	
	if (strcmp(maplogfile, "")){
	  printf("Inspecting %s.\n", maplogfile);
	  maplog = fopen(maplogfile, "r");
	  while (fscanf(maplog, "%s", line) > 0){
	    if (!strcmp(line, "succesfully")){
	      isfinished = 1 ;
	      break;
	    }
	  }
	  fclose(maplog);
	}
	else
	  printf("Job not yet started.\n");
      }
      
      

      
      printf("Removing split fastqs.\n");
      sprintf(command, "rm -fr %s", indir);
      system(command);

      printf("Removing downloaded fastqs.\n");
      sprintf(command, "rm -fr %s/%s/%d/download", samplename, libname, readlen);
      system(command);
      

      printf("Removing redundant SAMs.\n");
      sprintf(command, "rm -f %s/*.sam", outdir);
      system(command);

      
      printf("Concatenating DIVETs.\n");
      sprintf(command, "cat %s/*DIVET.vh | grep -v \"random\\|Un\\|hap\" | awk '{if ($9==\"I\" || ($9==\"D\" && $3-$7>-500000 && $3-$7<500000) || ($9==\"V\" && $3-$7>-10000000 && $3-$7<10000000 && ($3-$7<%d || $3-$7>%d)) && $10<=%d) print}' > %s/%s-%s-%d.vh", outdir, (-1 * maxcut), maxcut,(2*editdist - 2), vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
      printf("COMMAND: %s\n", command);
      system(command);
      printf("Creating temp libs.\n");

      /*
      mincut = mean - 5*stdev - 1;
      maxcut = mean + 5*stdev + 1;
      */

      sprintf(command, "echo %s-%d %s %s/%s/%s-%s-%d.vh %d %d %d > %s/%s-%s-%d.tmplib", mylanes[i].libname, mylanes[i].readlen, mylanes[i].samplename, curdir, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen, mincut, maxcut, mylanes[i].readlen, vhdir, mylanes[i].samplename,  mylanes[i].libname, mylanes[i].readlen);
      system(command);

      fprintf(vh, "bzip2 %s/%s/*DANT\n", curdir, outdir);
      fprintf(vh, "bzip2 %s/%s/*.vh\n", curdir, outdir);
      fprintf(vh, "bzip2 %s/%s/*.fastq\n", curdir, outdir);


      /*  sleep and check if done
	 cat divet
	 create .lib file
	 modify mrfastbatch to rm .sam and bzip2 others
	 create vh.sge file and submit to bigmem.q
      */

    }
  }

  /* this one assumes there is one sample in the index file */

  printf("Creating VH lib file.\n");
  mergelibs(vhdir, mylanes[0].samplename);
  
  sprintf(command, "echo %s : 1 > %s/%s/%s.weight", mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename);
  system(command);

  printf("Spawning VH job.\n");
  
  fprintf(vh, "/net/eichler/vol3/home/calkan/bin/VariationHunter-CR -p 0.001 -c %s -i %s -l %s/%s/%s.lib -o %s/%s/%s.out -r %s -g %s -t %s/%s/%s.name\n\n", ALLCHROM, INITINFO, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, SATFILE, GAPFILE, curdir, vhdir, mylanes[0].samplename);

  fprintf(vh, "/net/eichler/vol3/home/calkan/bin/vh_setcover_pool %s/%s/%s.lib %s/%s/%s.name %s/%s/%s.out %s/%s/%s.weight 15000 > %s/%s/%s.SV\n\n", curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename, curdir, vhdir, mylanes[0].samplename);


  fclose(vh);
  //sprintf(command, "qsub -q bigmem.q -l hostname puma %s", vhrunfile);
  sprintf(command, "qsub -l mem_requested=8G %s", vhrunfile);
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


void findlog (char *directory, char *samplename, char *libname, int readlen, char *maplogfile){

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
