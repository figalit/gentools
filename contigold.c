#include <stdio.h>
#include <string.h>
#include <ctype.h>
void search(char [], FILE *);

main(int argc, char **argv){
  char ch;
  char sequence[200];
  FILE *check=fopen(argv[1],"r");
  FILE *fasta=fopen(argv[2],"r");
  int cnt;
  int seqcnt=0;
  int exit=0;
  sequence[0]=0;
  fscanf(check, "%c", &ch);
  while (1){
    if (ch=='>')
      while (ch!='\n' && ch!='\r')
	if (fscanf(check, "%c", &ch) <= 0){
	  exit=1;
	  break;
	}
    
    if (exit)
      return;
    
    cnt=0;
    seqcnt++;
    while (ch!='>'){
      if (fscanf(check, "%c", &ch) <= 0){
	exit=1;
	break;
      }
      if (ch!='\r' && ch!='\n' && ch!='>')
	sequence[cnt++]=ch;
    }
    sequence[cnt]=0;
    search(sequence, fasta);
    if (exit)
      return;
  } // while
} // main

void search(char sequence[], FILE *fasta){
  int contigcnt=1;
  int ch;
  int cnt=0;
  int ncnt=0;
  rewind(fasta);

  printf("%s\n",sequence);

  while (fscanf(fasta,"%c", &ch) > 0 ){
    if (cnt == strlen(sequence)-1){
      printf("Contig #%d\n",contigcnt);
      return;
    } // if
    if (ch != '\n' && ch != '\r'){
      if (sequence[cnt++] != ch)
	cnt=0;
    }
    if (ch == 'N')
      ncnt++;
    else if (ch != '\n' && ch != '\r')
      ncnt=0;
    if (ncnt==100)
      contigcnt++;
  } // while
} // search











