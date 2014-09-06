#include <stdio.h>
#define MAX 100000000

char cover[MAX];

int main(int argc, char **argv){
  int i;
  int j;
  int zero, one;
  char fname[100];
  FILE *fp;
  char line[1000];

  char gene[100];
  int gs, ge, gl;
  
  char hit[100];
  int hs, he, hl;
  int numfrag=0;

  char prevhit[100];

  fname[0] = 0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
  }

  fp = fopen (fname, "r");

  // fgets(line, 1000, fp); // pass header

  memset(cover, 0, sizeof(char)*MAX);
  prevhit[0]=0;

  while(fscanf(fp, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t", gene, &gs, &ge, &gl, hit, &hs, &he, &hl) > 0){
    fgets(line, 1000, fp); // pass the rest
    
    if (strcmp(prevhit, gene))
      memset(cover, 0, sizeof(char)*MAX);

    zero=0; one=0;
    
    for (j=gs;j<=ge;j++)
      one+=cover[j];
    zero = ge-gs+1-one;
    
    //if (zero >= 2*one){
    if (zero!=0){
      fprintf(stdout, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s", gene, gs, ge, gl, hit, hs, he, hl, line);
      //printf("Frac hit:\t%s\treason:%d-%d\tzero:%d\tone:\t%d\n", hit, gs, ge, zero, one);
    }
  
    for (i=gs; i<=ge; i++)      
      cover[i] = 1;
  
    strcpy(prevhit, gene);
  }
}

  

