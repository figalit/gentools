/*
  Global pairwise alignment program, using affine gap model
  Usage : a.out sequenceFile1 sequenceFile2
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#define SEQ_LENGTH 150
#define MAX 25000
#include "fastatools.h"

int sig(char, char);
int max3(int, int, int);
int max(int, int);
void align(char *, char *, int **, int **);
void output(char *, char *);
char **seqs;
char **names;
void print_dp_table(int **T, int n, int m);
long long memUsage=0;
void* getMem( size_t size);

//int readSingleFasta(FILE *);


main(int argc, char **argv){
  int tSize, sSize; // textSize, sourceSize
  int Wg, Ws; // gap penalty , space penalty
  //  char T[MAX]; // text, and text_prime
  //char S[MAX]; // source, and source_prime
  char choice[2]; // either see the alignment or not
  int **V, **F, **E, **G; // tables
  int **P; /* position table for trace back
	    0 - toLeft
	    1 - toUp
	    2 - diagonal */
  int i, j;
  FILE *tFile;
  FILE *sFile;
  int nseq;

  if (argc != 2){
    printf("Global pairwise alignment program, using affine gap model\n");
    printf("Usage : %s sequenceFile\n",argv[0]);
    exit(0);
  }

  sFile = fopen(argv[1],"r");
  //fscanf(tFile,"%s",T);  seqs1->t
  //fscanf(sFile,"%s",S);  seqs0->s

  nseq = readSingleFasta(sFile);
  
  if (nseq!=2){
    fprintf(stderr, "2 sequences needed\n");
    return 0;
  }

  tSize = strlen(seqs[1]);
  sSize = strlen(seqs[0]);


  printf("Source Size = %d, Text Size = %d\n",sSize,tSize);
  /* create vectors */
  V = (int **)getMem(sizeof(int *)*(sSize+1));
  F = (int **)getMem(sizeof(int *)*(sSize+1));
  E = (int **)getMem(sizeof(int *)*(sSize+1));
  G = (int **)getMem(sizeof(int *)*(sSize+1));
  P = (int **)getMem(sizeof(int *)*(sSize+1));

  for (i=0;i<=sSize;i++){
    V[i]=(int *)getMem(sizeof(int)*(tSize+1));
    F[i]=(int *)getMem(sizeof(int)*(tSize+1));
    E[i]=(int *)getMem(sizeof(int)*(tSize+1));
    G[i]=(int *)getMem(sizeof(int)*(tSize+1));
    P[i]=(int *)getMem(sizeof(int)*(tSize+1));
  } // for
  
  /* get Wg and Ws 
  printf("Wg? ");
  scanf("%d",&Wg),
  printf("Ws? ");
  scanf("%d",&Ws);
  */

  Wg=16;
  Ws=4;

  /* initialize vectors */
  V[0][0] = 0;
  E[0][0] = -10000;
  G[0][0] = -10000;
  F[0][0] = -10000;
  P[0][0] = 0;
  for (i=1;i<=sSize;i++){
    V[i][0] = -Wg - i*Ws;
    E[i][0] = -10000; /* -infinity */
    P[i][0] = 1;
  }
  for (j=1;j<=tSize;j++){
    V[0][j] = -Wg - j*Ws;
    F[0][j] = -10000; /* -infinity */
    P[0][j] = 0;
  }
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      G[i][j] = V[i-1][j-1] + sig(seqs[0][i-1], seqs[1][j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
      if (V[i][j] == G[i][j])
	P[i][j] = 2; // match seqs[0][i]-seqs[1][j]
      else if (V[i][j] == E[i][j])
	P[i][j] = 0; // match seqs[1][j]-"-"
      else
	P[i][j] = 1; // match seqs[0][i]-"-"
    }


  printf("V:\n\n");
  print_dp_table(V, sSize, tSize);
  printf("G:\n\n");
  print_dp_table(G, sSize, tSize);
  printf("E:\n\n");
  print_dp_table(E, sSize, tSize);
  printf("F:\n\n");
  print_dp_table(F, sSize, tSize);
  printf("P:\n\n");
  print_dp_table(P, sSize, tSize);

  printf("The value of optimal alignment is: %d\n", V[sSize][tSize]);
  //printf("Want to see one alignment scheme? [y/n] ");
  //scanf("%s",choice);
  //if (choice[0] == 'y'){
  align(seqs[0], seqs[1], V, P);
    //}
} // main

int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t) && s!='-')
    return 5;
  else
    return -4;
} // G

int max3(int a, int b, int c){
  if (a>=b){
    if (a>=c)
      return a;
    else
      return c;
  }
  else{
    if (b>=c)
      return b;
    else
      return c;
  } 
} // max3

int max(int a, int b){
  if (a>b)
    return a;
  return b;
} // max

void align(char *S, char *T, int **V, int **P){
  int sSize = strlen(S);
  int tSize = strlen(T);
  int i,j,k; int c2=0;
  char *Sp, *Tp; // Sprime, Tprime
  char *Sp2, *Tp2; // Sprime, Tprime
  Tp = (char *)getMem(tSize*2);
  Sp = (char *)getMem(sSize*2);
  Tp[0]=0;
  Sp[0]=0;
  Tp2 = (char *)getMem(tSize*2);
  Sp2 = (char *)getMem(sSize*2);
  Tp2[0]=0;
  Sp2[0]=0;
  i = sSize; j = tSize;

  if (i != sSize){ // then j=tSize and i < sSize
    for (k=0; k< (sSize-i); k++){ // add spaces to end of Tprime
	sprintf(Tp2,"-%s",Tp);   
        sprintf(Sp2,"%c%s",seqs[0][sSize-k-1],Sp);
	strcpy(Sp, Sp2);
	strcpy(Tp, Tp2);
    } // for
  } // if
  if (j != tSize){ // then i=sSize and j < tSize
    for (k=0; k< (tSize-j); k++){ // add spaces to end of Sprime
	sprintf(Sp2,"-%s",Sp);   
        sprintf(Tp2,"%c%s",seqs[1][tSize-k-1],Tp);
	strcpy(Sp, Sp2);
	strcpy(Tp, Tp2);
    } // for
  } // if

  while (!(i==0 && j==0)){
    if (P[i][j] == 0){ // toLeft: match seqs[1][j]-"-"
      sprintf(Tp2,"%c%s",seqs[1][j-1],Tp);
      sprintf(Sp2,"-%s",Sp);   
      j--;
    } // if
    else if (P[i][j] == 1){ // toUp: match seqs[0][i]-"-"
      sprintf(Sp2,"%c%s",seqs[0][i-1],Sp);
      sprintf(Tp2,"-%s",Tp);
      i--;
    } // else if
    else { // diagonal: match seqs[0][i]-seqs[1][j]
      sprintf(Tp2,"%c%s",seqs[1][j-1],Tp);
      sprintf(Sp2,"%c%s",seqs[0][i-1],Sp);
      j--; i--; c2++;
    } // else
    strcpy(Sp, Sp2);
    strcpy(Tp, Tp2);
    if (i==0){
      for (k=0;k<j;k++){
	sprintf(Sp2,"-%s",Sp);
	strcpy(Sp, Sp2);
      }
      for (k=j;k>0;k--){
	sprintf(Tp,"%c%s",seqs[1][k-1],Tp);
	strcpy(Tp, Tp2);
      }
      break;
    } // if i==0
    if (j==0){
      for (k=0;k<i;k++){
	sprintf(Tp2,"-%s",Tp);
	strcpy(Tp, Tp2);
      }
      for (k=i;k>0;k--){
	sprintf(Sp2,"%c%s",seqs[0][k-1],Sp);
	strcpy(Sp, Sp2);
      }
      break;
    } // if j==0     
  } // while
  output(Sp, Tp);
} // align

void output(char *Sp, char *Tp){
  int size = strlen(Sp);
  int i=0,j=0,k;
  int cnt=0;
  while (i<size || j<size){
    printf("\n\nSp: ");
    while (cnt<=50 && i<size){
      printf("%c", toupper(Sp[i++]));
      cnt++;
    }
    cnt = 0;
    printf("\nTp: ");
    while (cnt<=50 && j<size){
      printf("%c", toupper(Tp[j++]));
      cnt++;
    }
    cnt = 0;
  } // while i<size
  
  printf("\n");
} // output


/*
int readSingleFasta(FILE *fastaFile){
  int cnt;
  char ch; 
  int i,j;
  int index;
  int seqcnt=0, seqlen=0;
  int maxlen;
  char dummy[SEQ_LENGTH];
  char str[SEQ_LENGTH];

  cnt = 0; i=0;
  fprintf(stderr, "Counting sequences.\n");
  maxlen=0;
  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      if (seqlen>maxlen)
	maxlen=seqlen;
      cnt++;
      seqlen=0;
      my_fgets(dummy, SEQ_LENGTH, fastaFile);
    }
    else if (!isspace(ch))
      seqlen++;
  }

  seqcnt = cnt;
  if (seqlen>maxlen)
    maxlen=seqlen;

  cnt = 0; i=0;

  fprintf(stderr, "Allocating memory for %d sequences with max length %d.\n", seqcnt, maxlen);
  
  
  seqs = (char **) getMem((seqcnt) * sizeof(char *));
  
  for (i=0; i<seqcnt; i++)
    seqs[i] = (char *) getMem(maxlen);
  

  names = (char **) getMem((seqcnt) * sizeof(char *));

  for (i=0; i<seqcnt; i++)
    names[i] = (char *) getMem(SEQ_LENGTH);
  
  
  
  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
  }
  

  fprintf(stderr, "Reading sequences.\n");
  rewind(fastaFile);
  cnt = -1; 
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", (cnt+1), seqcnt );
      my_fgets(names[cnt], SEQ_LENGTH, fastaFile);
      names[cnt][strlen(names[cnt])-1] = 0;
    }
    i = 0;
    if (cnt != 0){
      seqs[cnt][i++] = ch;
    }
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n'){
	seqs[cnt][i++] = ch;
      }
    } while (ch != '>');

    seqs[cnt][i] = 0;
    
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", (cnt+1), seqcnt );
      if (cnt != seqcnt){
	  my_fgets(names[cnt], SEQ_LENGTH, fastaFile);
	  names[cnt][strlen(names[cnt])-1] = 0;
      }
    } // if
  } // while
	    

  fprintf(stderr, "\n[OK] %d sequences read from fasta file.\n",seqcnt);


  return seqcnt;

}
*/


void print_dp_table(int **T, int n, int m){
  int i,j;

  for (i=0; i<=n; i++){
    for (j=0; j<=m; j++){
      fprintf (stdout, "%d\t", T[i][j]);
    }
    fprintf (stdout, "\n");
  }

    fprintf (stdout, "\n\n");

}

void* getMem( size_t size)
{
	void* ret;

	ret = malloc( size);
	if( ret == NULL)
	{
		fprintf( stderr, "Cannot allocate memory. Currently addressed memory = %0.2f MB, requested memory = %0.2f MB.\nCheck the available main memory.\n", memUsage/1048576.0, ( float) ( size / 1048576.0));
		exit( 0);
	}

	memUsage = memUsage + size;
	return ret;
}
