/* 
   qual-tandem-subseq.c
   Given a quals file and a seq.coord file\nit fetches those qual-sequences from that fasta file
   Last update: Dec 28, 2005
   Can Alkan
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NOOFDEL 500000
#define MAXQUALLEN 5000

int deletethis(char *, int, int *, int *, int *, int *);
void dumpq(int, int, int, int, FILE *);
char **toberemoved;
int *alreadyremoved;
int start[NOOFDEL];
int end[NOOFDEL];
int qvalue[MAXQUALLEN];
FILE *quarantine;
int qualthreshold;
int allowed;

int main(int argc, char **argv){

  FILE *fasta;
  FILE *del;
  FILE *log;
  char str[150];
  char dum1[80];
  char dum2[5];
  char buf[4];
  int delcnt;
  int i, j, mstart, mend, qlen;
  int ugly;
  int wflag;
  char ch;
  int index;
  int match;
  int deleted;

  if (argc != 5){
    fprintf(stderr, "Given a quals file and a seq.coord file\nit fetches those qual-sequences from that fasta file.\nLast update: Dec 28, 2005.\n");
    fprintf(stderr, "Usage: %s [fastafile] [listtofetch] [qualthreshold] [allowed-lowqual]\n", argv[0]);
    return 0;
  }

  fasta = fopen(argv[1], "r");
  del   = fopen(argv[2], "r");

  if (fasta == NULL || del == NULL){
    fprintf(stderr, "File opening error.\n");
    return 0;
  }

  qualthreshold = atoi(argv[3]);
  allowed = atoi(argv[4]);

  sprintf(str, "%s.q%d.m%d.qual", argv[2], qualthreshold, allowed);

  /* check if exists */
  quarantine = fopen(str, "r");
  if (quarantine != NULL){
    fprintf(stderr, "%s already exists, skipping.\n", str);
    return 0;
  }

  quarantine = fopen(str, "w");
  sprintf(str, "%s.q%d.m%d.log", argv[2], qualthreshold, allowed);
  log = fopen(str, "w");


  alreadyremoved = (int *) malloc(NOOFDEL * sizeof(int));
  toberemoved = (char **) malloc(NOOFDEL * sizeof(char *));

  for (i=0;i<NOOFDEL;i++){
    toberemoved[i] = (char *) malloc(150 * sizeof(char));
    alreadyremoved[i] = 0;
  }



  i=0;

  fgets(dum1, 80, del); // skip header

  while (fscanf(del, "%s %d %d %s", str, &mstart, &mend, dum1) > 0){
    str[strlen(str)-3]=0; // remove .fa
    strcpy(toberemoved[i], str);
    start[i] = mstart;
    end[i] = mend;
    i++;
    //fprintf(stderr, "%s %d %d\n", str, mstart, mend);
  }
  delcnt = i;
  
  fprintf(stderr, "%d sequences will be checked.\n", delcnt);
  

  rewind(fasta);
  wflag = 1;
  deleted = 0;
  i=1; j=0; qlen=0;
  qvalue[0]=-1;
  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch == '>'){
      //for (ugly=1;ugly<=3;ugly++){
      if (!wflag){
	dumpq(mstart, mend, i, index, log);
	if (match>1){
	  for (ugly=1;ugly<match;ugly++){
	    deleted++;
	    mstart = start[index+ugly];
	    mend   = end[index+ugly];
	    // this is the rest
	    if (mstart<mend)
	      fprintf(quarantine, ">lcl|%s from %d to %d (%d)(F)\n", toberemoved[index], mstart, mend, (mend-mstart+1));
	    else
	      fprintf(quarantine, ">lcl|%s from %d to %d (%d)(R)\n", toberemoved[index], mstart, mend, (mstart-mend+1));
	    dumpq(mstart, mend, i, index+ugly, log);
	  }
	}
      }
      i=1; j=0; qlen=0;
      if (deleted == delcnt){
	fprintf(log, "Breaking at %d of %d\n", deleted, delcnt);
	break;
      }
      fgets(str, 150, fasta);
      str[strlen(str)-1] = 0;
      if (deletethis(str, delcnt, &mstart, &mend, &index, &match)){
	deleted++;
	// this is the first
	if (mstart<mend)
	  fprintf(quarantine, ">lcl|%s from %d to %d (%d)(F)\n", toberemoved[index], mstart, mend, (mend-mstart+1));
	else
	  fprintf(quarantine, ">lcl|%s from %d to %d (%d)(R)\n", toberemoved[index], mstart, mend, (mstart-mend+1));
	wflag = 0;
      }
      
      else{
	wflag = 1;
      }
      //}
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
  //  }
  fprintf(stderr, "\n");

  if (!wflag){
    dumpq(mstart, mend, i, index, log);
    if (match>1){
      for (ugly=1;ugly<match;ugly++){
	deleted++;
	mstart = start[index+ugly];
	mend   = end[index+ugly];
	// this is the rest
	if (mstart<mend)
	  fprintf(quarantine, ">lcl|%s from %d to %d (%d)(F)\n", toberemoved[index], mstart, mend, (mend-mstart+1));
	else
	  fprintf(quarantine, ">lcl|%s from %d to %d (%d)(R)\n", toberemoved[index], mstart, mend, (mstart-mend+1));
	dumpq(mstart, mend, i, index+ugly, log);
      }
    }
  }
  
  for (i=0;i<delcnt;i++){
    if (alreadyremoved[i]==0)
      fprintf(log, "Not found: %s\n", toberemoved[i]);
  }

  return 1;    
}


int deletethis(char *str, int delcnt, int *s, int *e, int *index, int *match){
  // return only the first match; then interpolate with index+1..index+cnt
  int i;
  int cnt;
  cnt = 0;
  for (i=0;i<delcnt;i++){
    if (strstr(str, toberemoved[i]) && alreadyremoved[i]==0){
      if (cnt == 0){
	*s = start[i];
	*e = end[i];
	*index = i;
      }
      alreadyremoved[i]=1;
      cnt++;
      //return 1;
    }
    else if (cnt != 0){
      *match = cnt;
      return 1;
    }
  }
  
  if (cnt != 0){
    *match = cnt;
    return 1;
  }
  
  return 0;
}


void dumpq(int mstart, int mend, int qlen, int index, FILE *log){
  int i;
  int logged;
  int cnt=0;
  //fprintf(stderr, "dump %d %d; len: %d\n", mstart, mend, qlen);
  logged = 0;
  if (mstart<mend){ //forward
    for (i=mstart;i<=mend;i++){
      if (qvalue[i] < qualthreshold)
	cnt++;
      if (cnt>allowed && !logged){
	fprintf(log, "Faulty alphasat at %s from %d to %d\n", toberemoved[index], start[index], end[index]);
	logged=1;
      }
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
      if (qvalue[i] < qualthreshold)
	cnt++;
      if (cnt>allowed && !logged){
	fprintf(log, "Faulty alphasat at %s from %d to %d\n", toberemoved[index], start[index], end[index]);
	logged=1;
      }
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
  //alreadyremoved[index]=1;
}
