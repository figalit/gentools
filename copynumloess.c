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
  float a,b;
  
  float ax,bx;

  float expect=2.0;
  float expectx=1.0;

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
    /*
    else if (!strcmp(argv[i], "-e"))
      expect = atof(argv[i+1]);
    */
  }


  if (fname[0]==0 || a == 0.0 || b == 0.0)
    return 0;

  fp = fopen(fname, "r");
  
  while (fscanf(fp, "%f\t%f\n", &gc, &cn) > 0){
    
    // f(x) = ax+b;

    fx = a*gc + b;
    newcn = cn - (fx - expect);
    
    printf("%f\t%f\n", gc, newcn);

  }


  return 1;
}
