#include <stdio.h>
#include <time.h>
main(){
  int i,j;
  float r;
  srand(time(NULL));
  for (i=1;i<800;i++){
    printf("\t  tax%d ",i);
    for (j=1;j<i;j++){
      r=rand()%20;
      printf("%f ",r);
    }
    if (i!=799)
      printf("\n");
    else
      printf(";\n");
  }
  
}
