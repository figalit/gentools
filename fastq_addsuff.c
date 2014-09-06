#include <stdio.h>
#define MAX 500

int main(){
  char s[MAX], q[MAX], p[MAX], n[MAX];
  char s2[MAX], q2[MAX], p2[MAX], n2[MAX];
  
  while (scanf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", n, s, p, q, n2, s2, p2, q2) > 0){
    printf("%s/1\n%s\n%s\n%s\n%s/2\n%s\n%s\n%s\n", n, s, p, q, n2, s2, p2, q2);
  }
}
