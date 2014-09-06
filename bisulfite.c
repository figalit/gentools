#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#define MAX 200000

int main(int argc, char **argv){
  int mutprob;
  char seq[MAX];
  int i;
  int len;
  char ch;
  char dummy[1000];

  FILE *in;
  char fname[1000];
  int base;

  fname[0] = 0;
  mutprob = 0.0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-p"))
      mutprob = 100 / atoi(argv[i+1]);
  }

  if (fname[0]==0) return 0;

  in = fopen(fname, "r");

  i=0;
  while (fscanf(in, "%c", &ch) > 0){
    if (ch=='>') fgets(dummy, 1000, in);
    else if (!isspace(ch))
      seq[i++] = ch;
  }

  srand(time(NULL));

  seq[i] = 0;
  len = i;
  fclose(in);
  
  printf(">%s", dummy);
  for (i=0;i<len;i++){
    if (i % 59 == 0 && i != 0)
      printf("\n");

    if (rand() % mutprob == 0){

      base = rand() % 4;

      switch(base){
      case 0:
	printf("A");
	break;
      case 1:
	printf("C");
	break;
      case 2:
	printf("G");
	break;
      case 3:
	printf("T");
	break;
      }
      //printf("T");
    }

    else printf("%c", seq[i]);
  
  }

    /*
    if (seq[i]!='C') printf("%c", seq[i]);
    else{
      if (rand() % mutprob == 0)
	printf("T");
      else
	printf("C");
	}*/

  printf("\n");

}
