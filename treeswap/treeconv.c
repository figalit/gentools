#include <stdio.h>
#include <string.h>
main(){
  char ch;
  while (scanf("%c",&ch) > 0){
    if (ch=='(' || ch==')' || ch=='\n' || ch==',')
      printf("%c",ch);
  }
}
