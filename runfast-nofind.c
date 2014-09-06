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


int main(int argc, char **argv){
  char index[150];
  int threads;
  char seqname[150];
  char sfile[150];
  int start;
  FILE *in;
  FILE *out;
  char genomeidx[150];
  int i;
  char command[1000];
  int minwin=0;  


  if (argc < 3){
    fprintf(stderr, "%s\n\n", argv[0]);
    fprintf(stderr, "\t-genome [idx]:\t genome index\n");
    fprintf(stderr, "\t-search [sfile]:\t file to be searched\n");
    fprintf(stderr, "\t-threads [t]: # of threads\n");
  }

  threads = 1;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-genome"))
      strcpy(genomeidx, argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(sfile, argv[i+1]);
    else if (!strcmp(argv[i], "-threads"))
      threads = atoi(argv[i+1]);    
    else if (!strcmp(argv[i], "-minwin"))
      minwin = atoi(argv[i+1]);    
  }


  in = fopen(genomeidx, "r");
  if (in == NULL)
    return 0;

  while(fscanf(in, "%s%s%d", index, seqname, &start) > 0){
    if (minwin != 0)
    sprintf(command, "pfast-nofind -loadindex %s -search %s -istart %d -threads %d -seqname %s -minwin %d", index, sfile, start, threads, seqname, minwin);
    else
    sprintf(command, "pfast-nofind -loadindex %s -search %s -istart %d -threads %d -seqname %s", index, sfile, start, threads, seqname);
    system(command);
  }


  fclose(in);
  /*
  system("cat *out | grep chr | sort -k 1,1 -k 6,6nr > results.out.txt");

  system("prunepfast results.out.txt pruned.out.txt");

  sprintf(command, "mkdir %s.pfast", sfile);
  system("gzip *out");
  system(command); */
  sprintf(command, "mv *out.gz results.out.txt pruned.out.txt %s.pfast", sfile);
  system(command);

  return 1;
}
