#include <stdio.h>

int main(){
  int i;
  char name[200], seq[200], dummy[200], qual[200];

  while (scanf ("%s\n%s\n%s\n%s\n", name, seq, dummy, qual) > 0 ){
    printf("%s\n%s\n+\n", name, seq);
    for (i=0;i<strlen(qual);i++){
      printf("%c", qual[i]-31);
    }
    printf("\n");
  }
}
