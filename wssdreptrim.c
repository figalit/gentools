#include <stdio.h>

#define MAX 250000000

#define GAPFILE "/net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/hg17.gaps.tab"

char seq[MAX];

int main(int argc, char **argv){

  FILE *wssd;
  FILE *depth;
  FILE *gap = fopen(GAPFILE, "r");

  char chr[100]; char chr2[100];
  char chr3[100];

  char wssdfile[100]; char depthfile[100];

  int i, ie, s, e; float d;
  int gs, ge;

  chr[0]=0;

  wssdfile[0]=0; depthfile[0]=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-d"))
      strcpy(depthfile, argv[i+1]);
    else if (!strcmp(argv[i], "-c"))
      strcpy(chr, argv[i+1]);
  }

  if (chr[0]==0 || depthfile[0]==0 || wssdfile[0]==0)
    return;

  depth = fopen(depthfile, "r");
  wssd = fopen(wssdfile, "r");

  fprintf(stderr, "init.\n");
  memset(seq, 0, sizeof(char)*MAX);

  fprintf(stderr, "read depth.\n");
  while(fscanf(depth, "%s\t%d\t%d\t%f\n", chr2, &s, &e, &d) > 0){
    if (!strcmp(chr2, chr)) for (i=s;i<=e;i++) seq[i]=1;
  }

  fclose(depth);

  fprintf(stderr, "read & dump wssd.\n");
  while (fscanf(wssd, "%s\t%d\t%d\n", chr2, &s, &e) > 0){
    if (strcmp(chr2, chr)) continue;

    i=s;
    while (seq[i]==0) i++;

    ie = e;
    while (seq[ie]==0) ie--;


    rewind(gap);

    /*
    while (fscanf(gap, "%s %d %d", chr3, &gs, &ge) > 0){
      
      if (!strcmp(chr3, chr2) && i-ge<10000 && i>gs)
	i=ge+1;
      if (!strcmp(chr3, chr2) && gs-ie<10000 && gs>i)
	ie=gs-1;

    }
    */

    s = i; 
    e  = ie;
    printf("%s\t%d\t%d\n", chr2, s, e);
  }

}
