#include <stdio.h>

main(int argc, char **argv){
  char ch;
  int cnt=0;
  FILE *in;
  if (argc != 2){
    printf("Counts the number of sequences in a given FASTA file.\n");
    printf("Usage : %s [FASTA file]\n", argv[0]);
    exit(0);
  }
  in = fopen(argv[1], "r");
  if (in == NULL)
    exit(0);
  while (fscanf(in, "%c", &ch) > 0)
    if (ch == '>') 
      cnt++;
  printf("Total %d sequences.\n", cnt);
}
