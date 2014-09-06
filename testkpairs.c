#include <stdio.h>
#define MAX 250000000

char seq[MAX];

int main(int argc, char **argv){
  FILE *fasta;
  FILE *kmers;
  int i;
  char fastaname[100];
  char kmername[100];
  char ch;
  char line[1000];
  int len;
  char chr[100];
  int s1, s2, e1, e2;
  int cmp;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-f"))
      strcpy(fastaname, argv[i+1]);
    else if (!strcmp(argv[i], "-k"))
      strcpy(kmername, argv[i+1]);
  }

  fprintf(stderr, "%s\t%s\n", fastaname, kmername);

  fasta = fopen(fastaname, "r");
  kmers = fopen(kmername, "r");

  if (fasta == NULL || kmers == NULL){
    fprintf(stderr, "File open error\n"); return 0;
  }

  i=0;

  while(fscanf(fasta, "%c", &ch) > 0){
    if (ch=='>')
      fgets(line, 1000, fasta);
    else{
      if (!isspace(ch)) seq[i++]=toupper(ch);
    }
  }
  seq[i]=0;
  len=i;

  fclose(fasta);

  fprintf(stderr, "File %s read, %d characters.\n", fastaname, len);
  

  while(fscanf(kmers, "%s\t%d\t%d\t%d\t%d\n", chr, &s1, &e1, &s2, &e2) > 0){
    if (e1-s1 != e2-s2){
      printf("UNEQUAL:  %d\t%d\t%d\t%d\n", s1, e1, s2, e2);
      return 0;
    }

    cmp = memcmp(seq+s1, seq+s2, (e1-s1+1));

    if (cmp != 0){
      printf("NOMATCH:  %d\t%d\t%d\t%d\n", s1, e1, s2, e2);
      return 0;
    }
  }
  
}
