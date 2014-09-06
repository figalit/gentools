#include <stdio.h>
#include <time.h>
#include <string.h>
#define MAX 175
int max3(int, int, int);
float pairwise(char *, char *);
int sig(char, char);

main(int argc, char **argv){
  int i,j;
  float r;
  char *seqs[1000];
  int cnt=0;
  FILE *in;
  FILE *out;
  for (i=0;i<1000;i++)
    seqs[i]=(char *)malloc(MAX);
  srand(time(NULL));
  in = fopen(argv[1],"r");
  out = fopen(argv[2],"w");
  while (fscanf(in,"%s",seqs[cnt++]) > 0 )
    ;
  fprintf(out,"#NEXUS\nbegin distances;\n\tdimensions ntax=%d;\n\tformat nodiagonal;\n\tmatrix\n");
  
  for (i=0;i<cnt;i++){
    fprintf(out,"\t  seq_%d ",i);
    for (j=0;j<i;j++){
      r=pairwise(seqs[i], seqs[j]);
      printf("Sequences [%d, %d] Aligned\n",i,j);
      fprintf(out,"%f ",r);
    }
    if (i!=cnt)
      fprintf(out,"\n");
    else
      fprintf(out,";\n");
  }
  fprintf(out,"end;\n");
}


float pairwise(char *S, char *T){
  int tSize, sSize; // textSize, sourceSize
  int Wg=0, Ws=1; // gap penalty , space penalty
  int **V, **F, **E, **G; // tables
  //int V[MAX+1][MAX+1], G[MAX+1][MAX+1], E[MAX+1][MAX+1], F[MAX+1][MAX+1];
  int i, j;
  float ret;
  tSize = strlen(T);
  sSize = strlen(S);
  /* create vectors */
  V = (int **)malloc(sizeof(int)*(sSize+1));
  F = (int **)malloc(sizeof(int)*(sSize+1));
  E = (int **)malloc(sizeof(int)*(sSize+1));
  G = (int **)malloc(sizeof(int)*(sSize+1));
  for (i=0;i<=sSize;i++){
    V[i]=(int *)malloc(sizeof(int)*(tSize+1));    
    F[i]=(int *)malloc(sizeof(int)*(tSize+1));
    E[i]=(int *)malloc(sizeof(int)*(tSize+1));
    G[i]=(int *)malloc(sizeof(int)*(tSize+1));
  } // for 
  /* initialize vectors */
  V[0][0] = 0;
  E[0][0] = -10000;
  G[0][0] = -10000;
  F[0][0] = -10000;
  for (i=1;i<=sSize;i++){
    V[i][0] = -Wg - i*Ws;
    E[i][0] = -10000; /* -infinity */
  }
  for (j=1;j<=tSize;j++){
    V[0][j] = -Wg - j*Ws;
    F[0][j] = -10000; /* -infinity */
  }
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      G[i][j] = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
    }

  ret = V[sSize][tSize];
  for (i=0;i<=sSize;i++){  
    free(V[i]);
    free(F[i]);
    free(E[i]);
    free(G[i]);
  } // for
  free(V);
  free(G);
  free(E);
  free(F); 
  return ret;
} // pairwise


int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t)){ 
    if (s!='-')
      return 1;
    else
      return 0;
  } // if
  else
    return 0;
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
