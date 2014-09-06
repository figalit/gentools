#include <stdio.h>
#include <time.h>
#include <string.h>

int main(int argc, char **argv){
  int numreads;
  int lanenum;
  int readpertile;
  int x, y;
  
  int tilecnt; int tilechunk, tilecheck;
  int i;
  
  char *runname;
  char readname[1000];
  char *rest;
  char qual[100];
  char plus [100];
  char seq[100];

  FILE *in = fopen(argv[1], "r");;

  numreads = 0;


  while(fscanf(in, "%s\n", runname) > 0 ) numreads++;

  numreads = numreads / 4;

  rewind(in);
  
  tilechunk = numreads / 1023;
  tilecnt = 1;
  tilecheck = 0;

  i=0;
  //  for (i=0;i<numreads;i++){
    
  fprintf(stderr, "%d reads\n", numreads);

  while(fscanf(in, "%s\n%s\n%s\n%s\n", readname, seq, plus, qual) > 0){


    runname = strtok(readname, ":");
    lanenum = atoi(strtok(NULL, ":"));
    x = atoi(strtok(NULL, ":"));
    rest = strtok(NULL, ":");

    //fprintf(stderr, "RUN: %s\nLANE: %d\nX: %d\nREST: %s\n", runname, lanenum, x, rest);

    //return ;


    if  (tilecheck >= tilechunk){
      tilecnt++;
      fprintf(stderr, "nseq: %d\tread: %d\ttilechunk: %d\ttilecheck: %d\ttilecnt is now %d\n", numreads, i, tilechunk, tilecheck, tilecnt);
      tilecheck=0;
    }
    tilecheck++;
    
    //x = rand() % 4096;
    //y = rand() % 4096;

    fprintf(stdout, "%s:%d:%d:%d:%s\n%s\n+\n%s\n", runname, lanenum, tilecnt, x, rest, seq, qual);

    i++;

  }
  
}
