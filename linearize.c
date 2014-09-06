#include <stdio.h>

int main(){
  char ch;
  while (scanf("%c", &ch) > 0){
    if (ch != '\n' && ch != '\r')
      printf("%c", ch);
    else
      printf("\t");
  }
  printf ("\n");
}
