/*
  Given a set of 171bp sequences, calculates pairwise alignments
  Input : Set of 171bp sequences
  Output: Pairwise distance matrix (*.matrix)
          Histogram (*.histogram)
  todo:  fix input file format, readFasta() should be added
         for Ge, *matrix output is not needed
	 for Ge, automatically call gnuplot for histogram file
	 for Ge, change the name
	 for Ge, write a makefile including -lm
 */

#include "pimp.h"
#include "fastatools.h"

int hamming(char *, char *);

main(int argc, char **argv){
  int i,j,k;
  float r;
  int distance;
  int nseq;
  int histogram[ALPHASATLEN];
  char outfile[SEQ_LENGTH];
  int cnt=0;
  int index;
  int hor; // high order
  int intermediate; // intermediate
  FILE *in;
  FILE *histFile;
  FILE *matrixFile;
  int histStart=0;
  if (argc != 2 && argc != 3){
    printf("Given a set of alphasat sequences, calculates pairwise alignments\n");
    printf("Input : Set of alphasat sequences\n");
    printf("Output: Pairwise distance matrix (*.matrix)\nHistogram (*.histogram)\n");
    printf("\nUsage: %s SetofSequences <-align>\n",argv[0]);
    exit(0);
  }


  lemming = 0;
  if(argc == 3){
    if (!strcmp(argv[2], "-align"))
      lemming = 1;
  }

  makevectors();

  for (i=0;i<ALPHASATLEN;i++)
    histogram[i] = 0;

  in = fopen(argv[1],"r");
  sprintf(outfile,"%s.histogram",argv[1]);
  histFile = fopen(outfile,"w");
  sprintf(outfile,"%s.matrix",argv[1]);
  matrixFile = fopen(outfile,"w");


  nseq = readSingleFasta(in);

  for (i=0;i<nseq;i++){
    for (j=0;j<i;j++){
      if (lemming){
	align(seqs[i], seqs[j]);
	distance = hamming(Sp, Tp);
      }
      else
	distance = hamming(seqs[i], seqs[j]);
      fprintf(stderr,"\rSequences [%d, %d] Aligned: %d",i,j,distance);
      histogram[distance]++;
      fprintf(matrixFile,"%d\t%d\t%d\n", i, j, distance);      
    }
  }
  fprintf(stderr, "\n");
  
  // print histogram
  for (i=ALPHASATLEN-1; i>=0; i--){
    if (histogram[i] != 0)
      fprintf(histFile,"%3d %5d\n",i,histogram[i]);
  }

} // main


float hammingPercentage(char *S, char *T){
  int i;
  int similar=0;
  int length;
  int prev=0;
  float percentage;
  for (i=0;i<strlen(S);i++)
    if (S[i] == T[i] && S[i]!='-')
      similar++;
  length=similar;

  for (i=0;i<strlen(S);i++)
    if (S[i] != T[i] && S[i]!='-' && T[i]!='-')
      length++;

  for (i=0;i<strlen(S);i++){
    if (S[i] == '-'){
      if (!prev){
	prev = 1;
	length++;
      }
    }
    else
      prev=0;
  }
  prev=0;
  for (i=0;i<strlen(T);i++){
    if (T[i] == '-'){
      if (!prev){
	prev = 1;
	length++;
      }
    }
    else
      prev=0;
  }
  
  percentage = (1 - (float)similar/(float)length );

  if (percentage == 0.0 && strcmp(S,T)){
    printf("percentage: %f\n", percentage);
    printf("similar: %d length: %d\n",similar,length);
    printf("seq1: %s\n\n",S);
    printf("seq0: %s\n\n",T);
    exit(0); 
  }
  return percentage;
}


int hamming(char *S, char *T){
  int i;
  int len;
  int count;
  count = 0;
  len = strlen(S);
  for (i=0;i<len;i++){
    if (S[i] != T[i] & S[i]!='-' && T[i]!='-')
      count++;
  }
  return count;
}

