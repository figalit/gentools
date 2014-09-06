/* given a fasta file makes all ATGCU capital letters */

#include <stdio.h>
#include <ctype.h>

main(int argc, char **argv){
  char fname[100];
  char newfname[100];
  int i;
  FILE *in;
  FILE *out;
  char ch;
  if (argc != 3){
    fprintf(stderr, "%s [input fasta] [output fasta]\n", argv[0]);
    exit(0);
  }

  if (!strcmp(argv[1], "stdin"))
    in = stdin;
  else
    in = fopen(argv[1], "r");
  if (in == NULL){
    fprintf(stderr, "Cannot open file %s\n", argv[1]);
    exit(0);

  }

  if (!strcmp(argv[2], "stdout"))
    out = stdout;
  else
    out = fopen(argv[2], "w");    
  if (out == NULL){
    fprintf(stderr, "Cannot write file %s\n", argv[2]);
    exit(0);

  }

  while (fscanf(in, "%c", &ch) > 0){
    if (ch == '>'){
      fgets(fname, 100, in);
      fprintf(out, ">%s", fname);
    }
    else if (ch != ' ')
      fprintf(out, "%c", toupper(ch));
  }
  
  fclose(in); fclose(out);
}
