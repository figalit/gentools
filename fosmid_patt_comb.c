#include <stdio.h>
#include <stdlib.h>

/* merges unit patterns for reads */

int main(int argc, char **argv){

  char read[100];
  char prevread[100];
  char unit[5];
  char line[400];
  int fr=0;

  prevread[0]=0;

  //while(scanf("%s%s", read, unit) > 0){
  while(scanf("%s", read) > 0){
    fgets(line, 200, stdin);
    line[strlen(line)-1] = 0;
    if (strcmp(read, prevread)){
      if (fr == 0 && prevread[0]!=0)
	printf("\n----------------\n");
      if (fr == 0)
	printf("\nPAIR: \n%s\tORDER:\t%s", read, line);
      else
	printf("\n%s\tORDER:\t%s", read, line);
      strcpy(prevread, read);
      fr = !fr;
    }
    else
      printf(" - %s", line);
  }
  printf("\n");
  return 1;
}
