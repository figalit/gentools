/* merge kmer pair list from wgacish */
#include <stdio.h>

//#define MAX 1320000
#define MAX 5000000

typedef struct pair{
  char chr[25];
  int ls, le;
  int rs, re;
}_pair;

struct pair kmers[MAX];

int main(int argc, char **argv){
  int nkmer;
  int i,j;
  char chr[25]; int ls, le, rs, re;
  int didconverge;

  fprintf(stderr, "init pairs\n");
  for (i=0;i<MAX;i++){
    kmers[i].chr[0]=0;
    kmers[i].ls = 0;
    kmers[i].le = 0;
    kmers[i].rs = 0;
    kmers[i].re = 0;
  }

  i=0;

  fprintf(stderr, "read pairs\n");
  while(scanf("%s\t%d\t%d\t%d\t%d\n", chr, &ls, &le, &rs, &re) > 0){

    strcpy(kmers[i].chr, chr);
    kmers[i].ls=ls;
    kmers[i].le=le;
    kmers[i].rs=rs;
    kmers[i].re=re;
    i++;
    
  }

  nkmer = i;

  fprintf(stderr, "%d pairs loaded\n", nkmer);

  didconverge = 1;

  int loop = 0;

  do{
    loop++;
    didconverge = 1;
    fprintf(stderr, "\nloop %d\n", loop);
    for (i=0;i<nkmer;i++){
      //      fprintf(stderr, "\r%d", i);
      for (j=i+1; j<nkmer; j++){
	if (kmers[j].ls == 0) continue;
	if (strcmp(kmers[i].chr, kmers[j].chr)) continue;
	
	if (kmers[i].le == kmers[j].le-1 && kmers[i].re == kmers[j].re-1){
	  
	  kmers[i].le = kmers[j].le;
	  kmers[i].re = kmers[j].re;
	  
	  kmers[j].ls = 0;
	  didconverge = 0;
	}
	if (abs(kmers[i].le - kmers[j].le) > 10)
	  break;
      }
    }
  } while (!didconverge);
  
  fprintf(stderr, "\n\nDump output\n\n");
  for (i=0;i<nkmer;i++){
    if (kmers[i].ls == 0) continue;
    printf("%s\t%d\t%d\t%d\t%d\n", kmers[i].chr, kmers[i].ls, kmers[i].le, kmers[i].rs, kmers[i].re);
  }
}
