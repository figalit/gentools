/*
  find out which pairs of sequences are < 10% with the given hor 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

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
void kimura(char *, char *, double *, double *);
int **V, **F, **E, **G, **P; // tables
void rcomp(char *, char *);

double pairwise(char *S, char *T){
  int tSize, sSize; // textSize, sourceSize
  int Wg=16, Ws=4; // gap penalty , space penalty

  //int V[MAX+1][MAX+1], G[MAX+1][MAX+1], E[MAX+1][MAX+1], F[MAX+1][MAX+1];
  int i, j;
  double ret;
  tSize = strlen(T);
  sSize = strlen(S);
  //printf("%d-%d\n",sSize, tSize);
  if (sSize==358 && tSize==1065)
    printf("eh\n");
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
      return -4;
  } // if
  //else if (toupper(s)=='N' || toupper(t)=='N')
  // return 0;
  else
    return -3;
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


void align2(char *S, char *T, int **V, int **P){
  int sSize = strlen(S);
  int tSize = strlen(T);
  int i,j,k; int c2=0;
  char *Sp, *Tp; // Sprime, Tprime
  char *Sp2, *Tp2; // Sprime, Tprime
  Tp = (char *)malloc((tSize+sSize)*2);
  Sp = (char *)malloc((tSize+sSize)*2);
  Tp[0]=0;
  Sp[0]=0;
  Tp2 = (char *)malloc((tSize+sSize)*2);
  Sp2 = (char *)malloc((tSize+sSize)*2);
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

  strcpy(aligned[0], Sp);
  strcpy(aligned[1], Tp);

  free(Sp); free(Tp); free(Sp2); free(Tp2);

} // align



void alignorig(char *S, char *T, int **V, int **P){
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

  /*

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

  */


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
  free(Sp); free(Tp); free(Sp2); free(Tp2);
  
} // align

void align(char *S, char *T, int **V, int **P){
  int sSize = strlen(S);
  int tSize = strlen(T);
  int i,j,k; int c2=0;
  char *Sp, *Tp; // Sprime, Tprime
  //  char *Sp2, *Tp2; // Sprime, Tprime
  int ii, jj;
  
  Tp = (char *)malloc((tSize+sSize)*2); //(tSize*2);
  Sp = (char *)malloc((tSize+sSize)*2); //(sSize*2);
  Tp[0]=0;
  Sp[0]=0;
  //Tp2 = (char *)malloc(tSize*2);
  //Sp2 = (char *)malloc(sSize*2);
  //Tp2[0]=0;
  //Sp2[0]=0;
  i = sSize; j = tSize;
  
  //printf("align:\n%s\n%s\n%d-%d\n", S, T, sSize, tSize);

  ii=0; jj=0;

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
  Sp[ii] = 0;
  Tp[jj] = 0;

  strcpy(aligned[0], Sp);
  strcpy(aligned[1], Tp);
  free(Sp); free(Tp);// free(Sp2); free(Tp2);
  
} // align


double hamming(char *S, char *T){
  int i;
  int similar=0;
  int length;
  int prev=0;
  double percentage;
  for (i=0;i<strlen(S);i++)
    if (S[i] == T[i])// && S[i]!='-')
      similar++;

  
  
  //printf("SEQUENCES\n%s\n%s\n",S, T);

  length=strlen(S);

  /*
  for (i=0;i<strlen(S);i++)
    if (S[i] != T[i] && S[i]!='-' && T[i]!='-')
      length++;
  */

  /*
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
      }*/
  
  percentage = (1 - (double)similar/(double)length );

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
  char dummy[300];
  int j;

  cnt = 0; i=0;
  seqlen = 2000; horseqlen = 2000;
  
  rewind(alnFile);
  while (fscanf(alnFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(dummy, 300, alnFile);
    }
    /*
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
      } */
    
  }

  seqcnt = cnt;

  cnt = 0; i=0;

  rewind(horFile);

  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(dummy, 300, horFile);
    }
    /*
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
    */
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
    hornames[i] = (char *) malloc(200);

  for (i=0; i<horseqcnt; i++){
    horseqs[i][0] = 0;
    hornames[i][0] = 0;
  }

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(200);

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
      for (j=0;j<strlen(names[cnt]);j++){
	if (names[cnt][j]=='\t')
	  names[cnt][j] = ' ';
      }
      printf("seq-%d: %s\n", cnt, names[0]);
    }
    else if (isspace(ch))
      continue;

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
	for (j=0;j<strlen(names[cnt]);j++){
	  if (names[cnt][j]=='\t')
	    names[cnt][j] = ' ';
	}
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


  rewind(horFile);

  cnt = -1; 
  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(hornames[cnt], SEQ_LENGTH, horFile);
      hornames[cnt][strlen(hornames[cnt])-1] = 0;
      for (j=0;j<strlen(hornames[cnt]);j++){
	if (hornames[cnt][j]=='\t')
	  hornames[cnt][j] = ' ';
      } 
      hornames[cnt][strlen(hornames[cnt])-1] = 0;
      printf("hseq-%d: %s\n", cnt, hornames[0]);
    }
    else if (isspace(ch))
      continue;
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
      if (cnt != horseqcnt){
	fgets(hornames[cnt], SEQ_LENGTH, horFile);
	hornames[cnt][strlen(hornames[cnt])-1] = 0;
	for (j=0;j<strlen(hornames[cnt]);j++){
	  if (hornames[cnt][j]=='\t')
	    hornames[cnt][j] = ' ';
	}
	printf("hseq-%d: %s\n", cnt, hornames[cnt]);
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
  double distance;
  //double **distances;
  double se_distance;
  //double **se_distances;
  
  double totdist = 0.0;
  double totsedist = 0.0;

  FILE *outfasta;
  

  char fname[100];
  int hor=0;
  double score;
  char *revseq;

  int totalalignments;

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
    //aligned[i]=(char *)malloc(strlen(seqs[i]) * 3);
    aligned[i]=(char *)malloc(2000 * 5);

  revseq=(char *)malloc(2000 * 5);

  noofsequences=monocnt;

  /*
  distances=(double **) malloc(sizeof(double *) * monocnt);
  for (i=0;i<monocnt;i++)
    distances[i]=(double *) malloc(sizeof(double) * horcnt);
  */

  /*

  se_distances=(double **) malloc(sizeof(double *) * monocnt);
  for (i=0;i<monocnt;i++)
    se_distances[i]=(double *) malloc(sizeof(double) * horcnt);
  */

  sprintf(fname, "%s-%s.matrix", argv[1], argv[2]);
  outfasta = fopen(fname, "w");


  /*
  for (i=0;i<monocnt;i++)
    for (j=0;j<horcnt;j++)
      distances[i][j]=0.0;
  */

  totalalignments = monocnt*horcnt;

  fprintf(outfasta, "\t\t");
  for (j=0; j<horcnt; j++)
    fprintf(outfasta, "%s\t", hornames[j]);
  fprintf(outfasta, "\n");
 
  for (i=0; i<monocnt; i++){
    fprintf(outfasta, "%s\t\t", names[i]);
    for (j=0; j<horcnt; j++){
      if (i>j){
	score = pairwise(seqs[i], horseqs[j]);
	distance=hamming(aligned[0], aligned[1]);
      }
      else if (i<j){
	rcomp(horseqs[j], revseq);
	score = pairwise(seqs[i], revseq);
	distance = hamming(aligned[0], aligned[1]);
      }
      else{
	score = 0;
	distance = 0;
      }

      //kimura(aligned[0], aligned[1], &distance, &se_distance);

      /*
      if (!(distance>=0.0 && distance<=1.0)){
	fprintf(stderr, "Wrong Kimura : %d-%d\n%s\t%s\n", i, j, names[i], hornames[j]);
	//exit(0);
      }
      */

      //se_distances[i][j] = se_distance;
      if (distance<1.0 && distance>0.0 && se_distance>0.0 && se_distance<1.0){
	totdist += distance;
	totsedist += se_distance;
      }
      else{
	totalalignments--;
      }

      fprintf(outfasta, "%f\t", distance);

      if (distance < 0.1 && i!=j)
	printf("%s\t%s\t%f\n", names[i], names[j], distance);

      /*
      fprintf(outfasta, "%s\t%s\n", names[i], hornames[j]);
      fprintf(outfasta, "%s\n", aligned[0]);
      fprintf(outfasta, "%s\n\n", aligned[1]);
      fflush(outfasta);

      */


      //distances[i][j] = distance;
     

    }
    fprintf(outfasta, "\n");
  }
  
  
  fprintf(stdout, "Total K2M: %f\n", (totdist));
  fprintf(stdout, "Total Alignments: %d\tof\t%d\n", totalalignments, (monocnt*horcnt));
  fprintf(stdout, "Average K2M: %f\n", (totdist/ (double)(totalalignments)));
  fprintf(stdout, "Average SE_K2M: %f\n", (totsedist/ (double)(totalalignments)));


  fprintf(outfasta, "\n");
  fprintf(outfasta, "Average K2M: %f\n", (totdist/ (double)(totalalignments)));
  fprintf(outfasta, "Average SE_K2M: %f\n", (totsedist/ (double)(totalalignments)));

  
  /*
  
  if(!strcmp(argv[1],argv[2])){
    for (i=0;i<horcnt-2;i++){
      score = pairwise(seqs[i], seqs[i+2]);
      kimura(aligned[0], aligned[1], &distance, &se_distance);
      if (distance>0.2){
	fprintf(outfasta, "NOTDIMERIC\n");
      }
    }
  }
  */
  
  return 1;
} 


void kimura(char *s1, char *s2, double *dist, double *se_dist){

  double p;
  double q;
  int transitions; 
  int transversions; 
  int base_spaces;
  int i;
  double a;
  double b;

  double k_kimura;
  double SE_k_kimura;
  
  int ag,ac,at,aa,an;
  int gc,gt, ga, gg,gn;
  int ca,cc,cg,ct,cn;
  int ta,tc,tg,tt,tn;
  int na,nc,ng,nt,nn;


  ag = 0; ac = 0; at = 0; aa = 0; an = 0; 
  gc = 0; gt = 0; gg = 0; ga = 0; gn = 0; 
  ca = 0; cc = 0; cg = 0; ct = 0; cn = 0; 
  ta = 0; tc = 0; tg = 0; tt = 0; tn = 0; 
  na = 0; nc = 0; ng = 0; nt = 0; nn = 0; 


  //printf("%s\n%s\n", s1, s2);

  if(strlen(s1) != strlen(s2)){
    fprintf(stderr, "dude\n");
    exit(0);
  }

  for (i = 0; i<strlen(s1); i++){
    switch (toupper(s1[i])){
    case 'A':
      if (toupper(s2[i]) == 'T') at++;
      else if (toupper(s2[i]) == 'G') ag++;
      else if (toupper(s2[i]) == 'C') ac++;
      else if (toupper(s2[i]) == 'A') aa++;
      else if (toupper(s2[i]) == 'N') an++;
      break;
    case 'C':
      if (toupper(s2[i]) == 'T') ct++;
      else if (toupper(s2[i]) == 'G') cg++;
      else if (toupper(s2[i]) == 'A') ca++;
      else if (toupper(s2[i]) == 'C') cc++;
      else if (toupper(s2[i]) == 'N') cn++;
      break;
    case 'G':
      if (toupper(s2[i]) == 'T') gt++;
      else if (toupper(s2[i]) == 'A') ga++;
      else if (toupper(s2[i]) == 'C') gc++;
      else if (toupper(s2[i]) == 'G') gg++;
      else if (toupper(s2[i]) == 'N') gn++;
      break;
    case 'T':
      if (toupper(s2[i]) == 'G') tg++;
      else if (toupper(s2[i]) == 'A') ta++;
      else if (toupper(s2[i]) == 'C') tc++;
      else if (toupper(s2[i]) == 'T') tt++;
      else if (toupper(s2[i]) == 'N') tn++;
      break;
    case 'N':
      if (toupper(s2[i]) == 'G') ng++;
      else if (toupper(s2[i]) == 'A') na++;
      else if (toupper(s2[i]) == 'C') nc++;
      else if (toupper(s2[i]) == 'T') nt++;
      else if (toupper(s2[i]) == 'N') nn++;
      break;
    default:
      break;
    }
  }


  transversions = at+ta+ac+ca+gc+cg+gt+tg;
  transitions = ag+ga+ct+tc;
  base_spaces = aa+ag+ac+at+ga+gc+gg+gt+ta+tc+tt+tg+ca+cc+cg+ct;

  p = (double)transitions / (double)base_spaces;
  q = (double)transversions / (double)base_spaces;
  a = 1.0 / (1 - 2*p - q);
  b = 1.0 / (1 - 2*q);
  k_kimura = 0.5 * log(a) + 0.25 * log(b);
  SE_k_kimura = sqrt((a*a*p + pow((a+b)/2,2)*q - pow(a*p +(((a+b)/2*q)), 2)) / (double)base_spaces);

  /*
  if (k_kimura>1.0 || k_kimura<0.0){
    fprintf(stderr, "hodooooo\n");
    fprintf(stderr, "transitions:%d\ntransversions:%d\nbasespaces:%d\np:%f\nq:%f\na:%f\nb:%f\nkimura:%f\n",
	    transitions, transversions, base_spaces, p, q, a, b, k_kimura);
  }
  */

  *dist = k_kimura;
  *se_dist = SE_k_kimura;
  
}


void rcomp(char *window, char *rcomp){
  /* reverse complement */
  int i;
  int len = strlen(window);
  for (i=0;i<len;i++)
    switch (toupper(window[i])){
    case 'A':
      rcomp[len-i-1] = 'T';
      break;
    case 'C':
      rcomp[len-i-1] = 'G';
      break;
    case 'G':
      rcomp[len-i-1] = 'C';
      break;
    case 'T':
      rcomp[len-i-1] = 'A';
      break;
    default:
      rcomp[len-i-1] = window[i];
      break;
    }
  rcomp[len] = 0;
}
