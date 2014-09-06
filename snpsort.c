#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 15000000

typedef struct snp{
  char chr[6];
  int loc;
  char snpchar[5];
  char type;
}_snp;

struct snp allsnp[MAXLINE];

static int compar(const void *p1, const void *p2);

int main(){
  int cnt = 0;
  int i;


  while (scanf("%s\t%c\t%d\t%s\n", allsnp[cnt].chr, &(allsnp[cnt].type), &(allsnp[cnt].loc), allsnp[cnt].snpchar) > 0)
    cnt++;
	 
  fprintf(stderr, "Sorting %d lines.\n", cnt);
  qsort(allsnp, cnt, sizeof(struct snp),  compar);
  fprintf(stderr, "Dumping.\n");

  for (i=0;i<cnt;i++)
    printf("%s\t%c\t%d\t%s\n", allsnp[i].chr, allsnp[i].type, allsnp[i].loc, allsnp[i].snpchar);

}


static int compar(const void *p1, const void *p2){
  struct snp *a, *b;

  int ret;

  a = (struct snp *)p1;
  b = (struct snp *)p2;

  if (strcmp(a->chr,b->chr) != 0)
    ret = strcmp(a->chr, b->chr);

  else if (a->type != b->type){
    if (a->type=='D' && b->type!='D')
      ret = -1;
    else  if (a->type!='S' && b->type=='S')
      ret = -1;
    else  if (a->type!='D' && b->type=='D')
      ret = 1;
    else  if (a->type=='S' && b->type!='S')
      ret = 1;
  }  
  else{
    if (a->loc == b->loc)
      ret = strcmp(a->snpchar, b->snpchar);
    else
      ret = a->loc - b->loc;
  }

  return ret;
}

