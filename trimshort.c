#include <stdio.h>

#define MAX 920000

char **trimlist;

main(int atgc, char **argv){
  FILE *match;
  FILE *trim;
  int i;
  int trimcnt;
  char name[30];
  char rest[1000];
  int found;
  int done=1;

  trimlist = (char **)malloc(sizeof(char *)*MAX);
  for (i=0;i<MAX;i++)
    trimlist[i] = (char *)malloc(sizeof(char)*25);

  match=fopen(argv[1], "r");
  trim=fopen(argv[2], "r");
  i=0;
  while(fscanf(trim, "%s", trimlist[i++]) > 0)
    ;
  trimcnt = i;

  while(fscanf(match, "%s", name) > 0){
    fgets(rest, 1000, match);
    found = 0;
    for (i=0;i<trimcnt;i++){
      if (!strcmp(trimlist[i], name)){
	found=1;
	break;
      }
    }
    fprintf(stderr, "%d\n", done); done++;
    if (!found)
      printf("%s%s", name, rest);
  }
}
