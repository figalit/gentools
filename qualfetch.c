/* 
   qualfetc.c
   Given a fasta-like qual file and a file of list of sequences to be fetched
   it fetches those sequences from that fasta file
   Last update: Dec 14, 2005
   Can Alkan
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NOOFDEL 100000
#define MAXQUALLEN 5000

int deletethis(char *, int, int *, int *);
void dumpq(int, int, int);
char **toberemoved;
int start[NOOFDEL];
int end[NOOFDEL];
int qvalue[MAXQUALLEN];
FILE *quarantine;


int main(int argc, char **argv){

  FILE *fasta;
  FILE *del;
  char str[150];
  char dum1[8];
  char dum2[5];
  char buf[4];
  int delcnt;
  int i, j, mstart, mend, qlen;
  int wflag;
  char ch;
  int deleted;

  if (argc != 3){
    fprintf(stderr, "Given a fasta-like file and a file of list of sequences to be fetched\nit fetches those sequences from that fasta file.\nLast update: Dec 14, 2005.\n");
    fprintf(stderr, "Usage: %s [fastafile] [listtofetch]\n", argv[0]);
    return 0;
  }

  fasta = fopen(argv[1], "r");
  del   = fopen(argv[2], "r");

  if (fasta == NULL || del == NULL){
    fprintf(stderr, "File opening error.\n");
    return 0;
  }

  sprintf(str, "%s.qual", argv[2]);

  /* check if exists */
  quarantine = fopen(str, "r");
  if (quarantine != NULL){
    fprintf(stderr, "%s already exists, skipping.\n", str);
    return 0;
  }

  quarantine = fopen(str, "w");
  
  toberemoved = (char **) malloc(NOOFDEL * sizeof(char *));

  for (i=0;i<NOOFDEL;i++){
    toberemoved[i] = (char *) malloc(150 * sizeof(char));
  }

  i=0;
  while (fscanf(del, "%s %s %s %d %d", str, dum1, dum2, &mstart, &mend) > 0){
    strcpy(toberemoved[i], str);
    start[i] = mstart;
    end[i] = mend;
    i++;
  }
  delcnt = i;
  
  fprintf(stderr, "%d sequences will be deleted.\n", delcnt);
  
  wflag = 1;
  deleted = 0;
  i=1; j=0; qlen=0;
  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch == '>'){
      if (!wflag){
	dumpq(mstart, mend, i);
      }
      i=1; j=0; qlen=0;
      if (deleted == delcnt)
	break;
      fgets(str, 150, fasta);
      str[strlen(str)-1] = 0;
      if (deletethis(str, delcnt, &mstart, &mend)){
	deleted++;
	if (mstart<mend)
	  fprintf(quarantine, ">%s %d-%d\n", str, mstart, mend);
	else
	  fprintf(quarantine, ">%s %d-%d Reverse Complemented\n", str, mend, mstart);	  
	wflag = 0;
      }
      
      else{
	wflag = 1;
      }
      
    }
    if (wflag && ch != '>')
      ;
    else if (!wflag && isspace(ch)){
      if(j!=0){
	buf[j]=0;
	qvalue[i] = atoi(buf);
	//fprintf(stderr, " %d ", qvalue[i]);
	j=0; i++;
      }
      //fprintf(quarantine, "%c", ch);
    }
    else if (!wflag && isdigit(ch)){
      buf[j++]=ch;
    }
  }
  return 1;    
}


int deletethis(char *str, int delcnt, int *s, int *e){
  int i;
  
  for (i=0;i<delcnt;i++){
    if (strstr(str, toberemoved[i])){
      *s = start[i];
      *e = end[i];
      return 1;
    }
  }
  
  return 0;
}


void dumpq(int mstart, int mend, int qlen){
  int i;
  int cnt=0;
  //fprintf(stderr, "dump %d %d; len: %d\n", mstart, mend, qlen);
  if (mstart<mend){ //forward
    for (i=mstart;i<=mend;i++){
      if (qvalue[i]>=10)
	fprintf(quarantine, "%d ", qvalue[i]);
      else
	fprintf(quarantine, "0%d ", qvalue[i]);
      if ((i-mstart+1)%50==0)
	fprintf(quarantine, "\n");
    }
  }
  else{
    for (i=mend;i<=mstart;i++){
      if (qvalue[i]>=10)
	fprintf(quarantine, "%d ", qvalue[i]);
      else
	fprintf(quarantine, "0%d ", qvalue[i]);
      if ((i-mend+1)%50==0)
	fprintf(quarantine, "\n");
    }
  }
  fprintf(quarantine, "\n");
  fflush(quarantine);
}
