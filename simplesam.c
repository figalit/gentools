#include <stdio.h>

main(){
  char str[1000];
  char line[10000];
  char last;
  char *crop;

  while(fscanf(stdin, "%s", str) > 0){
    fgets(line, 10000, stdin);
      if (!strchr(str, '#'))
	  fprintf(stdout, "%s%s", str, line);
      else{
	last = str[strlen(str) - 1];
	crop = strrchr(str, '#');
	*crop = 0;
	if (last == '1' || last == '2')
	  fprintf(stdout, "%s/%c%s", str, last, line);
	else
	  fprintf(stdout, "%s%s", str, line);
      }
  }
}
