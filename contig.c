#include <stdio.h>
#include <string.h>
#include <ctype.h>
void search(int, int, FILE *);

int count=0;
int contigcnt=1;
//int ncnt=0;

main(int argc, char **argv){
  char ch;
  char sequence[200];
  FILE *check=fopen(argv[1],"r");
  FILE *fasta=fopen(argv[2],"r");
  int cnt;
  int from, to;
  int seqcnt=0;
  int exit=0;
  sequence[0]=0;

  while (fscanf(check, "%c", &ch) > 0){
    if (ch=='>'){
      fscanf(check, "%s", sequence); // skip name
      fscanf(check, "%s", sequence); // skip from
      fscanf(check, "%s", sequence); // get fronnum
      from = atoi(sequence);
      fscanf(check, "%s", sequence); // skip to
      fscanf(check, "%s", sequence); // get to
      to = atoi(sequence);
      search(from, to, fasta);
    }
    
  } // while
} // main

void search(int from, int to, FILE *fasta){

  char ch;
  int cnt=0;
  int ncnt=0;
  count=0;
  contigcnt=1;
  rewind(fasta);
  
  while (fscanf(fasta,"%c", &ch) > 0 ){
    count++;
    if (from==count){
      printf("From: %d Contig : #%d\n",from,contigcnt);
      break;
    }
    //printf("%c",ch);
    if (ch == 'N'){
      //printf("ncnt: %d\n",ncnt);
      ncnt++;
    }    
    else if (ch != '\n' && ch != '\r')
      ncnt=0;
    if (ncnt==100){
      contigcnt++;
      ncnt=0;
    }
  } // while
} // search











