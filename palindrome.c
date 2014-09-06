#include "fastatools.h"

int isPalindrome(char *word, int len){
  int i,j;
  i=0;
  j=len-1;
  while (i<j){
    if (word[i++] != word[j--])
      return 0;
  }
  return 1;
}

int main(int argc, char **argv){
  FILE *in=fopen(argv[1],"r");
  int palsize=atoi(argv[2]);

  char *word;

  int nseq;

  int i;
  int y;
  int len;

  nseq = readSingleFasta(in);

  word  = (char *)malloc(sizeof(char) * (palsize+1));
  
  for (i=0;i<nseq;i++){
    if (!strcmp(names[i], "chrY"))
      y = i;
  }
  

  len = strlen(seqs[y]);

  fprintf(stderr, "chrY:%d\tlen:%d\n", y, len);


  for (i=0;i<len-palsize;i++){
    memcpy(word, seqs[y]+i, palsize);
    word[palsize] = 0;
    if (strchr(word, 'N') != NULL)
      continue;
    
    if (isPalindrome(word, palsize)){
      printf("Palindrome at %d:\t%s\n", i, word);
    }
  }
}
