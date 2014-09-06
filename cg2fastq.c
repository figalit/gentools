#include <stdio.h>

int main(int argc, char **argv){
  int counter=1;
  
  char prefix[100];
  
  char read[100];
  char qual[100];
  int readlen;
  char dummy[100];
  char line[1000];
  int i;

  if (argc != 2) return -1;

  strcpy(prefix, argv[1]);
  
  dummy[0] = 0;

  while (strcmp(dummy, "scores")) fscanf(stdin, "%s", dummy);
  
  while (fscanf(stdin, "%s\t%s\t%s\n", dummy, read, qual) > 0){
    
    readlen = strlen(read);
    fprintf(stdout, "@%s_%d/1\n", prefix, counter);
    for (i=0;i<readlen/2;i++)
      fprintf(stdout, "%c", read[i]);

    fprintf(stdout, "\n+\n");
    for (i=0;i<readlen/2;i++)
      fprintf(stdout, "%c", qual[i]);
    
    fprintf(stdout, "\n@%s_%d/2\n", prefix, counter);
    for (i=readlen/2;i<readlen;i++)
      fprintf(stdout, "%c", read[i]);

    fprintf(stdout, "\n+\n");
    for (i=readlen/2;i<readlen;i++)
      fprintf(stdout, "%c", qual[i]);

    fprintf(stdout, "\n");

    counter++;
    
  }
  
}
