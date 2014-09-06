#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define MAX 180
int max3(int, int, int);
float pairwise(char *, char *);
int sig(char, char);
void align(char *, char *, int **, int **);
double hamming(char *S, char *T);

char *aligned[2];

main(int argc, char **argv){
  int i,j,k;
  float r;
  double **tmat;
  int histogram[101];
  char *seqs[1000];
  char outfile[30];
  int cnt=0;
  int index;
  int histStart=0;
  FILE *in;
  FILE *out;
  for (i=0;i<1000;i++)
    seqs[i]=(char *)malloc(MAX);
  for (i=0;i<101;i++)
    histogram[i]=0;
  for (i=0;i<2;i++)
    aligned[i]=(char *)malloc(MAX*3);
  srand(time(NULL));
  in = fopen(argv[1],"r");
  sprintf(outfile,"%s.histogram",argv[1]);
  //out = fopen(argv[2],"w");
  out = fopen(outfile,"w");
  while (fscanf(in,"%s",seqs[cnt++]) > 0 )
    ;
  cnt--;
  //tmat = (double **)malloc(sizeof(double *) * cnt);
  //for (i=0;i<=cnt;i++)
  //tmat[i] = (double *)malloc(sizeof(double) * (i+1));
  
  for (i=0;i<cnt;i++){
    for (j=0;j<i;j++){
      r=pairwise(seqs[i], seqs[j]);
      printf("Sequences [%d, %d] Aligned: %f \n",i,j,r);
      r=hamming(aligned[0], aligned[1]);
      //if (r <= 0.05){
      //fprintf(out,"%5d %5d %10f\n",i,j,r);
      //}
      index=(int)rint(r*100);
      histogram[index]++;
      //if (i==48 && j==5){
      //printf("%s \n%s\n",aligned[0],aligned[1]);
      //exit(0);
      //}
	//fprintf(out,"%f ",r);
      //tmat[i][j]=hamming(aligned[0], aligned[1]);
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
  //fprintf(out,"   ");
  //for(k=0;k<cnt;k+=12){
  /*
    i=0;
    fprintf(out,"%6d",i); 
    for (i=1;i<cnt;i++)
      fprintf(out,"%9d",i); 
    fprintf(out,"\n");
    for (i=0;i<cnt;i++)
      fprintf(out,"---------"); 
    fprintf(out,"\n");
    
    //for (i=k;i<k+12;i++){
    for (i=0;i<cnt;i++){
      //if (i>=cnt)
      //break;
      fprintf(out,"%d ",i); 
      for (j=0;j<i;j++){
	fprintf(out,"%5f ", tmat[i][j]);
      }
      fprintf(out,"\n");
      }
    //fprintf(out,"\n\n\n");
    //}
  */

  // print histogram
  for (i=100; i>=0; i--){
    if (histogram[i] != 0)
      histStart=1;
    if (histStart)
      fprintf(out,"%3d %5d\n",i,histogram[i]);
  }
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

double hamming(char *S, char *T){
  int i;
  int similar=0;
  int length;
  int prev=0;
  double percentage;
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
  
  percentage = (1 - (double)similar/(double)length );

  if (percentage == 0.0 && strcmp(S,T)){
    printf("percentage: %f\n", (1 - (double)similar/(double)length ));
    printf("similar: %d length: %d\n",similar,length);
    printf("seq1: %s\n\n",S);
    printf("seq0: %s\n\n",T);
    exit(0); 
  }
  return (1 - (double)similar/(double)length );
}




