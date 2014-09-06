#include <stdio.h>
#define MAXLEN 250000000
int seq[MAXLEN];

int main(int argc, char **argv){

  char seqname[30];
  int start;
  int end;
  
  int cs=-1;
  int ce=-1;

  while (scanf("%s\t%d\t%d\n", seqname, &start, &end) > 0){
  
    if (strcmp(seqname, argv[1]))
      continue;

    if (cs == -1)
      cs = start;
    if (ce == -1)
      ce = end;

    if (start > ce){    
      printf("%s\t%d\t%d\n", seqname, cs, ce);
      cs = start;
      ce = end;
    }
    else
      ce = end;
    
  }

  //if (start > ce)
  // ce = end;
  printf("%s\t%d\t%d\n", argv[1], cs, ce);

  return 0;
}
