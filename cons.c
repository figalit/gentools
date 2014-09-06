#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *ret;
char *computeCons(char [][30000], int);

main(int argc, char **argv){
  FILE *stdFile;
  int i, k, cnt, j;
  int cutoff;
  int slidepos; int value;
  FILE *in;
  char ch;
  FILE *out;
  char fname[25];
  char seqs[17][30000];
  char *consensus;
  for (i=0;i<17;i++){
    //i=0;
    sprintf(fname, "file%d.aln", (i+1));
    in = fopen(fname,"r");
    k=0;
    while (fscanf(in,"%c",&ch) > 0)
      if (ch != '\n')
	seqs[i][k++] = ch;
    seqs[i][k] = 0;
    fclose(in);
    printf("Sequence %d is read in %s!!\n",i,fname);
  }
  stdFile = fopen(argv[1], "r");
  sprintf(fname,"%s.out",argv[1]);
  out = fopen(fname, "w");
  cnt = 0;
  consensus = computeCons(seqs, 16);
  printf("seq len: %d Consensus len: %d\n",strlen(seqs[0]), strlen(consensus));
  while (cnt < strlen(seqs[0])){
    printf("cnt: %d\n",cnt);
    fprintf(out, "Consen: ");
    for (i=cnt; i<cnt+80; i++)
      if (i<strlen(consensus))
	fprintf(out, "%c", consensus[i]);
    fprintf(out,"\n");
    for (j=0; j<17; j++){
      if (j<10)
	fprintf(out, "Seq %d:  ",j);
      else
	fprintf(out, "Seq %d: ",j);
      for (i=cnt; i<cnt+80; i++){
	if (i >= strlen(seqs[0]))
	  break;
	if (seqs[j][i] == consensus[i])
	  fprintf(out, ".");
	else
	  fprintf(out, "%c", seqs[j][i]);
      } // for i
      fprintf(out, "\n");
    } // for j
    fprintf(out, "\n");
    cnt = i;
    if (cnt % 240 == 0)
      fprintf(out, "\n\n\n\n\n");
    fflush(out);
  } // while
  //fclose(out);
  
} // main

char *computeCons(char seqs[][30000], int seqtot){
  int i,j;
  int a,c,g,t,cons,gap;
  free(ret);
  ret = (char *)calloc(strlen(seqs[0]), sizeof(char));
  for (i=0;i<strlen(seqs[0]);i++){
    a=0; c=0; g=0; t=0; gap=0;
    for (j=0;j<=seqtot;j++){
      switch (seqs[j][i]){
      case 'A':
	a++;
	break;
      case 'C':
	c++;
	break;
      case 'G':
	g++;
	break;
      case 'T':
	t++;
	break;
      case '-':
	gap++;
	break;
      default:
	break;
      }// switch
      cons = a;
      if (c > cons)
	cons = c;
      if (g > cons)
	cons = g;
      if (t > cons)
	cons = t;
      if (gap > cons)
	cons = gap;
      if (cons == a)
	ret[i] = 'A';
      else if (cons == c)
	ret[i] = 'C';
      else if (cons == g)
	ret[i] = 'G';
      else if (cons == t)
	ret[i] = 'T';
      else
	ret[i] = '-';
    } // for j
  } // for i
  ret[i]=0;
  return ret;
} // computecons





