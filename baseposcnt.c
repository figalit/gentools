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

  memset(a, 0, sizeof(int)*60);
  memset(c, 0, sizeof(int)*60);
  memset(g, 0, sizeof(int)*60);
  memset(t, 0, sizeof(int)*60);
  memset(n, 0, sizeof(int)*60);

  FILE *in = fopen(argv[1], "r");

  while(fscanf(in, "%s\n%s\n", name, seq) > 0){
    nseq++;
    for(i=0;i<strlen(seq);i++){
      switch(seq[i]){
      case 'A':
	a[i]++;
	break;
      case 'C':
	c[i]++;
	break;
      case 'G':
	g[i]++;
	break;
      case 'T':
	t[i]++;
	break;
      case 'N':
	n[i]++;
	break;
      }
    }
  }

  sprintf(fname, "%s.A", argv[1]);
  fp = fopen(fname, "w");
  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)a[i] / nseq));
  }
  fclose(fp);

  sprintf(fname, "%s.C", argv[1]);

  fp = fopen(fname, "w");

  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)c[i] / nseq));
  }
  fclose(fp);

  sprintf(fname, "%s.G", argv[1]);

  fp = fopen(fname, "w");

  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)g[i] / nseq));
  }
  fclose(fp);

  sprintf(fname, "%s.T", argv[1]);

  fp = fopen(fname, "w");

  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)t[i] / nseq));
  }
  fclose(fp);

  sprintf(fname, "%s.N", argv[1]);

  fp = fopen(fname, "w");

  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)n[i] / nseq));
  }
  fclose(fp);


}
