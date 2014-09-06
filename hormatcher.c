/*
  As of Dec 14, 2005
  TODO:
  dump the hor-match patterns into seperate files.
  
  try with g248-finalhor: d8z2 first
  
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "pimp.h"
#define SEQ_LENGTH 150


char **hornames;
char **horseqs;
char **names;
char **seqs;
int  *clustered;

void readFasta(FILE *, FILE *, int *, int *);
//void trim(char *);
//int my_strcmp(char *, char *, int);
void kimura(char *, char *, double *, double *);


int main(int argc, char **argv){
  char str[1000];
  FILE *fasta;
  FILE *hor;
  FILE *log;
  FILE *histogram;
  FILE *plotfile;
  int i,j;
  int count;
  int nseq, nhor;
  int *freqs;
  int swapfreq;
  int mismatch;
  double distance, se_distance;

  lemming = 0;
  if (argc != 4 && argc!=5){
    fprintf(stderr, "Matches sequences to (exact or with given threshold) highorders given a two fasta-hor files.\n");
    fprintf(stderr, "%s [fasta file][hor fasta][threshold] <-align>\n", argv[0]);
    exit(0);
  }
  
  if (argc==5) 
    if (!strcmp(argv[4], "-align")) lemming = 1;

  fasta = fopen(argv[1], "r");
  hor = fopen(argv[2], "r");
  sprintf(str, "%s-%s.m%s.log", argv[1],argv[2], argv[3]);
  log = fopen(str, "w");
  sprintf(str, "%s-%s.m%s.histogram", argv[1],argv[2],argv[3]);
  histogram = fopen(str, "w");
  mismatch = atoi(argv[3]);

  readFasta(fasta, hor, &nseq, &nhor);
  fprintf(stderr, "%d monomers, %d highorders are read into memory.\n", nseq,nhor);
  makevectors();
  freqs = (int *) malloc(sizeof(int)*nhor);
  for (i=0;i<nhor;i++){
    freqs[i] = 0;
  }

  for (i=0;i<nhor;i++){
    count = 0;
    fprintf(log, "HIGH ORDER: %s\n----------\n", hornames[i]);
    for (j=0;j<nseq;j++){
      //my_strcmp(horseqs[i], seqs[j], mismatch);
      //kimura(Sp, Tp, &distance, &se_distance);
      //if (distance < 0.05){
      //if (mismatch){
	//printf("Sp:%s\nTp:%s\n",Sp,Tp);
	//printf("distance: %f\n", distance);
 	if (!my_strcmp(horseqs[i], seqs[j], mismatch)){
	//if (count == 0)
	fprintf(log, "%s\n", names[j]);
	count++;
	clustered[j]++;
      }
    }
    //    if (count != 0)
    fprintf(log, "%s MATCHES: %d\n\n", hornames[i], count);
    ///fprintf(histogram, "%d\n",count);
    freqs[i] = count;
    printf("\r%f%%", (float)(i+1)/(float)(nhor)*100);
    fflush(stdout);
  }
  


  /*
  for (i=0;i<nseq;i++)
    if (clustered[i]>1){
      printf("yes there are some invaders around: %s.\n", names[i]);
      //      return 1;
    }
  */

  // sort freq, reorder names
  
  for (i=0;i<nhor;i++){
    for (j=1;j<nhor;j++){
      if (i<j && freqs[i]<freqs[j]){
	swapfreq = freqs[i];
	freqs[i] = freqs[j];
	freqs[j] = swapfreq;
	strcpy(str, hornames[i]);
	strcpy(hornames[i], hornames[j]);
	strcpy(hornames[j], str);
      }
    }
  }

  plotfile=fopen("plot", "w");
  fprintf(plotfile, "set terminal postscript eps enhanced\n");
  fprintf(plotfile, "set output \"%s-%s.m%s.histogram.eps\"\n",argv[1],argv[2],argv[3]);
  fprintf(plotfile, "set size 2,2\n");
  fprintf(plotfile, "set bmargin 15\nset tmargin -5\n");
  fprintf(plotfile, "unset xtics\n");
  for (i=0;i<nhor;i++){
    fprintf(plotfile, "set label \"%s\" at first %d,0 rotate by -90 font \"Times,8\" \n", hornames[i], i);
    fprintf(histogram, "%d\n", freqs[i]);
  }
  fprintf(plotfile, "plot \"%s-%s.m%s.histogram\" with boxes\n", argv[1], argv[2],argv[3]);
  fclose(histogram);
  fclose(plotfile);
  system("gnuplot plot");
  sprintf(str, "%s-%s.m%s.plotfile", argv[1], argv[2],argv[3]);
  rename("plot", str);
  printf("\n");


  return 1;
}



void readFasta(FILE *fastaFile, FILE *horFile, int *nseq, int *nhor){
  int cnt;
  char ch; 
  int i;
  int seqcnt=0, seqlen=0;
  int horseqcnt=0, horseqlen=0;

  cnt = 0; i=0;

  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }

  seqcnt = cnt;
  seqlen = 250;

  cnt = 0; i=0;

  rewind(horFile);

  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }


  horseqlen = 250;
  horseqcnt = cnt;
  
  rewind(fastaFile);
  fprintf(stderr, "seqcnt: %d seqlen: %d\n", seqcnt, seqlen);
  fprintf(stderr, "horseqcnt: %d horseqlen: %d\n", horseqcnt, horseqlen);

  horseqs = (char **) malloc((horseqcnt+1) * sizeof(char *));
  
  for (i=0; i<=horseqcnt; i++)
    horseqs[i] = (char *) malloc(horseqlen);
  
  hornames = (char **) malloc((horseqcnt+1) * sizeof(char *));

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen);

  clustered = (int *) malloc((seqcnt+1) * sizeof(int));
  

  for (i=0; i<=horseqcnt; i++)
    hornames[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<horseqcnt; i++){
    horseqs[i][0] = 0;
    hornames[i][0] = 0;
  }

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
    clustered[i] = 0;
  }

  cnt = -1; 
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(names[cnt], SEQ_LENGTH, fastaFile);
      names[cnt][strlen(names[cnt])-1] = 0;
      //printf("seq-%d: %s\n", cnt, names[0]);
    }
    i = 0;
    if (cnt != 0)
      seqs[cnt][i++] = ch;
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	seqs[cnt][i++] = ch;
    } while (ch != '>');
    seqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(names[cnt], SEQ_LENGTH, fastaFile);
	names[cnt][strlen(names[cnt])-1] = 0;
	//printf("seq-%d: %s\n", cnt, names[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read from fasta\n",seqcnt);


  // hor file
  cnt = 0; i=0;

  rewind(horFile);

  cnt = -1; 
  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(hornames[cnt], SEQ_LENGTH, horFile);
      trim(hornames[cnt]);
      //      hornames[cnt][strlen(hornames[cnt])-1] = 0;
      //printf("seq-%d: %s\n", cnt, hornames[0]);
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
	trim(hornames[cnt]);
	//hornames[cnt][strlen(hornames[cnt])-1] = 0;
	//printf("horseq-%d: %s\n", cnt, hornames[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read\n",horseqcnt);

  *nseq = seqcnt;
  *nhor = horseqcnt;
} 

/*
void trim(char *name){
  int i=0;
  int dash=0;
  while (!isspace(name[i++])){
    if (name[i-1] == '-') dash++;
    if (dash == 2) break;
  }
   
  name[i-1]=0;
}

*/

/*
int my_strcmp(char *s1, char *s2, int mismatch){
	      // return 0 if match is ok
	      // 1 if not ok enough
	      // backward compability thingy
  
  int mcnt=0;
  int i, len;

  if (mismatch == 0)
    return strcmp(s1,s2);

  len = strlen(s1);
  if (strlen(s2) != len)
    return 1;

  for (i=0;i<len;i++){
    if (s1[i] != s2[i] && s1[i]!='-' && s2[i] != '-') mcnt++;
    if (mcnt > mismatch) return 1;
  }

  return 0;

  }*/



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

  //  printf("\n---------------\n\n%s\n%s\n\n\n", s1, s2);                                                                                                                                                                                                              

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
