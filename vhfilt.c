#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 15000000
#define NAMESIZE 100
#define RESTSIZE 500
#define MAXDIS 1000

typedef struct vh{
  //char chr[6];
  char name[NAMESIZE];
  char rest[RESTSIZE];
  int counter;
}_vh;

struct vh *allvh;

static int compar(const void *p1, const void *p2);

int main(int argc, char **argv){
  int cnt = 0;
  int i,j,k;
  char name[NAMESIZE]; char rest[RESTSIZE];
  int counter;
  FILE *in;

  in = fopen(argv[1], "r");

  while (fscanf(in, "%s", name) > 0){
    fgets(rest, RESTSIZE, in);
    cnt++;
  }
	 
  allvh = (struct vh *) malloc(sizeof(struct vh) * cnt);
  
  rewind(in);
  i = 0;
  while (fscanf(in, "%s", allvh[i].name) > 0){
    fgets(allvh[i].rest, RESTSIZE, in);
    allvh[i].counter = 0;
    i++;
  }

  fprintf(stderr, "Sorting %d lines.\n", cnt);
  qsort(allvh, cnt, sizeof(struct vh),  compar);
  fprintf(stderr, "Counting.\n");

  j=0;
  name[0] = 0; counter=1;
  for (i=0;i<cnt;i++){
    if (!strcmp(name, allvh[i].name))
      counter++;
    else{
      for (k=j;k<i;k++) allvh[k].counter = counter;
      j = i;
      counter = 1;
      strcpy(name, allvh[i].name);
    }
  }
  fprintf(stderr, "Dumping.\n");
  for (i=0;i<cnt;i++){
    if (allvh[i].counter <= MAXDIS) 
      printf("%s%s", allvh[i].name, allvh[i].rest);
    /*    else
	  fprintf(stderr, "Count: %d\n", allvh[i].counter);*/
  }
}


static int compar(const void *p1, const void *p2){
  struct vh *a, *b;

  int ret;

  a = (struct vh *)p1;
  b = (struct vh *)p2;

  //if (strcmp(a->name,b->name) != 0)
  
  ret = strcmp(a->name, b->name);


  /*
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
  */

  return ret;
}

