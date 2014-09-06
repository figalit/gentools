/*
  Global pairwise alignment program, using affine gap model
  Usage : a.out sequenceFile1 sequenceFile2
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#define MAX 25000
int sig(char, char);
int max3(int, int, int);
int max(int, int);
void align(char *, char *, int **, int **);
void output(char *, char *);

main(int argc, char **argv){
  int tSize, sSize; // textSize, sourceSize
  int Wg, Ws; // gap penalty , space penalty
  char T[MAX]; // text, and text_prime
  char S[MAX]; // source, and source_prime
  char choice[2]; // either see the alignment or not
  int **V, **F, **E, **G; // tables
  int **P; /* position table for trace back
	    0 - toLeft
	    1 - toUp
	    2 - diagonal */
  int i, j;
  FILE *tFile;
  FILE *sFile;
  if (argc != 3){
    printf("Global pairwise alignment program, using affine gap model\n");
    printf("Usage : %s sequenceFile1 sequenceFile2\n",argv[0]);
    exit(0);
  }
  tFile = fopen(argv[2],"r");
  sFile = fopen(argv[1],"r");
  fscanf(tFile,"%s",T);
  fscanf(sFile,"%s",S);
  tSize = strlen(T);
  sSize = strlen(S);
  printf("Source Size = %d, Text Size = %d\n",sSize,tSize);
  /* create vectors */
  V = (int **)malloc(sizeof(int)*(sSize+1));
  F = (int **)malloc(sizeof(int)*(sSize+1));
  E = (int **)malloc(sizeof(int)*(sSize+1));
  G = (int **)malloc(sizeof(int)*(sSize+1));
  P = (int **)malloc(sizeof(int)*(sSize+1));
  for (i=0;i<=sSize;i++){
    V[i]=(int *)malloc(sizeof(int)*(tSize+1));    
    F[i]=(int *)malloc(sizeof(int)*(tSize+1));
    E[i]=(int *)malloc(sizeof(int)*(tSize+1));
    G[i]=(int *)malloc(sizeof(int)*(tSize+1));
    P[i]=(int *)malloc(sizeof(int)*(tSize+1));
  } // for
  /* get Wg and Ws */
  printf("Wg? ");
  scanf("%d",&Wg),
  printf("Ws? ");
  scanf("%d",&Ws);
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
      G[i][j] = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
      if (V[i][j] == G[i][j])
	P[i][j] = 2; // match S[i]-T[j]
      else if (V[i][j] == E[i][j])
	P[i][j] = 0; // match T[j]-"-"
      else
	P[i][j] = 1; // match S[i]-"-"
    }

  printf("The value of optimal alignment is: %d\n", V[sSize][tSize]);
  printf("Want to see one alignment scheme? [y/n] ");
  scanf("%s",choice);
  if (choice[0] == 'y'){
    align(S, T, V, P);
  }
} // main

int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t) && s!='-')
    return 1;
  else
    return 0;
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
  Tp = (char *)malloc(tSize*2);
  Sp = (char *)malloc(sSize*2);
  Tp[0]=0;
  Sp[0]=0;
  Tp2 = (char *)malloc(tSize*2);
  Sp2 = (char *)malloc(sSize*2);
  Tp2[0]=0;
  Sp2[0]=0;
  i = sSize; j = tSize;

  if (i != sSize){ // then j=tSize and i < sSize
    for (k=0; k< (sSize-i); k++){ // add spaces to end of Tprime
	sprintf(Tp2,"-%s",Tp);   
        sprintf(Sp2,"%c%s",S[sSize-k-1],Sp);
	strcpy(Sp, Sp2);
	strcpy(Tp, Tp2);
    } // for
  } // if
  if (j != tSize){ // then i=sSize and j < tSize
    for (k=0; k< (tSize-j); k++){ // add spaces to end of Sprime
	sprintf(Sp2,"-%s",Sp);   
        sprintf(Tp2,"%c%s",T[tSize-k-1],Tp);
	strcpy(Sp, Sp2);
	strcpy(Tp, Tp2);
    } // for
  } // if

  while (!(i==0 && j==0)){
    if (P[i][j] == 0){ // toLeft: match T[j]-"-"
      sprintf(Tp2,"%c%s",T[j-1],Tp);
      sprintf(Sp2,"-%s",Sp);   
      j--;
    } // if
    else if (P[i][j] == 1){ // toUp: match S[i]-"-"
      sprintf(Sp2,"%c%s",S[i-1],Sp);
      sprintf(Tp2,"-%s",Tp);
      i--;
    } // else if
    else { // diagonal: match S[i]-T[j]
      sprintf(Tp2,"%c%s",T[j-1],Tp);
      sprintf(Sp2,"%c%s",S[i-1],Sp);
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
	sprintf(Tp,"%c%s",T[k-1],Tp);
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
	sprintf(Sp2,"%c%s",S[k-1],Sp);
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

