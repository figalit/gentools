#include <stdio.h>
main(){
  char ch;
  ch=0;
  int nw=0;
  int isnew =1;
  int line = 1;
  int maxw=0,maxl=1;

  while(scanf("%c", &ch)>0){
    if (isalnum(ch)){
      if (isnew){
	nw++; isnew=0;
      }
    }
    else if (ch == '\r' || ch=='\n'){
      printf ("line: %d, words: %d\n", line, nw);
      if (nw > maxw){
	maxw=nw; maxl=line;
      }
      isnew = 1;
      line++;
      nw = 0;
    }
    else if (isspace(ch)){
      isnew = 1;
    }
  }
  printf("maxline: %d, maxw: %d\n", maxl, maxw);
}
