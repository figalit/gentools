#include <stdio.h>
#define QCUT 20

int isShit(char [], char [], char [], char []);
int isAllQShit(char [], char [], int);
int isSingleShit(char [], char []);

int QUAL_OFFSET=33;
int shitshown=0;

int main(int argc, char **argv){
  FILE *out;
  char fname[5000];
  char outdir[5000];
  long i,j;
  int batch=0;
  char name[500];
  char seq[500];
  char plus[500];
  char qual[500];
  char name2[500];
  char seq2[500];
  char plus2[500];
  char qual2[500];
  int fasta=0;
  char dummy[500];
  int rmshit=0;
  int singleshit=0;
  int shit=0;
  int allq=0;
  int crop=0;
  int sangerize = 0;
  int shitcnt=0;

  int len1, len2;
  int numreads =  0;

  strcpy(outdir, "./");

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-n"))
      batch = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outdir, argv[i+1]);
    else if (!strcmp(argv[i], "-fasta"))
      fasta = 1;
    else if (!strcmp(argv[i], "-q"))
      QUAL_OFFSET = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-rmshit"))
      rmshit = 1;
    else if (!strcmp(argv[i], "-singleshit"))
      singleshit = 1;
    else if (!strcmp(argv[i], "-allq"))
      allq = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-crop"))
      crop = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-sq")){
      sangerize = 1;
    }
  }
  
  fprintf(stderr, "q=%d\n", QUAL_OFFSET);
  i=0;
  while (!feof(stdin)){
    shit = 0;
    if (scanf("%s", name) <= 0) break;
    fgets(dummy, 500, stdin);
    scanf("%s\n", seq);
    fgets(plus, 500, stdin);
    scanf("%s\n", qual);
    if (rmshit){
      if (scanf("%s", name2) <= 0) break;
      fgets(dummy, 500, stdin);
      scanf("%s\n", seq2);
      fgets(plus2, 500, stdin);
      scanf("%s\n", qual2);

      shit = isShit(seq, qual, seq2, qual2);
      
    }
    
    if (allq!=0){
      shit = isAllQShit(seq, qual, allq);
    }

    if (singleshit){
      shit = isSingleShit(seq, qual);
    }

    if (shit) {shitcnt++; continue;}
    

    //  while(scanf("%s\n%s\n%s\n%s\n", name, seq, plus, qual) > 0){
    if (i%batch == 0){
      if (i!=0) fclose(out);
      sprintf(fname, "%s%ld", outdir, i);
      out = fopen(fname, "w"); 
      fprintf(stderr, "%s\n", fname);
    }

    if (crop != 0 && strlen(seq)>crop){
      seq[crop]=0;
      seq2[crop]=0;
      qual[crop]=0;
      qual2[crop]=0;
    }

    if (sangerize){
      for (j=0;j<strlen(qual);j++){
	qual[j] = qual[j] - 31;
	qual2[j] = qual2[j] - 31;
      }
    }

    len1 = strlen(seq); len2 = strlen(seq2);

    if (len1 < len2) { seq2[len1]=0; qual2[len1] = 0; }
    else if (len1 > len2) { seq[len2]=0; qual[len2] = 0; }

    if (strlen(seq) >= 35){

      if (crop!=0 && strlen(seq)<crop)
	continue;
    
      if (!fasta)
	fprintf(out, "%s\n%s\n+\n%s\n", name, seq, qual);
      else
	fprintf(out, ">%s\n%s\n", (name+1), seq);
      i++; numreads++;
      if (rmshit){
	if (!fasta)
	  fprintf(out, "%s\n%s\n+\n%s\n", name2, seq2, qual2);
	else
	  fprintf(out, ">%s\n%s\n", (name2+1), seq2);
	i++;
	numreads++;
      }
    }

  }
  fprintf (stderr, "Removed pairs: %d\nTotal %d reads\n", shitcnt, numreads);

  return numreads;
}


int isAllQShit(char fseq[], char fqual[], int allq){
  int i;
  for (i=0;i<strlen(fseq);i++){
    if (fqual[i]-QUAL_OFFSET < allq)
      return 1;
  }
  return 0;
}

int isShit(char fseq[], char fqual[], char rseq[], char rqual[]){
  int hasdot;
  int rhasdot;

  int totqual;
  int rtotqual;

  float qualr, rqualr;
  int ncnt; int rncnt;
  
  int acnt, ccnt, gcnt, tcnt;
  int racnt, rccnt, rgcnt, rtcnt;
  int slen;
  int i;

  int maxn;

  
  slen = strlen(fseq);

  maxn = ceil(slen * 0.05);

  if (!shitshown){
    fprintf (stderr,"Slen: %d, MaxN: %d\n", slen, maxn);
    shitshown = 1;
  }

  ncnt=0;rncnt=0;
  acnt=0;   ccnt=0;   gcnt=0;   tcnt=0;
  racnt=0;   rccnt=0;   rgcnt=0;   rtcnt=0;
  hasdot=0; rhasdot=0;
  totqual=0;
  rtotqual=0;

  for (i=0;i<slen;i++){

    if (fseq[i]=='N') ncnt++;
    if (rseq[i]=='N') rncnt++;

    if (fseq[i]=='A') acnt++;
    if (rseq[i]=='A') racnt++;

    if (fseq[i]=='C') ccnt++;
    if (rseq[i]=='C') rccnt++;

    if (fseq[i]=='G') gcnt++;
    if (rseq[i]=='G') rgcnt++;

    if (fseq[i]=='T') tcnt++;
    if (rseq[i]=='T') rtcnt++;

    if (fseq[i]=='.') hasdot=1;
    if (rseq[i]=='.') rhasdot=1;

    if (QUAL_OFFSET == 64){
      if (fqual[i] == 'B')
	totqual += 15;
      else
	totqual += fqual[i]-QUAL_OFFSET;
      if (rqual[i] == 'B')
	rtotqual += 15;
      else
	rtotqual += rqual[i]-QUAL_OFFSET;
    }
    else{
      totqual += fqual[i]-QUAL_OFFSET;
      rtotqual += rqual[i]-QUAL_OFFSET;      
    }

  }


  qualr = (float)totqual / (float)slen;
  rqualr = (float)rtotqual / (float)slen;

  //  if (ncnt> maxn || rncnt > maxn || hasdot==1 || rhasdot==1){
  if (ncnt> maxn || rncnt > maxn || hasdot==1 || rhasdot==1 || qualr < QCUT || rqualr < QCUT){

    return 1;
  }

  /* repetitive crap */
  else if (acnt>5*slen/6 || racnt > 5*slen/6){
    return 1;
  }

  else if (ccnt>5*slen/6 || rccnt > 5*slen/6){
    return 1;
    /*
    fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
    continue;*/
  }

  else if (gcnt>5*slen/6 || rgcnt > 5*slen/6){
    return 1;
    /*
    fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
    continue;*/
    }
  else if (tcnt>5*slen/6 || rtcnt > 5*slen/6){
    return 1;
    /*
    fprintf(log, "%s\n%s\n+\n%s\n%s\n%s\n%+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
    continue;*/
    }

  return 0;


}

int isSingleShit(char fseq[], char fqual[]){
  int hasdot;
  int rhasdot;

  int totqual;
  int rtotqual;

  float qualr, rqualr;
  int ncnt; int rncnt;
  
  int acnt, ccnt, gcnt, tcnt;
  int racnt, rccnt, rgcnt, rtcnt;
  int slen;
  int i;
  
  slen = strlen(fseq);

  ncnt=0;rncnt=0;
  acnt=0;   ccnt=0;   gcnt=0;   tcnt=0;
  racnt=0;   rccnt=0;   rgcnt=0;   rtcnt=0;
  hasdot=0; rhasdot=0;
  totqual=0;
  rtotqual=0;

  for (i=0;i<slen;i++){

    if (fseq[i]=='N') ncnt++;

    if (fseq[i]=='A') acnt++;

    if (fseq[i]=='C') ccnt++;

    if (fseq[i]=='G') gcnt++;

    if (fseq[i]=='T') tcnt++;

    if (fseq[i]=='.') hasdot=1;

    totqual += fqual[i]-QUAL_OFFSET;

  }


  qualr = (float)totqual / (float)slen;

  if (ncnt>2 || hasdot==1 || qualr < QCUT){

    return 1;
  }


  return 0;

  /*  else if (acnt>2*slen/3 || racnt > 2*slen/3){
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
    }*/



}
