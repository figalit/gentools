#include <stdio.h>

typedef struct tab{
  char chr[30];
  int s, e;
}_tab;

struct tab draw[1000];

int main(int argc, char **argv){
  FILE *fp = fopen(argv[1], "r");
  int tcnt;
  int i;
  char chr[30];
  char chr2[30];
  int s, e;
  char dummy2[100];
  char dummy[1000];
  int s2,e2;
  int flag;

  i=0;
  while (fscanf(fp, "%s%d%d", draw[i].chr, &(draw[i].s), &(draw[i].e)) > 0)
    i++;
  tcnt = i;
  fclose(fp);

  fgets(dummy, 1000, stdin);
  fprintf(stdout, "%s", dummy);

  while (fscanf(stdin, "%s %d %d %s %s %d %d", chr, &s, &e, dummy2, chr2, &s2, &e2) > 0){
    //memset(dummy, 0, 1000);
    fgets(dummy, 1000, stdin);
    flag=0;
    for (i=0;i<tcnt;i++){
      //if ((s>=draw[i].s && e<=draw[i].e) || (s2>=draw[i].s && e2<=draw[i].e)){
      if ((s>=draw[i].s && s2<=draw[i].e)){
	flag=1; break;
      }
      if ((e>=draw[i].s && e2<=draw[i].e)){
	flag=1; break;
      }
    }
    if (flag){
      fprintf(stdout, "%s\t%d\t%d\t%s\t%s\t%d\t%d%s", chr, s, e, dummy2, chr2, s2, e2, dummy);
      fprintf(stderr, "%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s", chr, s, e, "300000000", chr2, s2, e2, dummy);
    }
  }
}


