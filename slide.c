#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define SLIDE_WIDTH 100
#define WINDOW_SIZE 1000
#define GAPS 0
#define STD 1

int slide(int, char [][30000], int, int);
char *computeCons(char [][30000], int);
char *ret;
int findDenom(char [][30000], int, char *);

main(int argc, char **argv){
  char ch=0;
  char seqs[17][30000];
  char *consensus;
  int seqcnt=0;
  int seqtot=0;
  int len;
  int i=0; int k;
  int numseq=0;
  FILE *gapsFile;
  FILE *stdFile;
  FILE *in;
  char fname[25];
  char outfname[25];
  int charcnt=0;
  int noofgaps;
  int stddif;
  char buf[10];
  char command[200];
  int denominator;
  gapsFile = fopen("gapFile","w");
  stdFile = fopen("stdfile","w");
  //strcpy(fname,argv[1]);
  for (i=0;i<17;i++)
    seqs[i][0]=0;
  
  for (i=0;i<17;i++){
    sprintf(fname, "file%d.aln", (i+1));
    in = fopen(fname,"r");
    k=0;
    while (fscanf(in,"%c",&ch) > 0)
      if (ch != '\n')
	seqs[i][k++] = ch;
    seqs[i][k] = 0;
    fclose(in);
    printf("Sequence %d is read !!\n",i);
  }
  
  seqtot = 16;
  printf("Length of alignment: %d\n",strlen(seqs[0]));
  for(i=0;i<strlen(seqs[0]);i+=SLIDE_WIDTH){
    printf("GAPS Slide Size : %d\n",i);
    noofgaps = slide(i, seqs, seqtot, GAPS);
    fprintf(gapsFile, "%d %d\n", i, noofgaps);
  }
  consensus = computeCons(seqs, seqtot);
  //printf("\n\nTotal %d sequences\n\nCons: %s\nLen:%d\n",(seqtot+1),consensus,strlen(consensus));
  if (strlen(seqs[0]) != strlen(consensus)){
    printf("Smtg wrong  \n");
    exit(0);
  }
  denominator = findDenom(seqs, seqtot, consensus);
  for(i=0;i<strlen(seqs[0]);i+=SLIDE_WIDTH){
    printf("STDS Slide Size : %d\n",i);
    stddif = slide(i, seqs, seqtot, STD);
    fprintf(stdFile, "%d %f\n", i, ((float)stddif/(float)(denominator)));
  }
  /*
    for (i=0;i<=seqtot;i++)
    if (!strcmp(seqs[i],consensus))
    printf("--> %d is the consensus\n",i); */
} // main


int slide(int startpos, char seqs[][30000], int seqtot,  int type){
  int i, j;
  int gaps=0;
  int std = 0;
  int a=0, c=0, g=0, t=0;
  char *consensus;
  if (type == GAPS){
    for (i=startpos;i<(startpos+WINDOW_SIZE);i++){
      for (j=0;j<=seqtot;j++){
	if (i >= strlen(seqs[j]))
	  return gaps;
	if (seqs[j][i] == '-')
	  gaps++;
      } // for j
    } // for i
    return gaps;
  } // if type==gaps
  else {
    consensus = computeCons(seqs, seqtot);
    for (i=startpos;i<(startpos+WINDOW_SIZE);i++){
      if (consensus[i] != '-'){
	a=0; c=0; g=0; t=0;
	for (j=0;j<=seqtot;j++){
	  if (i >= strlen(seqs[j]))
	    return std;
	  if (seqs[j][i] != consensus[i] && seqs[j][i]!='N' && seqs[j][i]!='-'){
	    switch(seqs[j][i]){
	    case 'A': 
	      a=1;
	      break;
	    case 'C':
	      c=1;
	      break;
	    case 'G':
	      g=1;
	      break;
	    case 'T':
	      t=1;
	      break;
	    default:
	      break;
	    } // switch
	  } // if  
	} // for j
	std+= a+c+g+t;
      } // if consensus
    } // for i
    return std;
  } // else
} // slide

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


int findDenom(char seqs[][30000], int seqtot, char *consensus){
  int i, j;
  int a,c,g,t, std=0;
  for (i=0;i<strlen(seqs[0]);i++){
    if (consensus[i] != '-'){
      a=0; c=0; g=0; t=0;
      for (j=0;j<=seqtot;j++){
	if (i >= strlen(seqs[j]))
	  return std;
	if (seqs[j][i] != consensus[i] && seqs[j][i]!='N' && seqs[j][i]!='-'){
	  switch(seqs[j][i]){
	  case 'A': 
	    a=1;
	    break;
	  case 'C':
	      c=1;
	      break;
	  case 'G':
	    g=1;
	    break;
	  case 'T':
	    t=1;
	    break;
	  default:
	    break;
	  } // switch
	} // if  
      } // for j
      std+= a+c+g+t;
    } // if consensus
  } // for i
} // find Denom
