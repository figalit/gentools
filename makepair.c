#include <stdio.h>
main(){
  char seq1[100];
  char seq2[100];
  int n1, n2, n3;
  while (scanf("%s %s %d %d %d", seq1, seq2, &n1, &n2, &n3)>0){
    if (n3 == 0)
      continue;
    if (n3 >= n1/2 || n3>=n2/2)
      printf("%s\t%s\n", seq1, seq2);
  }
}
