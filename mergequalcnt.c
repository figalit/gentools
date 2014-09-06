#include <stdio.h>

main(){
  long basequal[100];
  int n;
  int i; long f;
  n=0;
  memset(basequal, 0.0, sizeof(long)*100);
  
  while (scanf("%d %ld", &i, &f) > 0){
    basequal[i]+=f;
    if (i==1) n++;
  }

  for (i=1;i<100;i++){
    if (basequal[i]!=0.0){
      printf("%d\t%ld\n", i, basequal[i]);
    }
  }

}
