#include <stdio.h>
#define MAX 10000
char **lines;

main(){
  int i;
  int nlines;
  lines = (char **) malloc(sizeof(char *)*MAX);
  for (i=0;i<MAX;i++)
    lines[i] = (char *) malloc(sizeof(char)*60);
  for (i=0;i<MAX;i++)
    lines[i][0] = 0;
  i=0;
  while(fgets(lines[i], 60, stdin) > 0) i++;
  
  nlines = i;
  for (i=nlines-1;i>=0;i--)
    printf("%s",lines[i]);
}
