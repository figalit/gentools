#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv){
  int i;
  char fname[100];
  FILE *fp;

  char chr[50];
  int start, end;
  double depth;
  double gc;
  double newcn;
  double cn;
  
  double fx;
  double a,b;
  
  double ax,bx;

  double expect=2.0;
  double expectx=1.0;

  double avg[1000];
  double avgdepth;

  char avgname[100];

  double multiplier = 1000.0000;

  double divide=1.0;

  
  fname[0]=0; avgname[0]=0;



  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(avgname, argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      expect = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-d"))
      divide = atof(argv[i+1]);

  }


  expect = expect / divide;

  if (fname[0]==0)// || a == 0.0 || b == 0.0)
    return 0;

  fp = fopen(avgname, "r");

  for (i=0;i<1000;i++) avg[0]=0.0;

  while(fscanf(fp, "%d\t%lf\n", &i, &depth) > 0){
    //i = gc * multiplier;
    avg[i]=depth / divide;
    //    if (gc==0.502000)
    //fprintf(stdout,"------------%f\t%f\t%d\n", gc, depth, i);
  }

  fclose(fp);

  fp = fopen(fname, "r");
  
  while (fscanf(fp, "%s\t%d\t%d\t%lf\t%lf\n", chr, &start, &end, &gc, &depth) > 0){
    
    // f(x) = ax+b;

    i = gc * multiplier;

    fx = avg[i]; 

    if (fx==0){
      printf("%f boo\n", gc);
    }
      //a*gc + b;
    newcn = depth - (fx - expect);

    if (newcn < 0) newcn = 0.0;
    
    //    if (gc>0.5 && gc<0.51)
    //fprintf(stdout, "%lf\t%lf\t%lf\t%lf\t%d\n", gc, cn, fx, newcn, i);
    fprintf(stdout, "%s\t%d\t%d\t%lf\t%lf\n", chr, start, end, gc, newcn);

    //printf("%lf\t%lf\n", gc, newcn);

  }


  return 1;
}
