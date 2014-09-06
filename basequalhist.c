#include <stdio.h>

int main(int argc, char **argv){
  int nbase;
  char infile[100];
  char outfile[100];
  int i;
  long *quals;
  FILE *fp;
  char name[100];
  char bull[1000];
  int q;
  int nseq;
  float aq;

  nbase = 0; infile[0] = 0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      nbase = atoi(argv[i+1]);
  }

  if (infile[0]==0 || nbase ==0)
    return 0;

  printf("nbase: %d\n", nbase);
  quals = (long *)malloc(sizeof(long)*nbase);

  for (i=0;i<nbase;i++) quals[i]=0;

  fp = fopen(infile, "r");

  if (fp == NULL)
    return 0;

  nseq=0;
  while (fscanf(fp, "%s", name) > 0){
    nseq++;
    fgets(bull, 1000, fp);
    printf("\r%d", nseq);
    for (i=0;i<nbase;i++){
      fscanf(fp, "%d", &q);
      quals[i]+=q;
    }
  }
  
  sprintf(outfile, "%s.basehist", infile);
  fclose(fp);
  fp = fopen(outfile, "w");

  for(i=0;i<nbase;i++){
    aq = (float)quals[i] / (float)nseq;
    fprintf(fp, "%d\t%f\n", (i+1), aq);
  }
  

  return 1;
}
