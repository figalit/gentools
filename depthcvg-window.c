#include <stdio.h>

#define MAX 250000000
int seq[MAX];
main(){
  int loc; int depth; int end;
  int max;
  int i,j;
  int winsize=500000; int slide=100000;
  char chrom[100];

  memset(seq, 0, sizeof(int)*MAX);
  while (scanf("%s\t%d\t%d\n", chrom, &loc, &end)  > 0){
    seq[loc]++;
  }
  max = loc;

  depth = 0;

  for (i=1;i<=max;i+=slide){
    depth = 0;
    for (j=i;j<=max && j <i+winsize; j++){
      depth+=seq[j];
    }
    //printf("%d\t%f\n", i, ((float)depth / (float)(j-i)));
    if (depth!=0)
      printf("%s\t%d\t%d\n", chrom, i, depth);
  }

}
