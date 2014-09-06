#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

main(int argc, char **argv){
  FILE *in;
  FILE *out;
  char ch;
  char *sequence;
  char seqname[100];
  int len;
  int i,k;
  int start, stop;

  if (argc != 5){
    printf("Clips the sequence between the given locations.\n"
	   "Usage: %s [infile] [loc1] [loc2] [outfile]\n", argv[0]);
    exit(0);
  }
  in = fopen(argv[1], "r");
  if (in == NULL){
    printf("Unable to open input file %s\n", argv[1]);
    exit(0);
  }

  start=atoi(argv[2]);
  stop=atoi(argv[3]);

  fscanf(in, "%c", &ch);
  if (ch != '>'){
    printf("Input sequence is not in FASTA format.\n");
    exit(0);
  }
  fgets(seqname, 100, in);
  len = 0;
  while (fscanf(in, "%c", &ch) > 0){
    if (isalpha(ch))
      len++;
  }

  rewind(in);
  
  sequence = (char *) malloc(len+5);
  fgets(seqname, 100, in);
  i = 0;
  while (fscanf(in, "%c", &ch) > 0){
    if (isalpha(ch)){
      sequence[i++] = ch;
    }
  }
  sequence[i]=0;
  seqname[strlen(seqname)-1]=0;
  out = fopen(argv[4], "w");
  fprintf(out, "%s %d-%d\n", seqname, start, stop);
  k=0;
  for (i=start-1; i<stop; i++){
    fprintf(out, "%c", sequence[i]);
    k++;
    if (k % 60 == 0)
      fprintf(out, "\n");
  }
  fprintf(out, "\n");
  fclose(out);

}
