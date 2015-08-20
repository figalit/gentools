#include <stdio.h>
#include <string.h>
#define MAX 500

int main(int argc, char **argv){
  int clip=0;
  
  int i;
  char fname[100]; fname[0]=0;
  FILE *fp;
  
  char seq[MAX], qual[MAX], plus[MAX], name[MAX];

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-n"))
      clip = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
  }

  fp=fopen(fname, "r");
  if (fp==NULL){
    fprintf(stderr, "File %s cannot be opened.\n", fname);
    return 0;
  }
  if (clip <= 0){
    fprintf(stderr, "Use a non-zero positive integer for clip. You entered %d.\n", clip);
    return 0;
  }

  while(1){
    if (feof(fp)) break;
    fgets(name, MAX, fp);
    if (feof(fp)) break;
    fgets(seq, MAX, fp);
    fgets(plus, MAX, fp);
    fgets(qual, MAX, fp);
    seq[strlen(seq)-1]=0;
    qual[strlen(qual)-1]=0;

    seq[clip]=0; qual[clip]=0;

    fprintf(stdout, "%s%s\n%s%s\n", name, seq, plus,qual);
  }
  
  return 1;
}
