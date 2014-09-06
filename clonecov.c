#include <stdio.h>
#define MAX 250000000
int seq[MAX];

int main(){
  int i;
  char chr[100]; int s,e;
  int max=0;
  memset (seq, 0, sizeof(int) * MAX);
  while(scanf("%s\t%d\t%d\n", chr, &s, &e) > 0){
    for (i=s; i<=e; i++) seq[i]++;
    if (e>max) max=e;
  }

  for (i=1;i<=max;i++)
    printf("%d\t%d\n", i, seq[i]);
}
