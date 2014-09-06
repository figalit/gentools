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
  char outsecondname[250];
  char outprunedname[250];
  char outmatchedname[250];
  char *newindex;
  char tmpindex[250];
  int start;
  FILE *in;
  char genomeidx[150];
  int i;
  char command[1000];
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


  upper = 4000; lower = 2000; maxdist = 1000000;

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
    else if (!strcmp(argv[i], "-u"))
      upper = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      lower = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-m"))
      maxdist = atoi(argv[i+1]);
  }



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

    strcpy(tmpindex, index);
    tmpindex[strlen(index) - 6] = 0; // remove ".index"
    
    for (i=strlen(tmpindex)-1; i>=0; i--)
      if (tmpindex[i] == '/')
	break;
    newindex = tmpindex + i + 1;

    if (minwin!=0)
      sprintf(command, "/mnt/local/inhousebin/pfast-eeek -loadindex %s -search %s -istart %d -threads %d -seqname %s -outfolder %s -minwin %d -singlefile -minident 0.9", index, fullsfile, start, threads, seqname, outeachfolder, minwin);
    else
      sprintf(command, "/mnt/local/inhousebin/pfast-eeek -loadindex %s -search %s -istart %d -threads %d -seqname %s -outfolder %s -singlefile -minident 0.9", index, fullsfile, start, threads, seqname,outeachfolder);
    printf("Command is %s \n",command); 

    system(command);
    
    sprintf(command, "/mnt/local/inhousebin/pfastmatch-eeek -i %s/%s.%s.out -o %s/%s.%s.match -u %d -l %d -m %d", outeachfolder, sfile, newindex, outeachfolder, sfile, newindex, upper, lower, maxdist);

    printf("Command is %s \n",command); 
    system(command);
    
  }
  
  sprintf(outname,"%s/%s.out",outfolder,sfile);
  sprintf(outprunedname,"%s/%s.pruned",outfolder,sfile);
  fclose(in);

  
  sprintf(outsecondname,"%s/%s.tmp",outeachfolder,sfile);
  infile = fopen(outsecondname, "w");
  fwrite(&seqcnt, sizeof(seqcnt), 1, infile);
  fclose(infile);

  sprintf(outsecondname,"%s/%s.second",outeachfolder,sfile);

  sprintf(command, "cat %s/%s.tmp %s/*single > %s", outeachfolder, sfile, outeachfolder,outsecondname); 
  printf("Command is %s \n",command);
  system(command);
  
  
  sprintf(command, "/mnt/local/inhousebin/pfastmatch-eeek -i %s -o %s.match -u %d -l %d -m %d", outsecondname, outsecondname, upper, lower, maxdist);
  printf("Command is %s \n",command);
  system(command);


  sprintf(command, "cat %s/%s.tmp %s.match.single > %s/secondtemp; mv %s/secondtemp %s.match.single", outeachfolder, sfile, outsecondname, outeachfolder, outeachfolder, outsecondname); 
  printf("Command is %s \n",command);
  system(command);

  sprintf(command, "cat %s/*match > %s/%s.match ", outeachfolder, outeachfolder, sfile); 
  printf("Command is %s \n",command);
  system(command);


  /*
  sprintf(command, "cat %s/*out | sort -k 1,1 -k 6,6nr > %s",outeachfolder,outname); 
  printf("Command is %s \n",command);
  system(command);
  */

  /*
  sprintf(command,"/mnt/local/inhousebin/prunepfast-eeek %s %s",outname,outprunedname);
  printf("Command is %s \n",command);
  system(command);
  */


  gettimeofday(&end, &tz); 

  cputime = (long)(end.tv_sec)-(long)(tstart.tv_sec);

  printf("RUNTIME : %ld seconds.\n", cputime);

  return 1;
}
