#include <stdio.h>

#define MAXCHR 250000000

int numbers[MAXCHR];

main(int argc, char **argv){
  char chr[30];
  int s,e,c;
  int i,j;
  int width;
  int winsize=5000;
  int slidesize=1000;
  char fname[500];
  FILE  *wssd;
  int borderStart, borderEnd;
  char chrName[30];
  
  fname[0]=0;

  if (argc!=2){
    fprintf(stderr, "Windows table file?\n");
    exit(0);
  }


  strcpy(fname, argv[1]);
  wssd = fopen(fname, "r");

  if (wssd==NULL){
    fprintf(stderr, "Windows table file not found!!\n");
    fprintf(stderr, "Table given: %s!\n", fname);
    exit(0);
  }

  for (i=0;i<MAXCHR;i++)
    numbers[i] = 0;

  while (scanf("%s%d%d%d", chr, &s, &e, &c)>0){
    //printf("%s\t%d\t%d\t%d\n", chr, s, e, c);
    numbers[s]+=c;
  }


  while (fscanf(wssd, "%s\t%d\t%d\n", chrName, &borderStart, &borderEnd) > 0){
    if (strcmp(chrName, chr))
      continue;
    //if (numbers[borderStart] != 0)
    fprintf(stdout, "%s\t%d\t%d\t%d\n", chrName, borderStart, borderEnd, numbers[borderStart]); 
  }     
  
  
  /*
  for (i=0;i<MAXCHR-winsize;i+=slidesize){
  width = 0;
    for (j=i;j<i+winsize;j++)
      width+=numbers[j];
      if (width!=0)
	printf("%s\t%d\t%d\t%d\n", chr, i+1, (i+winsize), width);
	}*/



}
