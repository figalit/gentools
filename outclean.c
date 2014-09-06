
/*
  Removes map locations from mrfast-snp-qual out file that should not be there
  for the haploresolve project
*/

#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 200000000

char seq[MAXLEN];

int main(int argc, char **argv){
  char fname[100];
  FILE *in;
  char tabname[100];
  FILE *tab;
  int i;
  char chr[20]; int s, e;
  char name[100];
  char rest[1000];
  
  fname[0]=0;
  tabname[0]=0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-t"))
      strcpy(tabname, argv[i+1]);
  }

  if (fname[0]==0 || tabname[0]==0) return 0;

  in = fopen(fname, "r");
  tab = fopen(tabname, "r");

  if (in==NULL || tab==NULL) return 0;

  memset(seq, 0, sizeof(char)*MAXLEN);

  while (fscanf(tab, "%s%d%d\n", chr, &s, &e) > 0){
    for (i=s; i<=e; i++)
      seq[i]=1;
  }
  fclose(tab);
  
  while (fscanf(in, "%s\t%d\t%d", name, &s, &e) > 0){
    fgets(rest, 1000, in);
    if (seq[s]==1 && seq[e]==1) 
      printf("%s\t%d\t%d%s", name, s, e, rest);
  }

  fclose(in);
  
}
