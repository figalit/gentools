#include <stdio.h>
#define MAX 200

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

  while(fscanf(fp, "%s\n%s\n%s\n%s\n", name, seq, plus,qual)>0){
    fprintf(stdout, "%s\n%s\n%s\n%s\n", name, seq+clip, plus,qual+clip);
  }
  
  return 1;
}
