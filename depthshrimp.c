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
  char bname[100];
  char winname[100];

  FILE *in;
  FILE *border;

  
  //  for (i=0;i<MAXCHR;i++)
  //numbers[i] = 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-w"))
      strcpy(bname, argv[i+1]);
  }

  in = fopen(fname, "r");
  border = fopen(bname, "r");

  memset(numbers, 0, sizeof(int)*MAXCHR);

  line=0;
  while (fscanf(in, "%d\n", &e)>0){
    line++;
    //for (i=s;i<=e;i++)
    //numbers[i]++;
    numbers[e]++;
    if (e>maxe)
      maxe=e;
    //    fprintf(stderr, "%s\t%d\t%d\n", chr,s,e);
  }


  fclose(in);
  /*
  for (i=1;i<=maxe;i++){
    printf("%s\t%d\t%d\n", chr, i, numbers[i]);
  }
  */
  

  while (fscanf(border, "%s %d %d\n", chr, &s, &e) > 0){

  width = 0;

  for (i=s; i<=e; i++){
    width+=numbers[i];
  }
  
  printf("%s\t%d\t%d\t%d\n", chr, s, e, width);

  }

}
