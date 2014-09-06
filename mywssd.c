#include <stdio.h>

#define NWIN 7
#define DWIN 6

int main(int argc, char argv){

  char chr[50];

  int s, e;

  char pchr[50];
  int ps, pe;

  float xavg;
  float aavg;

  float depth;

  int i;

  char fname[100];

  FILE *fp;

  int windows;

  float tavg;

  fname[0]=0; xavg=0.0; aavg=0.0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      aavg = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-x"))
      xavg = atof(argv[i+1]);
  }

  if (fname[0]==0 || xavg==0 || aavg==0)
    return;


  fp = fopen(fname, "r");

  if (fp==NULL) return;


  pchr[0]=0; ps=-1.0; pe=-1.0;

  windows = 0;

  while (fscanf(fp, "%s%d%d%f", chr, &s, &e, &depth) > 0){

    if (!strcmp(chr, "chrX") || !strcmp(chr, "chrY"))
      tavg = xavg;
    else
      tavg = aavg;

    if (depth > aavg){
      if (!strcmp(chr, pchr) && s < pe){
	windows++;
      }
    }
     


  }

}
