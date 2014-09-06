/*
  given fetchfile -=- for GIBBON
  shrinks it to a fetchfile with fosmid pairs only
  last update: Jun 19, 2006
  Can Alkan
*/


#include <stdio.h>
#include <stdlib.h>

#define MAXREADS 3500

char **names;
char **rest;
int *dumped;

int main(int argc, char **argv){

  FILE *in;
  FILE *out;
  char name[100];
  char line[1000];
  int i,j,k;
  int nseq;
  int mmatch;
  int found;

  if (argc != 3){
    fprintf(stderr, "given fetchfile\nshrinks it to a fetchfile with GIBBON fosmid pairs only\nlast update: Jun 19, 2006\nCan Alkan\n\n");
    fprintf(stderr, "%s [fetchfile] [outfile]\n", argv[0]);
    return 0;
  }

  in = fopen(argv[1],"r");
  if (in==NULL){
    fprintf(stderr, "no file\n");
    return 0;
  }
  
  out = fopen(argv[2],"w");


  names = (char **) malloc(sizeof(char *)*MAXREADS);

  for (i=0;i<MAXREADS;i++)
    names[i] = (char *) malloc(sizeof(char)*60);

  rest = (char **) malloc(sizeof(char *)*MAXREADS);

  for (i=0;i<MAXREADS;i++)
    rest[i] = (char *) malloc(sizeof(char)*300);

  dumped = (int *) malloc(sizeof(int)*MAXREADS);

  i=0;
  while(fscanf(in, "%s", name) > 0){
    fgets(line, 1000, in);
    strcpy(names[i], name);
    line[strlen(line)-1]=0;
    strcpy(rest[i], line);
    dumped[i]=0;
    i++;
  }

  nseq=i;

  for (i=0;i<nseq;i++){
    if (dumped[i])
      continue;
    found = 0;
    for (j=i+1;j<nseq;j++){
      if (i==j)
	continue;
      mmatch=0;
      if (strlen(names[i]) != strlen(names[j])){
	continue;
      }
      if (!strcmp(names[i], names[j]))
	continue;
      for (k=0; k<strlen(names[i]); k++){
	if (names[i][k]=='.' && names[j][k]=='.')
	  break;
	if (names[i][k] != names[j][k])
	  mmatch++;
      }
      
      

      //printf("%s - $s - %d\n", names[i], names[j], mmatch);
      if (mmatch>0) 
	continue;
      
      else{
	found = 1;
	break;
      }
    }
    if (found){
      if (!dumped[i]){
      	fprintf(out, "%s%s\n", names[i],rest[i]);
        dumped[i]=1;
      }
      if (!dumped[j]){
      	fprintf(out, "%s%s\n", names[j],rest[j]);
        dumped[j]=1;
      }
    }
  }
  return 1;

}
