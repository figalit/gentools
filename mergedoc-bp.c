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
  int max = 0;
  
  for (i=0;i<MAXCHR;i++)
    numbers[i] = 0;

  while (scanf("%d\t%d\n", &s, &c)>0){
    //printf("%s\t%d\t%d\t%d\n", chr, s, e, c);
    numbers[s]+=c;
    if (s > max) max = s;
  }


  for (i=1;i<=max; i++)
    fprintf(stdout, "%d\t%d\n", i, numbers[i]); 



}
