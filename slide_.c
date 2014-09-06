#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define SLIDE_WIDTH 5
#define WINDOW_SIZE 10
#define GAPS 0
#define STD 1

int slide(int, char [][1000], int, char [], int);
char *computeCons(char [][1000], int);
int findDenom(char [][1000], int, char *);

main(int argc, char **argv){
  char ch=0;
  char seqs[1000][1000];
  char *consensus;
  int seqcnt=0;
  int seqtot=0;
  int len;
  int i=0; int k;
  int numseq=0;
  FILE *gapsFile;
  FILE *stdFile;
  FILE *in;
  FILE *listFile;
  char fname[25];
  char outfname[25];
  int charcnt=0;
  int noofgaps;
  int stddif;
  char buf[10];
  char command[200];
  FILE *gnuplot;
  int denominator;
  if (argc != 2){
    printf("Usage : %s AlignmentFile\n",argv[0]);
    exit(0);
  } // if
  listFile = fopen(argv[1],"r");
  gnuplot = fopen("plotfile","w");
  fprintf(gnuplot,"set terminal postscript\n");
  //strcpy(fname,argv[1]);
  while (fscanf(listFile,"%s",fname) > 0){
    seqtot=0;
    len=0;
    printf("%s \n",fname);
    for (i=0;i<1000;i++)
      seqs[i][0]=0;
    in = fopen(fname,"r");
    while (!isdigit(ch))
      fscanf(in,"%c",&ch);
    while (fscanf(in,"%c",&ch) > 0){      
      k=0;
      while (!isdigit(ch) && fscanf(in,"%c",&ch)>0 )
	;
      if(isdigit(ch)){
	while(ch!='-')
	  fscanf(in,"%c",&ch);;
      fscanf(in,"%c",&ch);
      while(ch!='-')
	fscanf(in,"%c",&ch);
      fscanf(in,"%c",&ch);
      while(ch!='-'){
	buf[k++]=ch;
	fscanf(in,"%c",&ch);
      }
      buf[k]=0;
      }
      while(ch!=' ' && fscanf(in,"%c",&ch) > 0)
	;
      while(ch==' ' && fscanf(in,"%c",&ch) > 0)
      ;
      seqcnt=atoi(buf);//%1000;
      if (seqcnt > seqtot)
	seqtot = seqcnt;
      i=strlen(seqs[seqcnt]);
      if (ch!='\n' && ch!='\r')
	seqs[seqcnt][i++]=ch;
      
      while((ch!='\n' && ch!='\r' && ch!=' ') && fscanf(in,"%c",&ch) > 0)
	if (ch!='\n' && ch!='\r' && ch!=' '){
	  seqs[seqcnt][i++]=ch; 
	} // if
	else{
	  seqs[seqcnt][i++]=0;    
	  break;
	} // else
    } // while fscanf
    printf("Length of alignment: %d\n",strlen(seqs[0]));
    sprintf(outfname,"%s.gaps",fname);
    gapsFile = fopen(outfname,"w");
    for(i=0;i<strlen(seqs[0]);i+=SLIDE_WIDTH){
      noofgaps = slide(i, seqs, seqtot, fname, GAPS);
      fprintf(gapsFile, "%d %d\n", i, noofgaps);
    }
    fclose(gapsFile); 
    consensus = computeCons(seqs, seqtot);
    printf("\n\nTotal %d sequences\n\nCons: %s\n",(seqtot+1),consensus);
    
    denominator = findDenom(seqs, seqtot, consensus);
    printf("denom: %d\n",denominator);
    sprintf(outfname,"%s.stds",fname);
    
    stdFile = fopen(outfname,"w");
    for(i=0;i<strlen(seqs[0]);i+=SLIDE_WIDTH){
      stddif = slide(i, seqs, seqtot, fname, STD);
      fprintf(stdFile, "%d %f\n", i, ((float)stddif/(float)(denominator)));
    }
    fclose(stdFile);
    
    fprintf(gnuplot,"set output \"%s.gaps.ps\"\n",fname);
    fprintf(gnuplot,"plot \"%s.gaps\" with boxes\n",fname);
    
  } // while fscanf 
  fclose(gnuplot);
} // main


int slide(int startpos, char seqs[][1000], int seqtot, char fname[], int type){
  int i, j;
  int gaps=0;
  int std = 0;
  char *consensus;
  int a=0, c=0, g=0, t=0;
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

char *computeCons(char seqs[][1000], int seqtot){
  char *ret;
  int i,j;
  int a,c,g,t,cons,gap;
  ret = (char *)malloc(strlen(seqs[0]));
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
  return ret;
} // computecons

int findDenom(char seqs[][1000], int seqtot, char *consensus){
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
