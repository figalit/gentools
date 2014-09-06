#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#define MAX 350000
float sig(char, char);
float max3(float, float, float);
float max(float, float);
void align(char *, char *, float **, float **);
void output(char *, char *);
//char names[100][2];
char **names;

float **V, **F, **E, **G; // tables

int main(int argc, char **argv){
  int tSize, sSize; // textSize, sourceSize
  float Wg, Ws; // gap penalty , space penalty
  char T[MAX]; // text, and text_prime
  char S[MAX]; // source, and source_prime
  char choice[2]; // either see the alignment or not

  float **P; /* position table for trace back
	    0 - toLeft
	    1 - toUp
	    2 - diagonal */
  int i, j;
  char ch;
  FILE *tFile;
  FILE *sFile;
  char dummy[100];

  if (argc != 3){
    printf("Usage: %s [file1] [file2]\n", argv[0]);
    exit(0);
  }
  tFile = fopen(argv[2],"r");
  sFile = fopen(argv[1],"r");
  if (sFile == NULL){
    printf("Can't read %s\n", argv[1]);
    exit(0);
  }
  if (tFile == NULL){
    printf("Can't read %s\n", argv[2]);
    exit(0);
  }
  //fscanf(sFile,"%c%s", &ch, names[0]);
  //fscanf(tFile,"%c%s", &ch, names[1]);

  names = (char **) malloc(sizeof(char *) *2);
  names[0] = (char *) malloc(sizeof(char) * 100);
  names[1] = (char *) malloc(sizeof(char) * 100);

  fscanf(sFile, ">%s", names[0]);
  fscanf(tFile, ">%s", names[1]);
  fgets(dummy, 100, sFile);
  fgets(dummy, 100, tFile);
  //names[0][strlen(names[0])-1] = 0;
  //names[1][strlen(names[1])-1] = 0;
  //(names[0])++; (names[1])++;

  i = 0;
  printf("Sequences: %s\t%s\n", names[0], names[1]);
  while (fscanf(sFile, "%c", &ch) > 0){
    if (isalpha(ch))
      S[i++] = ch;
    S[i] = 0;
  }
  i = 0;
  while (fscanf(tFile, "%c", &ch) > 0){
    if (isalpha(ch))
      T[i++] = ch;
    T[i] = 0;
  }
  tSize = strlen(T);
  sSize = strlen(S);
  printf("Source Size = %d, Text Size = %d\n",sSize,tSize);
  printf("Seq1: %s Seq2: %s\n",names[0],names[1]);
  /* create vectors */
  V = (float **)malloc(sizeof(float)*(sSize+1));
  F = (float **)malloc(sizeof(float)*(sSize+1));
  E = (float **)malloc(sizeof(float)*(sSize+1));
  G = (float **)malloc(sizeof(float)*(sSize+1));
  P = (float **)malloc(sizeof(float)*(sSize+1));
  for (i=0;i<=sSize;i++){
    V[i]=(float *)malloc(sizeof(float)*(tSize+1));    
    F[i]=(float *)malloc(sizeof(float)*(tSize+1));
    E[i]=(float *)malloc(sizeof(float)*(tSize+1));
    G[i]=(float *)malloc(sizeof(float)*(tSize+1));
    P[i]=(float *)malloc(sizeof(float)*(tSize+1));
  } // for
  /* get Wg and Ws */
  printf("Wg? ");
  scanf("%f",&Wg),
  printf("Ws? ");
  scanf("%f",&Ws);
  /* initialize vectors */
  V[0][0] = 0;
  P[0][0] = 0;
  for (i=1;i<=sSize;i++){
    V[i][0] = 0 - Wg - i*Ws;
    //V[i][0] = 0;
    E[i][0] = -1000000; /* -infinity */
    P[i][0] = 1;
  }
  for (j=1;j<=tSize;j++){
    V[0][j] = 0 - Wg - j*Ws;
    //V[0][j] = 0;
    F[0][j] = -1000000; /* -infinity */
    P[0][j] = 0;
  }
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      G[i][j] = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
      if (V[i][j] == F[i][j])
	P[i][j] = 1; // match S[i]-"-"
      else if (V[i][j] == E[i][j])
	P[i][j] = 0; // match T[i]-"-"
      else
	P[i][j] = 2; // match S[i]-T[j]
    }
  printf("The value of optimal alignment is: %f\n",V[sSize][tSize]);
  printf("Want to see one alignment scheme? [y/n] ");
  scanf("%s",choice);
  if (choice[0] == 'y'){
    align(S, T, V, P);
  }
  return 0;
} // main

float sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t) && s!='-')
    return 5;
  else
    return -1;
} // G

float max3(float a, float b, float c){
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

float max(float a, float b){
  if (a>b)
    return a;
  return b;
} // max

void align(char *S, char *T, float **V, float **P){
  int sSize = strlen(S);
  int tSize = strlen(T);
  int i,j,k;
  int ii, jj;
  int larger=sSize;
  char *Sp, *Tp; // Sprime, Tprime
  if (tSize > larger)
    larger = tSize;
  Tp = (char *)malloc(larger*2);
  Sp = (char *)malloc(larger*2);
  Tp[0]=0;
  Sp[0]=0;
  ii=0; jj=0;
  i = sSize; j = tSize;
  while (!(i==0 && j==0)){
    if (P[i][j] == 2){ // diagonal: match S[i]-T[j] 
      //while (i>0 && j>0 && V[i][j] == G[i][j]){ 
      Tp[jj++] = T[--j];
      Sp[ii++] = S[--i];
	//}
    } // if
    else if (P[i][j] == 1){ // toUp: match S[i]-"-"
      //while (i>0 && j>0 && V[i][j] == F[i][j]){
	Tp[jj++] = '-';
	Sp[ii++] = S[--i];      
	//     }
    } // else if   
    else { // toLeft: match T[j]-"-"
      //while (i>0 && j>0 && V[i][j] == E[i][j]){
	Tp[jj++] = T[--j];
	Sp[ii++] = '-';
	//}
    } // else
    if (i==0){
      for (k=0;k<j;k++){
	Sp[ii++] = '-';
      }
      for (k=j;k>0;k--){
	Tp[jj++] = T[k-1];
      }
      break;
    } // if i==0
    if (j==0){
      for (k=0;k<i;k++){
	Tp[jj++] = '-';
      }
      for (k=i;k>0;k--){
	Sp[ii++] = S[k-1];
      }
      break;
    } // if j==0
    //ii++; jj++;
  } // while
  Sp[ii+1] = 0;
  Tp[jj+1] = 0;
  output(Sp, Tp);
} // align

void output(char *Sp, char *Tp){  // reverse print
  int size = strlen(Sp);
  int i=size-1,j,k, m;
  int cnt=0;
  int namesize;
  k = i;
  j = i;
  if (strlen(names[0]) > strlen(names[1]))
    namesize = strlen(names[0]);
  else
    namesize = strlen(names[1]);
  
  while (i>=0 || j>=0){
    printf("\n\n\n%s ", names[0]);
    if (strlen(names[0]) < namesize){
      for (m=0; m<namesize-strlen(names[0]); m++)
	printf(" ");
    }
    while (cnt<=60 && i>=0){
      printf("%c", toupper(Sp[i--]));
      cnt++;
    }
    cnt = 0;
    printf("\n ");
    for (m=0; m<namesize; m++)
      printf(" ");
    while (cnt<=60 && k>=0){
      if (Sp[k] == Tp[k])
	printf("|");
      else
	printf(" ");
      cnt++; k--;
    }
    cnt = 0;
    printf("\n%s ", names[1]);
    if (strlen(names[1]) < namesize){
      for (m=0; m<namesize-strlen(names[1]); m++)
	printf(" ");
    }
    while (cnt<=60 && j>=0){
      printf("%c", toupper(Tp[j--]));
      cnt++;
    }
    cnt = 0;
  } // while i<size
  
  printf("\n");
} // output

