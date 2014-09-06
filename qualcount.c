/* 
   given a quality file
   counts the number of bases with quality bigger than a 
   given threshold
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv){
  
  char line[1000];
  char str[50];
  int nseq;
  int qual;
  FILE *in;
  int threshold;
  long charcnt;
  long long totcharcnt;
  int dumpname = 0;
  char *name;
  long length;

  if (argc != 3 && argc!=4){
    fprintf(stderr, "Given a quality file, counts the number of bases \nwith quality bigger than a given threshold\n");
    fprintf(stderr, "Usage: %s [qualityfile] [threshold]\n", argv[0]);
    return 0;
  }
  
  if(argc == 4)
    if (!strcmp(argv[3], "-dumpname"))
      dumpname = 1;

  name = (char *) malloc(100*sizeof(char));
  threshold = atoi(argv[2]);
  in = fopen(argv[1], "r");
  if (in == NULL){
    fprintf(stderr, "Unable to open file\n");
    return 0;
  }

  charcnt = 0;
  length = 0;
  totcharcnt = 0;
  nseq=0;
  name[0] = 0;
  while (fscanf(in, "%s", str) > 0){
    if (str[0] == '>'){
      if (nseq>0 && !dumpname)
	printf("Sequence %d No of chars with qual>%d is: %ld\tlength: %ld\tratio: %f\n", nseq, threshold, charcnt, length, ((double)charcnt / (double)length));
      else if (nseq>0 && dumpname)
	printf("Sequence %s No of chars with qual>%d is: %ld\tlength: %ld\tratio: %f\n", name, threshold, charcnt, length,  ((double)charcnt /( double)length));
      fgets(line, 1000, in);
      strcpy(name,str+1);
      nseq++;
      totcharcnt += charcnt;
      charcnt=0; length = 0;
    }
    else{
      qual = atoi(str);
      if (qual >= threshold)
	charcnt=charcnt+1;
      length++;
    }
  }
  totcharcnt += charcnt;
  if (!dumpname)
    printf("Sequence %d, No of chars with qual>%d is: %ld\tlength: %ld\tratio: %f\n", nseq, threshold, charcnt, length,  ((double)charcnt / (double)length));
  else
    printf("Sequence %s, No of chars with qual>%d is: %ld\tlength: %ld\tratio: %f\n", name, threshold, charcnt, length,  ((double)charcnt / (double)length));
  printf("\nTotal chars with qual>%d is: %lld\n", threshold, totcharcnt);

  return 1;
}
