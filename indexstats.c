#include <stdio.h>

int main(int argc, char **argv){
  FILE *in;
  FILE *hist;
  char fname[300];
  char dummy[100];
  int seq, loc;

  if (argc!=2)
    return 0;
  in = fopen(argv[1], "r");
  if (in==NULL)
    return 0;
  sprintf(fname, "%s.histogram", argv[1]);
  hist = fopen(fname, "w");
  
  fgets(dummy, 100, in);
  seq = 0; loc = 0;
  while (fscanf(in, "%s", dummy) > 0){
    if (!strcmp(dummy, "|")){
      if (seq!=0){
	fprintf(hist, "%d\t%d\n", seq, loc);
      }
      seq++; loc=0;
      while (strcmp(dummy, "!")){
	fscanf(in, "%s", dummy);
	if (dummy[0]!='!')
	  loc++;
      }
    }
  }

  fprintf(hist, "%d\t%d\n", seq, loc);
  fclose(hist);
  
  sprintf(fname, "%s.gnuplot", argv[1]);
  hist = fopen(fname, "w");
  fprintf(hist, "set terminal postscript eps color\n");
  fprintf(hist, "set output \"%s.eps\"\n", argv[1]);
  fprintf(hist, "plot \"%s.histogram\" with boxes\n", argv[1]);
  fclose(hist);
  sprintf(fname, "gnuplot %s.gnuplot", argv[1]);
  system(fname);
  sprintf(fname, "epstopdf %s.eps", argv[1]);
  system(fname);

}
