#include <stdio.h>

#define MAXCHR 250000000

int numbers[MAXCHR];

int main(int argc, char **argv){
  char chr[30];
  char chr2[30];
  int s,e,c;
  int i,j;
  int width;
  int winsize=500;
  int slide=100;
  long line;
  int maxe=0;

  char fname[100];
  char winname[100];

  FILE *in;

  
  //  for (i=0;i<MAXCHR;i++)
  //numbers[i] = 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-w"))
      strcpy(winname, argv[i+1]);
  }

  if (strcmp(fname, "stdin"))
    in = fopen(fname, "r");
  else
    in = stdin;

  memset(numbers, 0, sizeof(int)*MAXCHR);

  line=0;
  while (fscanf(in, "%s %d %d\n", chr, &s, &e)>0){
    if (s>=MAXCHR) continue;
    line++;
    numbers[s]++;
    if (e>maxe)
      maxe=e;
    //    fprintf(stderr, "%s\t%d\t%d\n", chr,s,e);
  }


  fclose(in);

  in = fopen(winname, "r");


  while (fscanf(in, "%s %d %d\n", chr2, &s, &e)>0){

    if (strcmp(chr, chr2)) continue;
    
    width=0;

    for (j=s;j<=e;j++){
      width+=numbers[j];
    }
    //printf("j:%d\n", j);                                                                                                                                                                                                                   
    //if (width!=0)
      printf("%s\t%d\t%d\t%d\n", chr, s, e, width);
  }


}
