#include <stdio.h>

int main(int argc, char **argv){
  int start;
  int end;
  int count;
  char line[1000];
  FILE *fasta;
  if (argc != 4){
    fprintf(stderr, "%s [fastafile] [start] [end]\n", argv[0]);
    return 0;
  }
  
  fasta = fopen(argv[1], "r");
  if (fasta==NULL){
    fprintf(stderr, "Unable to open file\n");
    return 0;
  }

  start = atoi(argv[2]);
  end = atoi(argv[3]);
  count = 0;
  while (fgets(line, 1000, fasta) > 0){
    if (line[0] == '>')
      count++;
    if (count >= start && count <= end)
      fprintf(stdout, "%s", line);
  }
  return 1;
}
