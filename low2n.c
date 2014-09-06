#include <stdio.h>
#include <ctype.h>

main(){
  char ch;
  char line[1000];
  while (scanf("%c", &ch) > 0){
    if (ch == '>'){
      fgets(line, 1000, stdin);
      printf(">%s",line);
    }
    else if (islower(ch))
      printf("N");
    else
      printf("%c", ch);
  }
}
