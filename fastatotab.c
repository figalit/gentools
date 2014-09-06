#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
  char ch;
  char line[1000];

  while (scanf("%c", &ch)>0){
    if (ch == '>'){
      fgets(line, 1000, stdin);
      line[strlen(line)-1] = 0;
      fprintf(stdout, "\n>%s\t--==--\t", line);
    }
    else if (ch != '\n' && ch!='\r')
      fprintf(stdout, "%c", ch);
    
  } 

  return 1;
}
