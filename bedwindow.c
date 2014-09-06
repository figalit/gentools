#include <stdio.h>

int main(){
  char chrom[100]; 
  int s, e;
  int i, j;

  while (scanf("%s\t%d\t%d\n", chrom, &s, &e) > 0){
    
    if (e-s<100) ;
    
    else if (e-s==100) printf("%s\t%d\t%d\n", chrom, s, e);

    else{
      for (i=s;i<e-100;i+=100)
	printf("%s\t%d\t%d\n", chrom, i, i+100);
    }
    
  }
}
