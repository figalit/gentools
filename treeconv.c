#include <stdio.h>
#include <string.h>
main(int argc, char **argv){
  char ch;
  FILE *in;
  FILE *out;
  if (argc != 3){
    printf("Deletes the sequence names in a given .dnd tree file\n");
    printf("Usage: %s [inFile.dnd] [outFile.dnd]\n", argv[0]);
    exit(0);
  }
  if ((in = fopen(argv[1], "r")) == NULL){
    printf("Unable to open infile %s\n", argv[1]);
    exit(0);
  }

  if ((out = fopen(argv[2], "w")) == NULL){
    printf("Unable to open outfile %s\n", argv[2]);
    exit(0);
  }

  while (fscanf(in, "%c",&ch) > 0){
    if (ch=='(' || ch==')' || ch=='\n')
      fprintf(out, "%c",ch);
    else{
      while (ch!=':' && fscanf(in, "%c",&ch) >0 )
	;
      fprintf(out, ":");
      while (ch!=',' && ch!=')'){
	if (fscanf(in, "%c",&ch) <= 0)
	  break;
	fprintf(out, "%c",ch);
      }
    }
  }
}
