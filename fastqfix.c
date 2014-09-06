#include <stdio.h>

main(){
  char name[100], plus[100], qual[500], seq[500];
  int len;

  while (fscanf(stdin, "%s\n%s\n%s\n%s\n", name, seq, plus, qual) > 0){
    len = strlen(name);
    if (name[len-1] != '1' &&   name[len-1] != '2')
      continue;
    if (name[len-2] != '/')
      continue;
    else
      fprintf(stdout, "%s\n%s\n%s\n%s\n", name, seq, plus, qual);
  }
  
}
