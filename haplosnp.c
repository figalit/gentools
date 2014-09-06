
/*
  Removes map locations from mrfast-snp-qual out file that should not be there
  for the haploresolve project
*/

#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 200000000

char A[MAXLEN];
char C[MAXLEN];
char G[MAXLEN];
char T[MAXLEN];

int main(int argc, char **argv){
  char fname[100];
  FILE *in;
  char tabname[100];
  FILE *tab;
  int i;
  char chr[20]; int s, e;
  int count;
  char snp[20];
  int a,c,g,t;
  char issnp;
  char name[100];
  char rest[1000];
  int min=10;

  fname[0]=0;
  tabname[0]=0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-t"))
      strcpy(tabname, argv[i+1]);
    else if (!strcmp(argv[i], "-min"))
      min = atoi(argv[i+1]);
  }

  if (fname[0]==0 || tabname[0]==0) return 0;

  in = fopen(fname, "r");
  tab = fopen(tabname, "r");

  if (in==NULL || tab==NULL) return 0;

  memset(A, 0, sizeof(char)*MAXLEN);
  memset(C, 0, sizeof(char)*MAXLEN);
  memset(G, 0, sizeof(char)*MAXLEN);
  memset(T, 0, sizeof(char)*MAXLEN);

  while (fscanf(tab, "%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%c\n", chr, &s, &count, snp, &a, &c, &g, &t, &issnp) > 0){
    if (issnp == 'P')
      continue;
    //fprintf(stdout, "%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%c\n", chr, s, count, snp, a, c, g, t, issnp);
    //getchar ();
    if (a >= min) A[s]=1;
    if (c >= min) C[s]=1;
    if (g >= min) G[s]=1;
    if (t >= min) T[s]=1;
  }
  fclose(tab);
  
  while (fscanf(in, "%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%c\n", chr, &s, &count, snp, &a, &c, &g, &t, &issnp) > 0){
    if (issnp == 'P')
      continue;
    if (A[s]==1 || C[s]==1 || G[s]==1 || T[s]==1)
      fprintf(stdout, "%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%c\n", chr, s, count, snp, a, c, g, t, issnp);
  }

  fclose(in);
  
}
