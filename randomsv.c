#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX 84
int main(int argc, char **argv){
  char chr[30];
  int s, e;
  char call[50];
  int size;
  int sup;
  float score;
  float colbert;
  double samplerand;
  int seqcnt = 0;
  FILE *in;

  if (argc != 2 )
    return 0;

  in = fopen(argv[1], "r");

  srand(time(NULL));
  

  while(fscanf(in,"%s\t%d\t%d\t%s\t%d\t%d\t%f\t%f\n", chr, &s, &e, call, &size, &sup, &score, &colbert) > 0){
    seqcnt++;
  }

  rewind(in);
  while(fscanf(in,"%s\t%d\t%d\t%s\t%d\t%d\t%f\t%f\n", chr, &s, &e, call, &size, &sup, &score, &colbert) > 0 ){
    
    samplerand = (double)rand()/(double)RAND_MAX * (double)seqcnt/(float)MAX;
    if (samplerand < 1.0){     
      fprintf(stdout,"%s\t%d\t%d\t%s\t%d\t%d\t%f\t%f\n", chr, s, e, call, size, sup, score, colbert);
    }
  }
}
