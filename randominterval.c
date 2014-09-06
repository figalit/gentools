#include <stdio.h>
#include <time.h>

#define MAX 250000000

#define INTLEN 100000

#define NOINT 20

char seq[MAX];


main(){
  int intcnt=0;
  char name[100];
  char fname[100];
  char ch;
  int i,j; int len;
  FILE *out;
  int s;
  int isX;
  i=0;
  fgets(name, 100, stdin);
  name[strlen(name)-1] = 0;
  while (scanf("%c", &ch) > 0){
    if (!isspace(ch)) seq[i++]=ch;
  }
  seq[i]=0; len=i;
  srand(time(NULL));
  while (intcnt != NOINT){
    s = rand() % (len - INTLEN);
    isX=0;
    for(i=s;i<s+INTLEN;i++){
      if (seq[i]=='X'){
	isX=1;
	break;
      }
    }
    if (!isX){
      intcnt++;
      sprintf(fname, "interval.%d", intcnt);
      out = fopen(fname,"w");
      fprintf(out, "%s.%d", name, s);
      j=0;
      for (i=s;i<s+INTLEN;i++){
	j++;
	if (j%60==1)
	  fprintf(out, "\n");
	fprintf(out, "%c", seq[i]);
      }
      fclose(out);
    }
  }
}
