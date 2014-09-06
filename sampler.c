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
  if (argc != 3){
    printf("Extracts samples from a given FASTA sequence input.\n%s InputFile NoOfSamples\n", argv[0]);
    exit(0);
  } // if
  if ((in = fopen(argv[1],"r")) == NULL){
    printf("Input File %s cannot be opened\n", argv[1]);
    exit(0);
  }
  sprintf(outfname,"%s_%d", argv[1], atoi(argv[2]));
  out = fopen(outfname, "w");
  while (fscanf(in, "%c", &ch) > 0){
    if (ch == '>')
      seqcnt++;
  }
  srand(time(NULL));
  rewind(in);
  buf[0] = 0;
  head[0] = 0;
  while (fscanf(in, "%c", &ch) > 0){
    if (ch == '>'){
      if (head[0] != 0){
	samplerand = (double)rand()/(double)RAND_MAX * (double)seqcnt/atof(argv[2]);	
	if (samplerand < 1.0){
	  fprintf(out,"%s", head);
	  buf[bufcnt] = 0;
	  for (i = 0 ; i<strlen(buf); i++){
	    if (i!=0 && i%80==0)
	      fprintf(out, "\n");
	    fprintf(out, "%c", buf[i]);
	  } // for
	  fprintf(out, "\n");
	} // if samplerand<1
      } // if head[0]!=0
      fgets(tmp, 100, in);
      sprintf(head, ">%s", tmp);
      bufcnt = 0;
      buf[0] = 0;
    } // if ch=='>'
    else if (ch!='\n')
      buf[bufcnt++] = ch;
  } // while  
  fclose(in);
  fclose(out);
} // main
