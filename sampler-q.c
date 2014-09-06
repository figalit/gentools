#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

main(int argc, char **argv){
  FILE *in;
  FILE *out;
  char ch;
  char outfname[100];
  char buf[30000];
  char head[100];
  char tmp[100];
  double samplerand;
  int seqcnt=0;
  int i;
  int bufcnt=0;
  
  char seq[100], qual[100], name[100], plus[100];


  if (argc != 3){
    printf("Extracts samples from a given FASTQ sequence input.\n%s InputFile NoOfSamples\n", argv[0]);
    exit(0);
  } // if
  if ((in = fopen(argv[1],"r")) == NULL){
    printf("Input File %s cannot be opened\n", argv[1]);
    exit(0);
  }
  sprintf(outfname,"%s_%d", argv[1], atoi(argv[2]));
  out = fopen(outfname, "w");
  while (fscanf(in, "%s\n%s\n%s\n%s\n", name, seq, plus, qual) > 0){
      seqcnt++;
  }
  srand(time(NULL));
  rewind(in);
  buf[0] = 0;
  head[0] = 0;
  while (fscanf(in, "%s\n%s\n%s\n%s\n", name, seq, plus, qual) > 0){

    //samplerand = (double)rand()/(double)RAND_MAX * (double)seqcnt/atof(argv[2]);	
    samplerand = (((double)rand() * (double)rand())/(double)((double)RAND_MAX * (double)RAND_MAX)) * (double)seqcnt/atof(argv[2]);	
    if (samplerand < 1.0){
      fprintf(out, "%s\n%s\n%s\n%s\n", name, seq, plus, qual);
    } // if samplerand<1
  } // while  
  fclose(in);
  fclose(out);
} // main
