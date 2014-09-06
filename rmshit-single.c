#include <stdio.h>

#define LEN 100

#define QUAL_OFFSET 33

main(int argc, char **argv){
  
  char fname[LEN], rname[LEN], fseq[LEN], rseq[LEN], fqual[LEN], rqual[LEN];
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
  sprintf(dummy, "%s.log", argv[1]);
  FILE *log=fopen (dummy, "w");
  sprintf(dummy, "%s.clean", argv[1]);
  FILE *out=fopen (dummy, "w");
  
  int acnt, ccnt, gcnt, tcnt;
  int racnt, rccnt, rgcnt, rtcnt;
  int hasdot;
  int rhasdot;

  int totqual;
  int rtotqual;

  float qualr, rqualr;

  while (fscanf(in, "%s\n%s\n%s\n%s\n", fname, fseq, dummy, fqual) > 0){
    //while (fscanf(in, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", fname, fseq, dummy, fqual, rname, rseq, rdum, rqual) > 0){

    slen = strlen(fseq);

    ncnt=0;rncnt=0;
    acnt=0;   ccnt=0;   gcnt=0;   tcnt=0; 
    racnt=0;   rccnt=0;   rgcnt=0;   rtcnt=0; 
    hasdot=0; rhasdot=0;
    totqual=0;
    rtotqual=0;

    for (i=0;i<slen;i++){
      ashit[i]='A';
      cshit[i]='C';
      gshit[i]='G';
      tshit[i]='T';
      
      if (fseq[i]=='N') ncnt++;
      //      if (rseq[i]=='N') rncnt++;

      if (fseq[i]=='A') acnt++;
      //      if (rseq[i]=='A') racnt++;

      if (fseq[i]=='C') ccnt++;
      //      if (rseq[i]=='C') rccnt++;

      if (fseq[i]=='G') gcnt++;
      //      if (rseq[i]=='G') rgcnt++;

      if (fseq[i]=='T') tcnt++;
      //      if (rseq[i]=='T') rtcnt++;

      if (fseq[i]=='.') hasdot=1;
      //      if (rseq[i]=='.') rhasdot=1;

      totqual += fqual[i]-QUAL_OFFSET;
      //      rtotqual += rqual[i]-QUAL_OFFSET;
      
    }
  
    
    qualr = (float)totqual / (float)slen;
    //rqualr = (float)rtotqual / (float)slen;
    
    ashit[i]=0;
    cshit[i]=0;
    gshit[i]=0;
    tshit[i]=0;
    

    if (ncnt>2 || hasdot==1 ){// || qualr < 10 || rqualr < 10){
      fprintf(log, "%s\n%s\n+\n%s\n", fname, fseq, fqual);
      continue;
    }

    /*
    else if (acnt>2*slen/3 || racnt > 2*slen/3){
      fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
      continue;
    }

    else if (ccnt>2*slen/3 || rccnt > 2*slen/3){
      fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
      continue;
    }

    else if (gcnt>2*slen/3 || rgcnt > 2*slen/3){
      fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
      continue;
    }

    else if (tcnt>2*slen/3 || rtcnt > 2*slen/3){
      fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
      continue;
      }*/

    
    /*
    else if (!strcmp(fseq, ashit) || !strcmp(rseq, ashit)) {      fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual); continue;}
    else if (!strcmp(fseq, cshit) || !strcmp(rseq, cshit)) {       fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual); continue; }
    else if (!strcmp(fseq, gshit) || !strcmp(rseq, gshit)) {       fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual); continue; }
    else if (!strcmp(fseq, tshit) || !strcmp(rseq, tshit)) {       fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);continue; }

    isdimer = 1;
    isrdimer = 1;
    
    dimerlen=0; rdimerlen=0;
    
    for (i=0;i<slen-2;i++){
      if (fseq[i]!=fseq[i+2]) isdimer = 0; 
      else dimerlen++;
      if (rseq[i]!=rseq[i+2]) isrdimer = 0; else rdimerlen++;
    }

    if (isdimer || isrdimer){// || dimerlen > 6 || rdimerlen > 6) {
       fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual); 
       continue; 
    }
    */

    fprintf(out, "%s\n%s\n+\n%s\n", fname, fseq, fqual);
    
  }
  
}
