#include <stdio.h>

int main(int argc, char **argv){
  
  double an;
  int n;
  double theta;
  int numseg;
  int i,j;

  n = atoi(argv[1]);
  numseg = atoi(argv[2]);
  
  an = 0.0;
  for (i=1; i<n; i++)
    an += 1.0/(double)i;

  theta = (double)numseg / an;
  
  //printf("theta: %f\n", theta);
  
  for (j=1; j<75000000; j+=1000){
    an = 0.0;
    for (i=1; i<j; i++)
      an += 1.0/(double)i;   
    numseg = (int) (an * theta);
    printf ("%d\t%d\n", j, numseg);
  }
}
