#include <stdio.h>

main(){
  float basequal[100];
  int n;
  int i; float f;
  n=0;
  memset(basequal, 0.0, sizeof(float)*100);

  while (scanf("%d%f", &i, &f) > 0){
    basequal[i]+=f;
    if (i==1) n++;
    //printf("read: %d\t%f\n", i, f);
  }

  for (i=1; i<100; i++){
    if (basequal[i] != 0.0){
      f = basequal[i]/((float)n);
      printf("%d\t%f\n", i, f);
    }
  }

}
