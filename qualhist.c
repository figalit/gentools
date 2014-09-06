/* 
   given a quality file
   dumps the quality histogram
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv){
  
  char line[1000];
  char str[50];
  int nseq;
  int qual;
  int quals[101];
  FILE *in;
  int threshold;
  long charcnt;
  long long totcharcnt;
  int i;
  FILE *out;
  int maxq;

  if (argc != 3){
    fprintf(stderr, "Given a quality file, counts the number of bases \nwith quality bigger than a given threshold\n");
    fprintf(stderr, "Usage: %s [qualityfile] [threshold]\n", argv[0]);
    return 0;
  }
  

  for (i=0;i<101;i++){
    quals[i] = 0;
  }
  maxq = 0;

  threshold = atoi(argv[2]);
  in = fopen(argv[1], "r");
  if (in == NULL){
    fprintf(stderr, "Unable to open file\n");
    return 0;
  }

  charcnt = 0;
  totcharcnt = 0;
  nseq=0;
  while (fscanf(in, "%s", str) > 0){
    if (str[0] == '>'){
      if (nseq>0)
	printf("Sequence %d, No of chars with qual>%d is: %ld\n", nseq, threshold, charcnt);
      fgets(line, 1000, in);
      nseq++;
      totcharcnt += charcnt;
      charcnt=0;
    }
    else{
      qual = atoi(str);
      quals[qual]++;
      if (qual > maxq) maxq = qual;
      if (qual >= threshold)
	charcnt=charcnt+1;
    }
  }
  totcharcnt += charcnt;
  printf("Sequence %d, No of chars with qual>%d is: %ld\n", nseq, threshold, charcnt);
  printf("\nTotal chars with qual>%d is: %lld\n", threshold, totcharcnt);
  fclose(in);
  sprintf(line,"%s.hist", argv[1]);
  out = fopen(line, "w");

  for (i=0;i<=maxq; i++){
    fprintf(out, "%d\t%d\n", i, quals[i]);
  }
  fclose(out);
  
  sprintf(line,"%s.gnuplot", argv[1]);
  out = fopen(line, "w");
  fprintf(out, "set terminal postscript enhanced eps color\n");
  fprintf(out, "set title \"Quality Histogram For %s\"\n", argv[1]);
  fprintf(out, "set output \"%s.eps\"\n", argv[1]);
  fprintf(out, "set xlabel \"phred quality value\"\n");
  fprintf(out, "set ylabel \"number of bp\"\n");
  fprintf(out, "plot \"%s.hist\" with boxes\n", argv[1]);

  sprintf(line, "gnuplot %s.gnuplot", argv[1]);
  system(line);
  return 1;
}
