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
    sprintf(command, "pfast-ditag -loadindex %s -search %s -istart %d -threads %d -seqname %s -textout -singlefile -minwin %d -minident 0.8", index, sfile, start, threads, seqname, minwin);
    else
    sprintf(command, "pfast-ditag -loadindex %s -search %s -istart %d -threads %d -seqname %s -textout -singlefile -minident 0.8", index, sfile, start, threads, seqname);
    system(command);
  }


  fclose(in);
  //system("cat *out | sort -k 1,1 -k 6,6nr > results.out.txt");

  //system("prunepfast results.out.txt pruned.out.txt");


  /*
  sprintf(command, "mkdir %s.pfast", sfile);
  system(command);
  sprintf(command, "mkdir %s.matched", sfile);
  system(command);
  sprintf(command, "mkdir %s.secondpass", sfile);
  system(command);
  system("for i in `ls *out`; do pfastmatch-ditag -i $i -o $i.match --high 48000 --low 32000 --maxlength 10000000; done");

  sprintf(command, "cat *.single > %s.secondpass", sfile);
  system(command);
  sprintf(command, "pfastmatch-ditag -i %s.secondpass -o --high 48000 --low 32000 --maxlength 10000000 %s.transchromosomal", sfile, sfile);
  system(command);
  //sprintf(command, "cat *.match | sort -k 1,1 -k 18,18nr > %s.chromosomal", sfile);
  sprintf(command, "cat *.match > %s.chromosomal", sfile);
  system(command);
  //  sprintf(command, "sort -k 1,1 -k 18,18nr %s.transchromosomal > temp; mv temp %s.transchromosomal", sfile, sfile);
  //system(command);
  
  //system("gzip *out");
  //system(command);
  sprintf(command, "mv *out %s.pfast", sfile);
  system(command);
  sprintf(command, "mv *match %s.matched", sfile);
  system(command);
  //sprintf(command, "rm -f *secondpass");
  //system(command);
  sprintf(command, "mv *single %s.secondpass", sfile);
  system(command);
  */
  return 1;
}
