/*
  find out which pairs of sequences are < 10% with the given hor 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SEQ_LENGTH 150

int maxLen;
char **seqs;
char **names;

char **horseqs;
char **hornames;

char *aligned[2];

int horcnt;
int monocnt;

int sig(char s, char t);
void align(char *S, char *T, int **V, int **P);
int max3(int a, int b, int c);
int max(int a, int b);

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
      if (V[i][j] == F[i][j])
	P[i][j] = 1; // match S[i]-"-"
      else if (V[i][j] == E[i][j])
	P[i][j] = 0; // match T[j]-"-"
      else
	P[i][j] = 2; // match S[i]-T[j] 
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
  free(F);
  free(E);
  free(G); 
  free(P); 
  
  return ret;
} // pairwise


int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t)){ 
    if (s!='-')
      return 5;
    else
      return 0;
  } // if
  else
    return -4;
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

  strcpy(aligned[0], Sp);
  strcpy(aligned[1], Tp);
  
} // align

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




int readFasta(FILE *alnFile, FILE *horFile){
  int cnt;
  char ch; 
  int i;
  int seqcnt=0, seqlen=0;
  int horseqcnt=0, horseqlen=0;

  cnt = 0; i=0;

  rewind(alnFile);
  while (fscanf(alnFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
    if (cnt == 1){
      while (fscanf(alnFile, "%c", &ch) > 0){
	if (ch!='>' && ch!='\r' && ch!='\n')
	  i++;
	if (ch == '>'){
	  cnt++;
	  break;
	}
      }
      seqlen = i;
    }
  }

  seqcnt = cnt;

  cnt = 0; i=0;

  rewind(horFile);

  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
    if (cnt == 1){
      while (fscanf(horFile, "%c", &ch) > 0){
	if (ch!='>' && ch!='\r' && ch!='\n')
	  i++;
	if (ch == '>'){
	  cnt++;
	  break;
	}
      }
      horseqlen = i;
    }
  }

  
  horseqcnt = cnt;
  
  rewind(alnFile);
  printf("seqcnt: %d seqlen: %d\n", seqcnt, seqlen);
  printf("horseqcnt: %d horseqlen: %d\n", horseqcnt, horseqlen);

  horseqs = (char **) malloc((horseqcnt+1) * sizeof(char *));
  horcnt = horseqcnt;
  
  for (i=0; i<=horseqcnt; i++)
    horseqs[i] = (char *) malloc(horseqlen+2);
  
  hornames = (char **) malloc((horseqcnt+1) * sizeof(char *));

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen+2);
  

  for (i=0; i<=horseqcnt; i++)
    hornames[i] = (char *) malloc(100);

  for (i=0; i<horseqcnt; i++){
    horseqs[i][0] = 0;
    hornames[i][0] = 0;
  }

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(100);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
  }

  cnt = -1; 
  while (fscanf(alnFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(names[cnt], SEQ_LENGTH, alnFile);
      names[cnt][strlen(names[cnt])-1] = 0;
      printf("seq-%d: %s\n", cnt, names[0]);
    }
    i = 0;
    if (cnt != 0)
      seqs[cnt][i++] = ch;
    do{
      if (!(fscanf(alnFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	seqs[cnt][i++] = ch;
    } while (ch != '>');
    seqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(names[cnt], SEQ_LENGTH, alnFile);
	names[cnt][strlen(names[cnt])-1] = 0;
	printf("seq-%d: %s\n", cnt, names[cnt]);
      }
    } // if
  } // while
	    
  maxLen = strlen(seqs[0]);
  printf("[OK] %d sequences read\n",seqcnt);


  monocnt = seqcnt;

  // hor file
  cnt = 0; i=0;

  /*
  rewind(horFile);

  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
    if (cnt == 1){
      while (fscanf(horFile, "%c", &ch) > 0){
	if (ch!='>' && ch!='\r' && ch!='\n')
	  i++;
	if (ch == '>'){
	  cnt++;
	  break;
	}
      }
      seqlen = i;
    }
  }

  seqcnt = cnt;
  */
  rewind(horFile);

  cnt = -1; 
  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(hornames[cnt], SEQ_LENGTH, horFile);
      hornames[cnt][strlen(hornames[cnt])-1] = 0;
      printf("seq-%d: %s\n", cnt, hornames[0]);
    }
    i = 0;
    if (cnt != 0)
      horseqs[cnt][i++] = ch;
    do{
      if (!(fscanf(horFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	horseqs[cnt][i++] = ch;
    } while (ch != '>');
    horseqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(hornames[cnt], SEQ_LENGTH, horFile);
	hornames[cnt][strlen(hornames[cnt])-1] = 0;
	printf("seq-%d: %s\n", cnt, hornames[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read\n",horseqcnt);


  return seqcnt;
} // readFasta



int main(int argc, char **argv){
  FILE *horfile;
  FILE *fasta;
  int noofsequences;
  int i,j,k;
  float distance;
  int *distances;
  FILE *outfasta;
  char fname[100];
  int hor=0;
  float score;

  if (argc != 3){
    printf("Usage: %s [fastaFile] [hor file]\n", argv[0]);
    exit(0);
  }

  fasta = fopen(argv[1], "r");
  if (fasta == NULL)
    exit(0);
  horfile = fopen(argv[2], "r");
  if (horfile == NULL)
    exit(0);

  

  noofsequences = readFasta(fasta, horfile);

  for (i=0;i<2;i++)
    aligned[i]=(char *)malloc(strlen(seqs[i]) * 3);

  noofsequences=monocnt;

  distances=(int *) malloc(sizeof(int) * noofsequences);

  sprintf(fname, "%s.marked", argv[1]);
  outfasta = fopen(fname, "w");


  for (i=0;i<noofsequences;i++)
    distances[i]=0;

  printf("mono %d   hor %d\n", monocnt, horcnt);

  for (i=0; i<monocnt; i++){
    for (j=0; j<horcnt; j++){
      score = pairwise(seqs[i], horseqs[j]);
      printf("Sequences [%d, %d] Aligned: %f \n",i,j,score);
      distance=hamming(aligned[0], aligned[1]);
      printf("Hamming [%d, %d] Computed: %f \n",i,j,distance);
      if (distance <= 0.1)
	distances[i] = 1;
    }
  }

  for (i=0;i<monocnt;i++){
    if (distances[i] == 1){
      hor++;
      fprintf(outfasta, ">[HOR]_%s\n", names[i]);
      k=0;
      for (j=0;j<strlen(seqs[i]);j++){
	if(seqs[i][j]!='-'){
	  fprintf(outfasta, "%c", seqs[i][j]);
	  k++;
	  if (k%60==0 && k!=0)
	    fprintf(outfasta, "\n");
	}
      }
      fprintf(outfasta, "\n");
    }
    else{
      fprintf(outfasta, ">[MONO]_%s\n", names[i]);
      k=0;
      for (j=0;j<strlen(seqs[i]);j++){
	if(seqs[i][j]!='-'){
	  fprintf(outfasta, "%c", seqs[i][j]);
	  k++;
	  if (k%60==0 && k!=0)
	    fprintf(outfasta, "\n");
	}
      }
      fprintf(outfasta, "\n");    
    }
  }

  printf("%d sequences are marked as HOR, %d are MONO\n", hor, (monocnt-hor));
  return 1;
} 
