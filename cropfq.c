#include <stdio.h>

int main(int argc, char **argv){
  int kmer;
  int i;
  char name[200]; char seq[200]; char plus[200]; char qual[200];
  char name2[200]; char seq2[200]; char plus2[200]; char qual2[200];

  /*
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-k"))
      kmer = atoi(argv[i+1]);
      }*/

  while (scanf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", name, seq, plus, qual, name2, seq2, plus2, qual2) > 0){
    if (strlen(seq) != strlen(qual) || strlen(seq2) != strlen(qual2) || strlen(seq) != strlen(seq2) ||  strlen(seq2) != strlen(qual2) )
      continue;

    printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", name, seq, plus, qual, name2, seq2, plus2, qual2);
    
  }
}
