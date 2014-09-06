#include <stdio.h>

main(int argc, char **argv){
  int t[60];
  int a[60];
  int c[60];
  int g[60];
  int n[60];
  int i;
  char name[100];
  char seq[100];
  int nseq=0;
  char fname[100];
  FILE *fp;
  int rev=0; int fwd=0;

  memset(a, 0, sizeof(int)*60);
  memset(c, 0, sizeof(int)*60);
  memset(g, 0, sizeof(int)*60);
  memset(t, 0, sizeof(int)*60);
  memset(n, 0, sizeof(int)*60);

  FILE *in = fopen(argv[1], "r");

  while(fscanf(in, "%s\n%s\n", name, seq) > 0){
    
    if (seq[2]=='N' && seq[6]=='N'&& seq[15]=='N')
      if (name[strlen(name)-1]=='1')
	fwd++;
      else
	rev++;

  }

  printf("fwd: %d\trev: %d\n", fwd, rev);


}
