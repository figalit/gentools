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
  char index[150];
  int threads;
  char seqname[150];
  char fullsfile[250];
  
  char *sfile=(char *)malloc(sizeof(char)*250);
  char *temp=(char *)malloc(sizeof(char)*250);
  char *stokenizer=(char *)malloc(sizeof(char)*250);

  char outfolder[250];
  char outeachfolder[250];
  char outname[250];
  char outprunedname[250];
  int start;
  FILE *in;
  char genomeidx[150];
  int i;
  char command[1000];
  int returncode=0;
  int minwin = 0;

  if (argc < 5){
    fprintf(stderr, "%s\n\n", argv[0]);
    fprintf(stderr, "\t-genome [idx]:\t genome index\n");
    fprintf(stderr, "\t-search [sfile]:\t file to be searched\n");
    fprintf(stderr, "\t-threads [t]: # of threads\n");
    fprintf(stderr, "\t-minwin [t]: min # of window hits\n");
    fprintf(stderr,"\t-outputfolder [out]: output folder \n");
    if (argc<3)
	return 0;
  }

  threads = 1;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-genome"))
      strcpy(genomeidx, argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(fullsfile, argv[i+1]);
    else if (!strcmp(argv[i], "-threads"))
      threads = atoi(argv[i+1]);    
    else if (!strcmp(argv[i], "-minwin"))
      minwin = atoi(argv[i+1]);    
    else if (!strcmp(argv[i], "-out"))
      strcpy(outfolder,argv[i+1]);    
  }



  sprintf(sfile,"%s",fullsfile);	
  sprintf(stokenizer,"%s",fullsfile);	
   
  temp=strtok(stokenizer,"\\/");

   while ((temp=strtok(_NULL,"\\/")))	
	sprintf(sfile,"%s",temp);		


   printf("Sfile is %s \n",sfile);

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
  
  while(fscanf(in, "%s%s%d", index, seqname, &start) > 0){
    if (minwin!=0)
      sprintf(command, "/mnt/local/inhousebin/pfast-eeek -loadindex %s -search %s -istart %d -threads %d -seqname %s -outfolder %s -minwin %d", index, fullsfile, start, threads, seqname,outeachfolder, minwin);
    else
      sprintf(command, "/mnt/local/inhousebin/pfast-eeek -loadindex %s -search %s -istart %d -threads %d -seqname %s -outfolder %s", index, fullsfile, start, threads, seqname,outeachfolder);
    printf("Command is %s \n",command); 
   system(command);
  }

  sprintf(outname,"%s/%s.out",outfolder,sfile);
  sprintf(outprunedname,"%s/%s.pruned",outfolder,sfile);
  fclose(in);

  sprintf(command, "cat %s/*out | sort -k 1,1 -k 6,6nr > %s",outeachfolder,outname); 
  printf("Command is %s \n",command);
  system(command);

  sprintf(command,"/mnt/local/inhousebin/prunepfast-eeek %s %s",outname,outprunedname);
  printf("Command is %s \n",command);
  system(command);

  return 1;
}
