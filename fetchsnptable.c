#include <stdio.h>
#define MAX 250000000

char seq[MAX];

main(int argc, char **argv){
  char snptable[100];
  char inttable[100];
  char outtable[100];
  FILE *stab, *itab, *otab;
  int i;

  char chr[100]; int s,e;

  snptable[0]=0; inttable[0]=0;

  int r, a,c,g,t; char ch; char snp[10];
  int gt2;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-s")) strcpy(snptable, argv[i+1]);
    else if (!strcmp(argv[i], "-i")) strcpy(inttable, argv[i+1]);
    else if (!strcmp(argv[i], "-o")) strcpy(outtable, argv[i+1]);
  }

  if (snptable[0]==0 || inttable[0]==0)
    return;
  memset(seq, 0, sizeof(char)*MAX);
  stab=fopen(snptable, "r");
  itab=fopen(inttable, "r");
  otab=fopen(outtable, "w");
  if (stab==NULL||itab==NULL) return;

  while (fscanf(itab, "%s %d %d", chr, &s, &e) > 0){
    for (i=s;i<=e;i++) seq[i]=1;
  }
  fclose(itab);

  while (fscanf(stab, "%s %d %d %s %d %d %d %d %c\n", chr, &s, &r, snp, &a, &c, &g, &t, &ch) > 0){
    gt2=0;

    if (a>=r/6) gt2++;
    if (c>=r/6) gt2++;
    if (g>=r/6) gt2++;
    if (t>=r/6) gt2++;

    if (seq[s]==1 && ch=='S' && gt2>=2)
      //fprintf(otab, "%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%c\n", chr, s, r, snp, a, c, g, t, ch);
      fprintf(otab, "%s\t%d\t%d\n", chr, s, s);
  }

  fclose (otab); fclose (stab);
} 
