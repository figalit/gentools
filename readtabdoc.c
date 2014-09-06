// Reads a binary or text output file (gzipped or not)

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
#include <math.h>
#include <assert.h>
#include <zlib.h>

#define SEQ_LENGTH 200

int *numbers;

int main(int argc, char **argv){
  FILE *tab;
  int i,j;
  int linecnt;
  char infile[SEQ_LENGTH];
  char lineString[10000];
  int chrStart, chrEnd;
  char chr[30];
  int maxnum=0;
  int winsize=0;
  int width;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-m"))
      maxnum = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-w"))
      winsize = atoi(argv[i+1]);
  }
  
  tab = gzopen(infile, "r");
  maxnum = 250000000;
  numbers = (int *) malloc(sizeof(int)*maxnum);
  for (i=0;i<maxnum;i++)
    numbers[i]=0;

  //fprintf(stderr, "Reading.\n");
  //gzread(tab, &linecnt, sizeof(linecnt));
  //fread(&linecnt, sizeof(linecnt), 1, tab);
  //fprintf(stderr, "%d lines.\n", linecnt);
  while (!gzeof(tab)) {
    gzgets(tab, lineString, 10000);
    if (gzeof(tab))
      break;
    linecnt++;
    sscanf(lineString, "%s\t%d\n", chr, &chrStart);
    numbers[chrStart]++;
    //printf("%s\t%d\n", chr, chrStart);//, chrEnd);
  }
  //fprintf(stderr, "Printing DOC.\n");

  for (i=0;i<maxnum;i+=winsize){
    width=0;
    for (j=i;j<i+winsize && j<maxnum;j++){
      width+=numbers[j];
    }
    //printf("j:%d\n", j);
    if (width!=0)
      printf("%s\t%d\t%d\t%d\n", chr, i, (i+winsize), width);
  }

}
