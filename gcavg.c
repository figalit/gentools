#include <stdio.h>
#include <math.h>

#define GRANULARITY 100

float gcdepth[GRANULARITY+1];
int gccnt[GRANULARITY+1];

int main(int argc, char **argv){

  float gc; float depth;

  int i;
  
  for (i=0;i<=GRANULARITY;i++){
    gcdepth[i]=0.0;
    gccnt[i]=0;
  }

  while (fscanf(stdin, "%f\t%f\n", &gc, &depth) > 0){
    
    i = (int) round(gc * GRANULARITY);
    gcdepth[i] += depth;
    gccnt[i]++;
    
  }


  for (i=0; i<=GRANULARITY; i++){
    if (gccnt[i] != 0)
      fprintf(stdout, "%f\t%f\n", ((float)i / (float)GRANULARITY), (gcdepth[i] / gccnt[i]));
  }
}
