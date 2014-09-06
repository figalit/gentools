#include <stdio.h>
main(){
  char ch;
  int left=0, right=0;
  while (scanf("%c",&ch) > 0)
    if (ch==')')
      right++;
    else if(ch=='(')
      left++;
  if (left!=right)
    printf("non valid\n");
  else
    printf("valid\n");
}
