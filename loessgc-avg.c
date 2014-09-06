#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  int i;
  char fname[100];
  FILE *fp;

  char chr[50];
  int start, end;
  int depth;
  float gc;
  float newcn;
  float cn;
  
  float fx;
  float fdx;
  float a,b;
  float ax,bx;

  float ad,bd;
  float adx,bdx;

  float expect=2.0;
  float expectx=1.0;

  float expectd;
  float expectdx;
  float newdepth;

  float avg[1000];

  fname[0]=0;



  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      a = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-b"))
      b = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-ax"))
      ax = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-bx"))
      bx = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-ad"))
      ad = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-bd"))
      bd = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-adx"))
      adx = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-bdx"))
      bdx = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      expectd = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-ex"))
      expectdx = atof(argv[i+1]);
  }


  if (fname[0]==0 || a == 0.0 || b == 0.0)
    return 0;

  fp = fopen(fname, "r");
  
  if (a == ax && b == bx)
    expectx = expect;

  while (fscanf(fp, "%s\t%d\t%d\t%f\t%d\t%f\n", chr, &start, &end, &gc, &depth, &cn) > 0){
    
    // f(x) = ax+b;

    if (start > 2692000 && (!strcmp(chr, "chrX") || !strcmp(chr, "chrY"))){
      fx = ax*gc + bx;
      newcn = cn - (fx - expectx);
      fdx = adx*gc + bdx;
      newdepth = depth - (fdx - expectdx);
    }

    else{
      fx = a*gc + b;
      newcn = cn - (fx - expect);
      fdx = ad*gc + bd;
      newdepth = depth - (fdx - expectd);
    }
    
    if (newdepth < 0){
      newdepth = 0;
      newcn=0;
    }
    if (newcn < 0)
      newcn = 0;

    fprintf(stdout, "%s\t%d\t%d\t%f\t%f\t%f\n", chr, start, end, gc, newdepth, newcn);

  }


  return 1;
}
