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

  if (argc != 3){
    printf("Generates reverse-complement of the given sequence.\n"
	   "Usage: %s [infile] [outfile]\n", argv[0]);
    exit(0);
  }
  in = fopen(argv[1], "r");
  if (in == NULL){
    printf("Unable to open input file %s\n", argv[1]);
    exit(0);
  }

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

  out = fopen(argv[2], "w");
  fprintf(out, "%s", seqname);
  k=0;
  for (i=strlen(sequence)-1; i>=0; i--){
    if (toupper(sequence[i]) == 'A')
      fprintf(out, "T");
    else if (toupper(sequence[i]) == 'T')
      fprintf(out, "A");
    else if (toupper(sequence[i]) == 'C')
      fprintf(out, "G");
    else if (toupper(sequence[i]) == 'G')
      fprintf(out, "C");
    else if (toupper(sequence[i]) == 'U')
      fprintf(out, "A");
    else if (toupper(sequence[i]) == 'N')
      fprintf(out, "N");
    k++;
    if (k % 60 == 0)
      fprintf(out, "\n");
  }
  fprintf(out, "\n");
  fclose(out);

}
