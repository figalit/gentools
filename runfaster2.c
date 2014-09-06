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

  char outfolder[250];
  char outeachfolder[250];
  char outname[250];
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
  int DOCAT = 0;
  float minident=0.9;
  float minlen=0.9;
  int length;
  int NORANDOM=0;
  int NOM=0;
  int NOSOLEXA=0;
  int GZOUT=0;
  int BAC=0;
  int PFAST_GZ=0;

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
      length = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-cat"))
      DOCAT=1;
    else if (!strcmp(argv[i], "-norandom"))
      NORANDOM=1;
    else if (!strcmp(argv[i], "-noM"))
      NOM=1;
    else if (!strcmp(argv[i], "-nosolexa"))
      NOSOLEXA=1;
    else if (!strcmp(argv[i], "-gzout"))
      GZOUT=1;
    else if (!strcmp(argv[i], "-gz"))
      PFAST_GZ=1;
    else if (!strcmp(argv[i], "-bac"))
      BAC=1;
  }
  
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
  

  gettimeofday(&tstart, &tz); 


  while(fscanf(in, "%s%s%d", index, seqname, &start) > 0){

    if (NORANDOM && strstr(seqname,"random"))
      continue;
    if (NOM && strstr(seqname, "chrM"))
      continue;

    strcpy(tmpindex, index);
    tmpindex[strlen(index) - 6] = 0; // remove ".index"
    
    for (i=strlen(tmpindex)-1; i>=0; i--)
      if (tmpindex[i] == '/')
	break;
    newindex = tmpindex + i + 1;


    if (!DOCAT){
      sprintf(command, "pfaster -loadindex %s -search %s -istart %d -seqname %s -outfolder %s", index, fullsfile, start, seqname, outeachfolder);
      if (PFAST_GZ)
	strcat(command, " -gz");
      printf("Command is %s \n",command); 
      
      system(command);
      
    }
  
    else{

      sprintf(command, "pfaster -loadindex %s -search %s -istart %d -seqname %s -outfolder %s -nocnt", index, fullsfile, start, seqname, outeachfolder);
      if (PFAST_GZ)
	strcat(command, " -gz");
      printf("Command is %s \n",command); 
      system(command);      
    }
    
  }
  

  sprintf(outname,"%s/%s.out",outfolder,sfile);


  fclose(in);


  if (DOCAT){
    printf("Concatenating.\n");
    sprintf(outsecondname,"%s/%s.tmp",outeachfolder,sfile);
    infile = fopen(outsecondname, "w");
    fwrite(&seqcnt, sizeof(seqcnt), 1, infile);
    fclose(infile);

    sprintf(command, "cat %s >> %s",  outsecondname , outname); 
    system(command);

    if (!BAC){
      for (i=1;i<=22;i++){
	//sprintf(command, "cat %s %s/*chr%d*out* >> %s", outsecondname, outeachfo3Dlder, i, outname); 
	sprintf(command, "cat %s/*chr%d_*out* >> %s",  outeachfolder, i, outname); 
	printf("CAT Command is %s \n",command);
	system(command);
      }
      sprintf(command, "cat %s/*chrX*out* >> %s", outeachfolder, outname); 
      printf("CAT Command is %s \n",command);
      system(command);
      sprintf(command, "cat %s/*chrY*out* >> %s", outeachfolder, outname); 
      printf("CAT Command is %s \n",command);
      system(command);
      if (!NOM){
	sprintf(command, "cat %s/*chrM*out* >> %s", outeachfolder, outname); 
	printf("CAT Command is %s \n",command);
	system(command);      
      }
    }
    else{
	sprintf(command, "cat %s/*out* >> %s", outeachfolder, outname); 
	printf("CAT Command is %s \n",command);
	system(command);      
    }

  }


  if (!NOSOLEXA){
    sprintf(command, "/mnt/local/inhousebin/solexaset -i %s -gzout -nwh -l %d -o %s", outname, length, outfolder);
    printf("\nSOLEXA  command is %s\n\n", command);
    system(command);
    
    
    remove(outname);
    sprintf(command, "touch %s", outname);
    system(command);
    
  }

  if (GZOUT && NOSOLEXA){
    sprintf(command, "gzip %s", outname);
    printf("\nGZIP  command is %s\n\n", command);
    system(command);    
  }

  gettimeofday(&end, &tz); 

  cputime = (long)(end.tv_sec)-(long)(tstart.tv_sec);

  printf("RUNTIME : %ld seconds.\n", cputime);

  return 1;
}
