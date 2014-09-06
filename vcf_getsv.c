#include <stdio.h>

int main(){
  
  char field[10000000];

  while (scanf("%s", field) > 0){
    
    if (strstr(field, "END=")  && !strstr(field, "CI")) printf("%s\n", field);

  }
				  
  
}
