#include <stdio.h>

main(){
  char name[100];
  char fwd[100];
  char rev[100];
  char pfwd[100];
  char prev[100];
  
  pfwd[0]=0; prev[0]=0;

  while(scanf("%s\t%s\t%s\n", name, fwd, rev) > 0){
    if (strcmp(fwd, pfwd) || strcmp(rev, prev))
      printf("%s\t%s\t%s\n", name, fwd, rev);
    strcpy(pfwd, fwd); strcpy(prev, rev);
  }
}
