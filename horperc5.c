#include <stdio.h>
#include <time.h>
#include <string.h>
#define MAX 180
int max3(int, int, int);
float pairwise(char *, char *);
int sig(char, char);

main(int argc, char **argv){
  int i,j;
  float r;
  float **tmat;
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
  cnt--;
  tmat = (float **)malloc(sizeof(float *) * cnt);
  for (i=0;i<=cnt;i++)
    tmat[i] = (float *)malloc(sizeof(float) * (i+1));
  
  for (i=0;i<cnt;i++){
    for (j=0;j<i;j++){
      r=pairwise(seqs[i], seqs[j]);
      printf("Sequences [%d, %d] Aligned: %f\n",i,j,r);
      //fprintf(out,"%f ",r);
      tmat[i][j]=r / ((strlen(seqs[i])+strlen(seqs[j]))/2);
    }
  }
    /*
      if (i!=cnt)
      fprintf(out,"\n");
      else
      fprintf(out,";\n");
      }
      fprintf(out,"end;\n");
    */
  for (i=0;i<cnt;i++)
    fprintf(out,"%10d ",i); 
  fprintf(out,"\n");
  for (i=0;i<cnt;i++)
    fprintf(out,"-----"); 
  fprintf(out,"\n");
  
  for (i=0;i<cnt;i++){
    fprintf(out,"%d ",i); 
    for (j=0;j<i;j++){
      fprintf(out,"%5f ", tmat[i][j]);
    }
    fprintf(out,"\n");
  }
} // main


float pairwise(char *S, char *T){
  int **V; // scoring table
  int tSize, sSize; // textSize, sourceSize
  int i, j, retVal;
  int op1, op2, op3;
  tSize = strlen(T);
  sSize = strlen(S);
  /* create vectors */  
  V = (int **)malloc(sizeof(int)*(sSize+1));
  for (i=0;i<=sSize;i++)
    V[i]=(int *)malloc(sizeof(int)*(tSize+1));
  /* initialize vectors */
  V[0][0] = 0;
  for (i=1;i<=sSize;i++)
    V[i][0] = V[i-1][0] + sig(S[i-1],'-');
  
  for (j=1;j<=tSize;j++)
    V[0][j] = V[0][j-1] + sig(T[i-1],'-');
    
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      op1 = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      op2 = V[i-1][j] + sig(S[i-1], '-');
      op3 = V[i][j-1] + sig('-', T[j-1]);
  
      V[i][j] = max3(op1, op2, op3);
    } // for j
  
  retVal =  V[sSize][tSize];
  
  for (i=0;i<=sSize;i++)
   free(V[i]);
  free(V);
  
  return retVal;

} // pairwise


int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t)){ 
    if (s!='-')
      return 0;
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
