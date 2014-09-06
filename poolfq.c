#include <stdio.h>

#define LEN 200

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
  int i,j;
  int slen;

  FILE *in = fopen(argv[1], "r");
  FILE *pf = fopen(argv[2], "r");
  FILE *unpooled;
  
  int acnt, ccnt, gcnt, tcnt;
  int racnt, rccnt, rgcnt, rtcnt;
  int hasdot;
  int rhasdot;

  int totqual;
  int rtotqual;

  float qualr, rqualr;

  char **pools;
  FILE **fpools;
  
  char pchar[50];
  int pid;
  char filename[100];

  int poolcnt;
  int barlen;

  char unpooledfname[100];
  int pooled;

  sprintf(unpooledfname, "%s.unpooled", argv[1]);
  unpooled = fopen(unpooledfname, "w");

  poolcnt=0;

  while (fscanf(pf, "%d %s", &pid, pchar) > 0){
    poolcnt++;
  }

  fprintf (stderr, "%d pools.\n", poolcnt);
  rewind(pf);

  pools = (char **) malloc(sizeof(char *) * poolcnt);
  fpools = (FILE **) malloc(sizeof(FILE *) * poolcnt);

  for (i=0;i<poolcnt;i++){
    pools[i] = (char *) malloc(sizeof(char) * 50);
    sprintf(filename, "%s.pool.%d", argv[1], (i+1));
    fpools[i] = fopen(filename, "w");
  }

  i=0;
  while (fscanf(pf, "%d %s", &pid, pchar) > 0){
    for (j=0;j<strlen(pchar);j++)
      pchar[j] = toupper(pchar[j]);
    strcpy(pools[i++], pchar);
  }

  barlen = strlen(pools[0]);
  fprintf(stderr, "barcode_length: %d\n", barlen);

  for (i=0;i<poolcnt;i++){
    printf("%d\t%s\n", i+1, pools[i]);
  }

  while (fscanf(in, "%s\n%s\n%s\n%s\n", fname, fseq, dummy, fqual) > 0){

    /*
    memcpy(pchar, fseq, barlen*sizeof(char));
    pchar[barlen]=0;
    */
    pooled=0;
    for (i=0;i<poolcnt;i++){
      //if (!strcmp(pools[i], pchar)){
      if (!memcmp(pools[i], fseq, barlen*sizeof(char))){
	//fprintf(stdout, "code: %s, read: %s writing to %d\n", pools[i], fseq, i+1);
	//getchar();
	fprintf(fpools[i], "%s\n%s\n%s\n%s\n", fname, (fseq), dummy, fqual);
	//fprintf(fpools[i], "%s\n%s\n%s\n%s\n", fname, (fseq+barlen), dummy, (fqual+barlen));
	pooled=1;
	break;
      }
    }
    if (!pooled)
      fprintf(unpooled, "%s\n%s\n%s\n%s\n", fname, (fseq), dummy, fqual);
  }
  
}
