#include <stdio.h>

main(){
  char name[100], plus[100], qual[500], seq[500];
  int len;
  char *ch;
  while (1){
    if (feof(stdin)) break;
    fgets(name, 100, stdin);
    if (feof(stdin)) break;
    name[strlen(name)-1]=0;

    fgets(seq, 500, stdin);
    seq[strlen(seq)-1]=0;

    fgets(plus, 100, stdin);
    

    fgets(qual, 500, stdin);
    qual[strlen(qual)-1]=0;

    len = strlen(name);
    /*    if (name[len-1] != '1' &&   name[len-1] != '2')
      continue;
    if (name[len-2] != '/')
    continue;
    else*/
    
    ch = strchr(name, ' ');
    *ch=0;
    fprintf(stdout, "%s\n%s\n%s\n%s\n", name, seq, "+", qual);
  }
  
}
