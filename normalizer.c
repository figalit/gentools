#include <stdio.h>
#include <ctype.h>

main(){
  char ch;
  int i=0;
  while (scanf("%c",&ch) > 0)
   if (ch=='c' || ch=='g' || ch=='t' || ch=='a' || ch=='n'){
     printf("%c",toupper(ch));
     i++;
     if (i==70){
        i=0;
        printf("\n");
     }
   }
}
