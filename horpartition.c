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


int readFasta(FILE *alnFile){
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

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen+2);
  
  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(100);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
  }


  rewind(alnFile);
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

  return seqcnt;
} // readFasta



int main(int argc, char **argv){
  FILE *horfile;
  FILE *monofile;
  FILE *fasta;
  int noofsequences;
  int i,j,k;
  char fname[100];
  float score;

  if (argc != 2){
    printf("Usage: %s [fastaFile]\n", argv[0]);
    exit(0);
  }

  fasta = fopen(argv[1], "r");
  if (fasta == NULL)
    exit(0);
  

  noofsequences = readFasta(fasta);


  sprintf(fname, "%s.hor", argv[1]);
  horfile = fopen(fname, "w");
  sprintf(fname, "%s.mono", argv[1]);
  monofile = fopen(fname, "w");


  for (i=0;i<noofsequences;i++){
    printf("%s\n", names[i]);
    if (strstr(names[i], "[HOR]")){
      fprintf(horfile, ">%s\n", names[i]);
      for (j=0;j<strlen(seqs[i]);j++){
	if(seqs[i][j]!='-'){
	  fprintf(horfile, "%c", seqs[i][j]);
	  k++;
	  if (k%60==0 && k!=0)
	    fprintf(horfile, "\n");
	}
      }
      fprintf(horfile, "\n");
    }
    else{
      fprintf(monofile, ">%s\n", names[i]);
      k=0;
      for (j=0;j<strlen(seqs[i]);j++){
	if(seqs[i][j]!='-'){
	  fprintf(monofile, "%c", seqs[i][j]);
	  k++;
	  if (k%60==0 && k!=0)
	    fprintf(monofile, "\n");
	}
      }
      fprintf(monofile, "\n");    
    }
  }

  return 1;
} 
