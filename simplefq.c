#include <stdio.h>

main(){
  char str[1000];
  char line[1000];
  char last;
  char *crop;

  while(fscanf(stdin, "%s", str) > 0){
    if (str[0]=='@'){
      fgets(line, 1000, stdin);
      if (!strchr(str, '#'))
	  fprintf(stdout, "%s\n", str);
      else{
	last = str[strlen(str) - 1];
	crop = strrchr(str, '#');
	*crop = 0;
	if (last == '1' || last == '2')
	  fprintf(stdout, "%s/%c\n", str, last);
	else
	  fprintf(stdout, "%s\n", str);
      }
      fscanf(stdin, "%s\n", str); // seq;
      fprintf(stdout, "%s\n", str);
      fgets(line, 1000, stdin); // + line;
      fprintf(stdout, "+\n");
      fscanf(stdin, "%s\n", str); // qual;
      fprintf(stdout, "%s\n", str);      
    }
  }
}
