#include <stdio.h>

#define MAX 250000000

char seq[MAX];

int main(int argc, char **argv){
  char ch;
  int i;
  char name[100];
  char dummy[100];
  int len;
  int chrlen;
  int s, e;
  FILE *fasta; FILE *gaps;
  
  if (argc == 1) return 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i")) fasta = fopen(argv[i+1], "r");
    else if (!strcmp(argv[i], "-g")) gaps = fopen(argv[i+1], "r");
  }
  i=0;

  if (fasta==NULL || gaps ==NULL) return 0;

  fscanf(fasta,">%s", name);
  fgets(dummy, 100, fasta);


  i=0;
  while(fscanf(fasta,"%c", &ch) > 0){
    
    if (!isspace(ch)) seq[i++]=ch;

  }
  
  seq[i]=0;
  chrlen = i;


  while (fscanf(gaps, "%s\t%d\t%d\n", dummy, &s, &e) > 0){

    if (strcmp(dummy, name)) continue;

    for (i=s;i<=e;i++) seq[i]='X';
  }

  fclose(fasta); fclose(gaps);
  
  fprintf(stdout, ">%s\n", name);

  for (i=0; i<chrlen; i++){
    fprintf(stdout, "%c", seq[i]);
    if (i%60 == 1 && i!=1) fprintf(stdout, "\n");
    
  }
  

}
