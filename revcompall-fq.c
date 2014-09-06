#include <stdio.h>

#define LEN 100

#define QUAL_OFFSET 33
#define QCUT 20.0

main(int argc, char **argv){
  
  char fname[LEN], fqual[LEN], fseq[LEN]; 
  char rcfqual[LEN], rcfseq[LEN];
  
  char dummy[LEN]; char rdum[LEN];
  
  char ashit[LEN];
  char tshit[LEN];
  char gshit[LEN];
  char cshit[LEN];
  char nshit[LEN];
  

  int isdimer; int isrdimer;
  int dimerlen; int rdimerlen;
  int ncnt; int rncnt;
  int i;
  int slen;

  FILE *in = fopen(argv[1], "r");
  sprintf(dummy, "%s.rc", argv[1]);
  FILE *out=fopen (dummy, "w");
  
  int acnt, ccnt, gcnt, tcnt;
  int racnt, rccnt, rgcnt, rtcnt;
  int hasdot;
  int rhasdot;

  int totqual;
  int rtotqual;

  float qualr, rqualr;
  char reason[100];

  while (fscanf(in, "@%s\n%s\n%s\n%s\n", fname, fseq, dummy, fqual) > 0){ 

    slen = strlen(fseq);

    for (i=0;i<slen;i++){
      if (toupper(fseq[i])=='A') rcfseq[slen-i-1]='T';
      else if (toupper(fseq[i])=='C') rcfseq[slen-i-1]='G';
      else if (toupper(fseq[i])=='G') rcfseq[slen-i-1]='C';
      else if (toupper(fseq[i])=='T') rcfseq[slen-i-1]='A';
      else
	rcfseq[slen-i-1]='N';
      rcfqual[slen-i-1] = fqual[i];

    }
    rcfseq[slen]=0;
    rcfqual[slen]=0;
    

    fprintf(out, "@%s\n%s\n+\n%s\n", fname, rcfseq, rcfqual);
    
  }
  
}
