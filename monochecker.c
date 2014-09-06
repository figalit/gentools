/* input: pairwise degree of divergence matrix file,
   sequence file,
   and cutoff percentage in integer
   and finds one pair and aligns them */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 200
float pairwise(char *, char *);
float hamming(char *, char *);
void output(char *, char *);
int sig(char, char);
void align(char *, char *, int **, int **);

char *aligned[2];

main(int argc, char **argv){
  FILE *matrixFile;
  FILE *sequenceFile;
  float **tmat;
  int numseq;
  float fcutoff;
  int i, j;
  int breakout=0;
  char *seqs[1000];
  float pairScore;
  float hamScore;
  if (argc != 4){
    printf("Usage : %s SequenceFile MatrixFile CutOff\n",argv[0]);
    exit(0);
  }
  sequenceFile = fopen(argv[1],"r");
  matrixFile = fopen(argv[2],"r");
  fscanf(matrixFile, "%d", &numseq);
  fcutoff = (float)atof(argv[3])/100.0;
  tmat = (float **)malloc(sizeof(float *) * numseq);
  for (i=0;i<=numseq;i++)
    tmat[i] = (float *)malloc(sizeof(float) * (i+1));
  for (i=0;i<1000;i++)
    seqs[i]=(char *)malloc(MAX);
  i=0;
  while (fscanf(sequenceFile,"%s",seqs[i++]) > 0 )
    ;
  
  for (i=0;i<2;i++)
    aligned[i]=(char *)malloc(MAX*3);
  
  for (i=0;i<numseq;i++){
    for (j=0;j<i;j++){
      fscanf(matrixFile,"%f", &(tmat[i][j]));
      //printf("%f\n",tmat[i][j]);
      if (tmat[i][j] > fcutoff){
	breakout=1;
	break;  // temporarily, will read all matrix later
      }
    }
    if (breakout)
      break;
  }
  
  printf("Sequences : %d and %d, percentageMatrix: %f\n",i,j,tmat[i][j]);
  pairScore=pairwise(seqs[j], seqs[i]);
  hamScore = hamming(aligned[0], aligned[1]);
  printf("Re-Calculated\n-------------------\n");
  printf("Pairwise Score: %f\nHamming Distance: %f\n----------------\n\n\n",pairScore,hamScore);
  output(aligned[0],aligned[1]);
  //printf("%s\n%s\n",aligned[0],aligned[1]);
} // main

float pairwise(char *S, char *T){
  int tSize, sSize; // textSize, sourceSize
  int Wg=16, Ws=4; // gap penalty , space penalty
  int **V, **F, **E, **G, **P; // tables
  //int V[MAX+1][MAX+1], G[MAX+1][MAX+1], E[MAX+1][MAX+1], F[MAX+1][MAX+1];
  int i, j;
  float ret;
  tSize = strlen(T);
  sSize = strlen(S);
  /* create vectors */
  V = (int **)malloc(sizeof(int *)*(sSize+1));
  F = (int **)malloc(sizeof(int *)*(sSize+1));
  E = (int **)malloc(sizeof(int *)*(sSize+1));
  G = (int **)malloc(sizeof(int *)*(sSize+1));
  P = (int **)malloc(sizeof(int *)*(sSize+1));
  for (i=0;i<=sSize;i++){
    V[i]=(int *)malloc(sizeof(int)*(tSize+1));    
    F[i]=(int *)malloc(sizeof(int)*(tSize+1));
    E[i]=(int *)malloc(sizeof(int)*(tSize+1));
    G[i]=(int *)malloc(sizeof(int)*(tSize+1));
    P[i]=(int *)malloc(sizeof(int)*(tSize+1));
  } // for 
  /* initialize vectors */
  V[0][0] = 0;
  P[0][0] = 0;
  E[0][0] = -10000;  
  G[0][0] = -10000;
  F[0][0] = -10000;
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

  align(S, T, V, P);

  ret = V[sSize][tSize];
  for (i=0;i<=sSize;i++){  
    free(V[i]);
    free(F[i]);
    free(E[i]);
    free(G[i]);
    free(P[i]);
  } // for
  free(V);
  free(G);
  free(E);
  free(F); 
  free(P); 
  return ret;
} // pairwise

float hamming(char *S, char *T){
  int i;
  int similar=0;
  int length;
  int prev=0;
  float percentage;
  for (i=0;i<strlen(S);i++)
    if (S[i] == T[i] && S[i]!='-')
      similar++;
  length=similar;

  for (i=0;i<strlen(S);i++)
    if (S[i] != T[i] && S[i]!='-' && T[i]!='-')
      length++;

  for (i=0;i<strlen(S);i++){
    if (S[i] == '-'){
      if (!prev){
	prev = 1;
	length++;
      }
    }
    else
      prev=0;
  }
  prev=0;
  for (i=0;i<strlen(T);i++){
    if (T[i] == '-'){
      if (!prev){
	prev = 1;
	length++;
      }
    }
    else
      prev=0;
  }
  
  percentage = (1 - (float)similar/(float)length );

  if (percentage == 0.0 && strcmp(S,T)){
    printf("percentage: %f\n", percentage);
    printf("similar: %d length: %d\n",similar,length);
    printf("seq1: %s\n\n",S);
    printf("seq0: %s\n\n",T);
    exit(0); 
  }
  return percentage;
}


int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t)){ 
    if (s!='-')
      return 1;
    else
      return 0;
  } // if
  else
    return -1;
} // sig


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
  char Sp[MAX*3], Tp[MAX*3]; // Sprime, Tprime
  char Sp2[MAX*3], Tp2[MAX*3]; // Sprime, Tprime
  //Tp = (char *)malloc(tSize*2);
  //Sp = (char *)malloc(sSize*2);
  Tp[0]=0;
  Sp[0]=0;
  //Tp2 = (char *)malloc(tSize*2);
  //Sp2 = (char *)malloc(sSize*2);
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
  strcpy(aligned[0], Sp);
  strcpy(aligned[1], Tp);

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
