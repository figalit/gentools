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

#define X 23
#define Y 24

int main(int argc, char **argv){
  char index[500];


  int length[25];

  char **repfile;

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
  int NORANDOM=0;
  int NOM=0;
  int NOSOLEXA=0;
  int GZOUT=0;
  int BAC=0;
  int PFAST_GZ=0;
  int build35 = 0;

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

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-genome"))
      strcpy(genomeidx, argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(fullsfile, argv[i+1]);
    else if (!strcmp(argv[i], "-out"))
      strcpy(outfolder,argv[i+1]);    
    else if (!strcmp(argv[i], "-norandom"))
      NORANDOM=1;
    else if (!strcmp(argv[i], "-noM"))
      NOM=1;
    else if (!strcmp(argv[i], "-hg17"))
      build35 = 1;
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


  gettimeofday(&tstart, &tz); 


  sprintf(command, "mrfast -loadidx %s -search %s -outfolder %s -nocnt -norandom", genomeidx, fullsfile, outeachfolder);

  printf("Command is %s \n",command); 
      
  returncode = system(command);


  printf("Return:%d\nConcatenating.\n", returncode);
  
  /*
    sprintf(outsecondname,"%s/%s.tmp",outeachfolder,sfile);
    
    infile = fopen(outsecondname, "w");
    fwrite(&seqcnt, sizeof(seqcnt), 1, infile);
    fclose(infile);
    
    sprintf(command, "cat %s >> %s",  outsecondname , outname); 
    system(command);
  */
 

  for (i=1;i<=22;i++){
    sprintf(command, "cat %s/*chr%d_*out* >> %s/chr%d.%s.out",  outeachfolder, i, outfolder, i, sfile); 
    printf("CAT Command is %s \n",command);
    system(command);
  }
  sprintf(command, "cat %s/*chrX_*out* >> %s/chrX.%s.out", outeachfolder, outfolder, sfile); 
  printf("CAT Command is %s \n",command);
  system(command);
  sprintf(command, "cat %s/*chrY_*out* >> %s/chrY.%s.out", outeachfolder, outfolder, sfile); 
  printf("CAT Command is %s \n",command);
  system(command);
  if (!NOM){
    sprintf(command, "cat %s/*chrM_*out* >> %s/chrM.%s.out", outeachfolder, outfolder, sfile); 
    printf("CAT Command is %s \n",command);
    system(command);      
  }
    
    


  length[0] = 0;
  
  if (!build35){
    length[1] = 247249719;
    length[2] = 242951149;
    length[3] = 199501827;
    length[4] = 191273063;
    length[5] = 180857866;
    length[6] = 170899992;
    length[7] = 158821424;
    length[8] = 146274826;
    length[9] = 140273252;
    length[10] = 135374737;
    length[11] = 134452384;
    length[12] = 132349534;
    length[13] = 114142980;
    length[14] = 106368585;
    length[15] = 100338915;
    length[16] = 88827254;
    length[17] = 78774742;
    length[18] = 76117153;
    length[19] = 63811651;
    length[20] = 62435964;
    length[21] = 46944323;
    length[22] = 49691432;
    length[X] = 154913754;
    length[Y] = 57772954;
  }
  else{
    /*
    length[1] = 245522847;
    length[2] = 243018229;
    length[3] = 199505740;
    length[4] = 191411218;
    length[5] = 180857866;
    length[6] = 170975699;
    length[7] = 158628139;
    length[8] = 146274826;
    length[9] = 138429268;
    length[10] = 135413628;
    length[11] = 134452384;
    length[12] = 132449811;
    length[13] = 114142980; 
    length[14] = 106368585;
    length[15] = 100338915;
    length[16] = 88827254;
    length[17] = 78774742;
    length[18] = 76117153;
    length[19] = 63811651;
    length[20] = 62435964;
    length[21] = 46944323;
    length[22] = 49554710;
    length[X] = 154824264;
    length[Y] = 57701691;
    */
    for (i=1;i<=22;i++)
      length[i] = 280000000;
    length[X] = 200000000;
    length[Y] = 100000000;
    
  }


  repfile = (char **) malloc(sizeof(char *) * 25);
  for (i=0;i<25;i++){
    repfile[i] = (char *) malloc(sizeof(char) * 250);
    repfile[i][0]=0;
  }

  if (!build35){
    for (i=1;i<=22;i++)
      sprintf(repfile[i], "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD36/repeatout/chr%d.repeats", i);
    
    sprintf(repfile[X], "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD36/repeatout.chrX.repeats");
    sprintf(repfile[Y], "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD36/repeatout/chrY.repeats");

  }

  else{
    for (i=1;i<=22;i++)
      sprintf(repfile[i], "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/repeats/rmask/chr%d.repeats", i);
    
    sprintf(repfile[X], "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/repeats/rmask/chrX.repeats");
    sprintf(repfile[Y], "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/repeats/rmask/chrY.repeats");
  }


  for (i=1;i<=22;i++){
    sprintf(command, "stamify -i %s/chr%d.%s.out -r %s -l %d -nh", outfolder, i, sfile, repfile[i], length[i]);
    printf("\nSTAM  command is %s\n\n", command);
    system(command);
    sprintf(command, "%s/chr%d.%s.out", outfolder, i, sfile);
    printf("\nSTAM-RM  command is %s\n\n", command);
    remove(command);
  }

  sprintf(command, "stamify -i %s/chrX.%s.out -r %s -l %d -nh", outfolder, sfile, repfile[X], length[X]);
  printf("\nSTAM  command is %s\n\n", command);
  system(command);
  sprintf(command, "%s/chrX.%s.out", outfolder, sfile);
  printf("\nSTAM-RM  command is %s\n\n", command);
  remove(command);
  
  
  sprintf(command, "stamify -i %s/chrY.%s.out -r %s -l %d -nh", outfolder, sfile, repfile[Y], length[Y]);
  printf("\nSTAM  command is %s\n\n", command);
  system(command);
  sprintf(command, "%s/chrY.%s.out", outfolder, sfile);
  printf("\nSTAM-RM  command is %s\n\n", command);
  remove(command);

  sprintf(command, "rm -fr %s", outeachfolder);
  printf("\nSTAM-RMDIR  command is %s\n\n", command);
  system(command);

  gettimeofday(&end, &tz); 

  cputime = (long)(end.tv_sec)-(long)(tstart.tv_sec);

  printf("RUNTIME : %ld seconds.\n", cputime);

  return 1;
}
