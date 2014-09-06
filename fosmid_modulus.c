/*
  given fetchfile -=- for CHIMP
  maps to a k-mer
  last update: Feb 22, 2006
  Can Alkan
*/


#include <stdio.h>
#include <stdlib.h>

#define MAXREADS 2000


int main(int argc, char **argv){

  FILE *in;
  FILE *out;
  char *name;
  char *num;
  char line[1000];
  int i,j,k;
  int nseq;
  int mmatch;
  int found;
  int modulus;

  if (argc != 3){
    fprintf(stderr, "given pairfile\nmaps to a k-mer: Feb 22, 2006\nCan Alkan\n\n");
    fprintf(stderr, "%s [pairfile] [modulus]\n", argv[0]);
    return 0;
  }

  in = fopen(argv[1],"r");
  if (in==NULL){
    fprintf(stderr, "no file\n");
    return 0;
  }
  
  modulus = atoi(argv[2]);
  name = (char *)malloc(100);
  i=0;
  while(fscanf(in, "%s", name) > 0){
    if (strstr(name, "newhor")){
      num = strtok(name, ".");
      num = strtok(NULL, ".");
      printf("newhor.%d ", (atoi(num)%modulus));
    }
    else if (strstr(name, "PAIR") || strstr(name, "---") || strstr(name, "lcl") || strstr(name,"G248"))
      printf("\n%s ", name);
    else
      printf("%s ", name);
  }

  printf("\n");

  return 1;

}
