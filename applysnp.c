#include <stdio.h>
#define MAXSEQLEN 5000000

int main(int argc, char **argv){
  FILE *fasta;
  char seq[MAXSEQLEN];
  char ch; char newch[10];
  int i; int len; char dummy[1000];
  fasta = fopen(argv[1], "r");
  i=0;

  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch=='>') fgets(dummy, 1000, fasta);
    else if (!isspace(ch)) seq[i++]=ch;
  }
  seq[i]=0;
  len=i;
  
  fclose(fasta);

  fasta = fopen(argv[2], "r");
  
  while (fscanf(fasta, "%d\t%s\n", &i, newch) > 0){
    /*
    if (seq[i] != newch[0]){
      fprintf(stderr, "Coordinate %d %c:%c\n", i, seq[i], newch[0]);
      return 0;
      }*/
    //seq[i] = newch[2];
    seq[i] = newch[0];
  }
  
  printf(">%s", dummy);

  for (i=0;i<len;i++){
    printf("%c", seq[i]);
    if (i!=0 && i%59==0) printf("\n");
  }
  
}
