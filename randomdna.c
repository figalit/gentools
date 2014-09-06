#include <stdio.h>
#include <time.h>

int main(int argc, char **argv){
  int i,j;
  int len;
  int nseq;
  int base;
  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      nseq = atoi(argv[i+1]);
  }
  srand(time(NULL));
  for (i=0;i<nseq;i++){
    printf(">seq%d\n",i);
    for (j=0;j<len;j++){
      base = rand()%4;
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
    }
    printf("\n");
  }
}
