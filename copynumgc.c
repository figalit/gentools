#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  int i;
  char fname[100];
  float autoavg=0.0;
  float xavg=0.0;
  FILE *fp;

  char chr[50];
  int start, end;
  int depth;
  float gc;
  float ratio;

  fname[0]=0;


  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      autoavg = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-x"))
      xavg = atof(argv[i+1]);
  }


  if (fname[0]==0 || autoavg == 0.0 || xavg == 0.0)
    return 0;

  fp = fopen(fname, "r");
  
  while (fscanf(fp, "%s\t%d\t%d\t%f\t%d\n", chr, &start, &end, &gc, &depth) > 0){
    
    if (start > 2692000 && (!strcmp(chr, "chrX") || !strcmp(chr, "chrY")))
      ratio = (float)depth / xavg;
    else
      ratio = (float)depth / autoavg;

    
    printf("%s\t%d\t%d\t%f\t%d\t%f\n", chr, start, end, gc, depth, ratio);

  }


  return 1;
}
