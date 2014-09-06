#include <stdio.h>

#define MAX 250000000


char seq[MAX];

int main(){
  
  char fname[100];
  int start;
  int i;
  
  char dummy[1000];
  char ch;

  FILE *fp;
  int max=0;

  char chrname[100];

  for (i=0;i<MAX;i++)
    seq[i]='N';

  i=0;

  while (scanf("%s\t%s\t%d\n", fname, chrname, &start) > 0){
    
    fprintf(stderr, "reading %s\t last i: %d\n", fname, i);
    FILE *fp = fopen(fname, "r");
    
    fgets(dummy, 1000, fp);

    ch=0;

    i=start;

    while (fscanf(fp, "%c", &ch) > 0){
      
      if (!isspace(ch))
	seq[i++] = ch;
      
      if (i>max) max = i;
	  
    }
    
    fclose(fp);

  }

  printf(">%s\n", chrname);

  for (i=1;i<=max;i++){
    if (i%60==0) printf("\n");

    printf("%c", seq[i]);
    
  }

}
