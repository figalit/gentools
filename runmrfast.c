#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#define _NULL 0

int main(int argc, char **argv){
  char index[500];
  int threads;
  char seqname[150];
  char fullsfile[250];
  
  char *sfile=(char *)malloc(sizeof(char)*250);
  char *temp=(char *)malloc(sizeof(char)*250);
  char *stokenizer=(char *)malloc(sizeof(char)*250);

  char ed[10];

  char outfolder[250];
  char outeachfolder[250];
  char outname[250];
  char outchrname[250];
  char outsecondname[250];
  char outprunedname[250];
  char outmatchedname[250];
  char *newindex;
  char tmpindex[250];
  int start;
  FILE *in;
  char genomeidx[150];
  int i;
  char command[5000];
  int returncode=0;
  int minwin = 0;
  struct timeval tstart, end;
  struct timezone tz;
  long cputime;
  int upper, lower, maxdist;
  int seqcnt;
  FILE *infile;
  char ch;
  char dummy[250];
  int DOMATCH=1;
  int DOCAT = 1;
  float minident=0.9;
  float minlen=0.9;
  int length;
  int NORANDOM=0;
  int NOM=0;
  int NOSOLEXA=0;
  int GZOUT=0;
  int BAC=0;
  int PFAST_GZ=0;
  int NOMATCH=0;
  int NOCOUNT=0;
  char textout[100];
  char norandom[100];
  char nom[100];
  char summary[100];
  char best[20];
  int rmout = 0;
  char noconcordant[20];
  int CHRBYCHR=0;
  int nowrite;

  if (argc < 5){
    fprintf(stderr, "%s\n\n", argv[0]);
    fprintf(stderr, "\t-genome [idx]:\t genome index\n");
    fprintf(stderr, "\t-search [sfile]:\t file to be searched\n");
    fprintf(stderr, "\t-threads [t]: # of threads\n");
    fprintf(stderr, "\t-minwin [t]: min # of window hits\n");
    fprintf(stderr,"\t-out [out]: output folder \n");
    if (argc<3)
	return 0;
  }

  threads = 1;

  outfolder[0]='.';

  length = 27;

  textout[0]=0;
  summary[0]=0;

  norandom[0] = 0;
  nom[0] = 0;
  nowrite=0;

  best[0] = 0;
  noconcordant[0] = 0;

  upper = 300; lower = 100; maxdist = 1000000;

  ed[0]=0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-genome"))
      strcpy(genomeidx, argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(fullsfile, argv[i+1]);
    else if (!strcmp(argv[i], "-threads"))
      threads = atoi(argv[i+1]);    
    else if (!strcmp(argv[i], "-minwin"))
      minwin = atoi(argv[i+1]);    
    else if (!strcmp(argv[i], "-minident"))
      minident = atof(argv[i+1]);    
    else if (!strcmp(argv[i], "-minlen"))
      minlen = atof(argv[i+1]);    
    else if (!strcmp(argv[i], "-out"))
      strcpy(outfolder,argv[i+1]);    
    else if (!strcmp(argv[i], "-l"))
      lower = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-u"))
      upper = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-cat"))
      DOCAT=1;
    else if (!strcmp(argv[i], "-norandom")){
      NORANDOM=1;
      strcpy(norandom, "-norandom");
    }
    else if (!strcmp(argv[i], "-noM")){
      NOM=1;
      strcpy(nom, "-noM");
    }
    else if (!strcmp(argv[i], "-nocnt"))
      NOCOUNT=1;
    else if (!strcmp(argv[i], "-nosolexa"))
      NOSOLEXA=1;
    else if (!strcmp(argv[i], "-nomatch"))
      NOMATCH=1;
    else if (!strcmp(argv[i], "-gzout"))
      GZOUT=1;
    else if (!strcmp(argv[i], "-textout"))
      strcpy(textout, "-textout");
    else if (!strcmp(argv[i], "-summary"))
      strcpy(summary, "-summary");
    else if (!strcmp(argv[i], "-best"))
      strcpy(best, "-best");
    else if (!strcmp(argv[i], "-ed"))
      sprintf(ed, "-ed %s", argv[i+1]);
    else if (!strcmp(argv[i], "-noconc"))
      strcpy(noconcordant, "-noconc");
    else if (!strcmp(argv[i], "-rmout"))
      rmout = 1;
    else if (!strcmp(argv[i], "-nw"))
      nowrite = 1;
    else if (!strcmp(argv[i], "-gz"))
      PFAST_GZ=1;
    else if (!strcmp(argv[i], "-bac"))
      BAC=1;
    else if (!strcmp(argv[i], "-chrbychr")){
     CHRBYCHR=1;
     
    }
  }


  if (CHRBYCHR)
    DOCAT=0;
  
  printf("docat1: %d\n", DOCAT);


  sprintf(sfile,"%s",fullsfile);	
  sprintf(stokenizer,"%s",fullsfile);	
  
  temp=strtok(stokenizer,"\\/");

  while ((temp=strtok(_NULL,"\\/")))	
    sprintf(sfile,"%s",temp);		
  
  infile = fopen(fullsfile, "r");
  
  seqcnt = 0;
  while (fscanf(infile, "%c", &ch) > 0){
    if (ch == '>'){
      seqcnt++;
      fgets(dummy, 250, infile);
    }
  }
  
  fclose(infile);

  printf("Sfile is %s cnt: %d\n",sfile, seqcnt);
  
  sprintf(outeachfolder,"%s/%s/",outfolder,sfile);
  

  returncode=mkdir (outfolder, S_IRWXU|S_IRGRP|S_IXGRP);
  if (returncode<0)
    printf("%s Folder already exists \n",outfolder);
  returncode=mkdir (outeachfolder, S_IRWXU|S_IRGRP|S_IXGRP);


  if (returncode<0)
    printf("%s Folder already exists \n",outeachfolder);

  in = fopen(genomeidx, "r");
  if (in == NULL)
    return 0;
  fclose(in);
  

  gettimeofday(&tstart, &tz); 


  //sprintf(command, "/net/eichler/vol3/home/calkan/bin/mrfast -loadidx %s -search %s -outfolder %s -nocnt -norandom -noM -textout", genomeidx, fullsfile, outeachfolder);

  if (nowrite)
    sprintf(command, "/net/eichler/vol3/home/calkan/bin/mrfast-nowrite -loadidx %s -search %s -outfolder %s -nocnt %s %s %s %s -noY", genomeidx, fullsfile, outeachfolder, textout, summary, norandom, nom);
  else
    sprintf(command, "/net/eichler/vol3/home/calkan/bin/mrfast -loadidx %s -search %s -outfolder %s -nocnt %s %s %s %s %s", genomeidx, fullsfile, outeachfolder, textout, summary, norandom, nom, ed);

  printf("Command is %s \n",command);

  system(command);

  

  sprintf(outname,"%s/%s.out",outfolder,sfile);




  if (DOCAT){
    printf("Concatenating.\n");
    sprintf(outsecondname,"%s/%s.tmp",outeachfolder,sfile);
    infile = fopen(outsecondname, "w");

    if (!NOCOUNT && textout[0]==0)
      fwrite(&seqcnt, sizeof(seqcnt), 1, infile);
    
    else if (!NOCOUNT && textout[0]!=0)
      fprintf(infile, "%d\n", seqcnt);

    fclose(infile);

    sprintf(command, "cat %s >> %s",  outsecondname , outname); 
    system(command);

    if (!BAC){
      for (i=1;i<=22;i++){
	//sprintf(command, "cat %s %s/*chr%d*out* >> %s", outsecondname, outeachfo3Dlder, i, outname); 
	sprintf(command, "cat %s/*chr%d_*out* >> %s 2>/dev/null",  outeachfolder, i, outname); 
	printf("CAT Command is %s \n",command);
	system(command);
      }
      sprintf(command, "cat %s/*chrX*out* >> %s 2>/dev/null", outeachfolder, outname); 
      printf("CAT Command is %s \n",command);
      system(command);
      sprintf(command, "cat %s/*chrY*out* >> %s 2>/dev/null", outeachfolder, outname); 
      printf("CAT Command is %s \n",command);
      system(command);
      if (!NOM){
	sprintf(command, "cat %s/*chrM*out* >> %s 2>/dev/null", outeachfolder, outname); 
	printf("CAT Command is %s \n",command);
	system(command);      
      }
    }
    else{
	sprintf(command, "cat %s/*out* >> %s 2>/dev/null", outeachfolder, outname); 
	printf("CAT Command is %s \n",command);
	system(command);      
    }

  }



  if (!NOMATCH && CHRBYCHR){

    sprintf(outsecondname,"%s/%s.tmp",outeachfolder,sfile);
    infile = fopen(outsecondname, "w");

    if (!NOCOUNT && textout[0]==0)
      fwrite(&seqcnt, sizeof(seqcnt), 1, infile);
    
    else if (!NOCOUNT && textout[0]!=0)
      fprintf(infile, "%d\n", seqcnt);

    fclose(infile);

    for (i=1;i<=22;i++){

      sprintf(outchrname,"%s/%s.chr%d.out",outfolder,sfile, i);

      sprintf(command, "cat %s %s/*chr%d_*out* >> %s 2>/dev/null",  outsecondname, outeachfolder, i, outchrname); 
      printf("CAT Command is %s \n",command);
      system(command);

      sprintf(command, "/net/eichler/vol3/home/calkan/bin/solexamatch -i %s -o %s.match -u %d -l %d -m 1000000 %s %s -min 72", outchrname, outchrname, upper, lower, best, noconcordant);
      printf("\nSOLEXAMATCH  command is %s\n\n", command);
      system(command);


      remove(outchrname);
    }


    sprintf(outchrname, "%s/%s.chrX.out",outfolder,sfile);
    
    sprintf(command, "cat %s %s/*chrX_*out* >> %s 2>/dev/null",  outsecondname, outeachfolder, outchrname); 
    printf("CAT Command is %s \n",command);
    system(command);
    
    sprintf(command, "/net/eichler/vol3/home/calkan/bin/solexamatch -i %s -o %s.match -u %d -l %d -m 1000000 %s %s -min 72", outchrname, outchrname, upper, lower, best, noconcordant);
    printf("\nSOLEXAMATCH  command is %s\n\n", command);
    system(command);
    
    remove(outchrname);

    sprintf(outchrname,"%s/%s.chrY.out",outfolder,sfile);
    
    sprintf(command, "cat %s %s/*chrY_*out* >> %s 2>/dev/null",  outsecondname, outeachfolder, outchrname); 
    printf("CAT Command is %s \n",command);
    system(command);
    
    sprintf(command, "/net/eichler/vol3/home/calkan/bin/solexamatch -i %s -o %s.match -u %d -l %d -m 1000000 %s %s -min 72", outchrname, outchrname, upper, lower, best, noconcordant);
    printf("\nSOLEXAMATCH  command is %s\n\n", command);
    system(command);

    remove(outchrname);

    sprintf(command, "cat %s/%s*chr*match > %s.match 2>/dev/null", outfolder, sfile, outname); 
    printf("CAT Command is %s \n",command);
    system(command);          

    sprintf(command, "rm %s/%s.*chr*.match ", outfolder, sfile); 
    printf("CAT-RM Command is %s \n",command);
    system(command);          

  }


  if (!NOMATCH && !strstr(fullsfile, "broad") && !CHRBYCHR){

    //sprintf(command, "/net/eichler/vol3/home/calkan/bin/solexamatch -i %s -o %s.match -u 134 -l 74 -m 1000000 -text", outname, outname);

    sprintf(command, "ls -lh %s", outname);
    system(command);
    sprintf(command, "/net/eichler/vol3/home/calkan/bin/solexamatch -i %s -o %s.match -u %d -l %d -m 1000000 %s %s -min 72", outname, outname, upper, lower, best, noconcordant);
    printf("\nSOLEXAMATCH  command is %s\n\n", command);
    system(command);
  

    
  }
  
  if (rmout){
    sprintf(command, "rm -fr %s", outeachfolder);
    system(command);      

    
    remove(outname);
    sprintf(command, "touch %s", outname);

    

    system(command);
  }
  
  /*
  if (GZOUT && NOSOLEXA){
    sprintf(command, "gzip %s", outname);
    printf("\nGZIP  command is %s\n\n", command);
    system(command);    
  }
  */


  gettimeofday(&end, &tz); 

  cputime = (long)(end.tv_sec)-(long)(tstart.tv_sec);

  printf("RUNTIME : %ld seconds.\n", cputime);

  return 1;
}
