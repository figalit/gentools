#include <stdio.h>

#define QCUT 20
#define QOFF 33

#define MAX 250000000
char *reparr;

int main(int argc, char **argv){

  char clone[100];
  char fwdname[100];
  char fchr[30];
  char rchr[30];
  char tchr[30];
  int fs, fe, fd;
  char fo;
  char revname[100];
  int rs, re, rd;
  char ro;
  char call[100];
  char fsnpt1, fsnpt2;
  char fsnp1[5], fsnp2[5];
  char fsq1[5], fsq2[5];
  int fsnpp1, fsnpp2;
  char rsnpt1, rsnpt2;
  char rsnp1[5], rsnp2[5];
  char rsq1[5], rsq2[5];
  int rsnpp1, rsnpp2;
  int td, span;
  int ts, te;
  char q1[100];
  char q2[100];

  int qint1, qint2;
  int qpass;
  float qf1, qf2;

  int i;
  char fname[100];
  char repfile[100];
  FILE *inf; FILE *rep;
  int isrepeat;
  int risrepeat;
  char mychr[30];
  double perror;

  repfile[0]=0;

  for (i=0;i<argc;i++){

    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-r"))
      strcpy(repfile, argv[i+1]);
    else if (!strcmp(argv[i], "-c"))
      strcpy(mychr, argv[i+1]);    
  }
  
  inf = fopen(fname, "r");

  if (repfile[0]!=0){
    if (mychr[0]==0) { fprintf(stderr, "I need chr name\n"); exit(0); }
    rep = fopen(repfile, "r");
    reparr = (char *) malloc(sizeof(char) * MAX);
    memset (reparr, 0, sizeof(char)*MAX);
    while (fscanf(rep, "%s\t%d\t%d\n", fchr, &fs, &fe) > 0){
      if (strcmp(fchr, mychr)) continue;
      for (i=fs;i<=fe;i++) reparr[i]=1;
    }
    fclose(rep);
  }

  while (fscanf(inf, "%s\t%s\t%s\t%d\t%d\t%d\t%c\t%s\t%s\t%d\t%d\t%d\t%c\t%d\t%d\t%s\t%s\t%d\t%d\t", clone, fwdname, fchr, &fs, &fe, &fd, &fo, revname, rchr, &rs, &re, &rd, &ro, &span, &td, call, tchr, &ts, &te) > 0){

    if (fd>0)
      fscanf(inf, "%c\t%d\t%s\t%s\t", &fsnpt1, &fsnpp1, fsnp1, fsq1);
    if (fd==2)
      fscanf(inf, "%c\t%d\t%s\t%s\t", &fsnpt2, &fsnpp2, fsnp2, fsq2);

    fscanf(inf, "%s\t", q1);

    if (rd>0)
      fscanf(inf, "%c\t%d\t%s\t%s\t", &rsnpt1, &rsnpp1, rsnp1, rsq1);
    if (rd==2)
      fscanf(inf, "%c\t%d\t%s\t%s\t", &rsnpt2, &rsnpp2, rsnp2, rsq2);

    fscanf(inf, "%s\n", q2);

    qint1=0; qint2=0;

    for (i=0;i<strlen(q1);i++){
      qint1 += q1[i] - QOFF;
      qint2 += q2[i] - QOFF;
    }


    qf1 = (float)qint1 / (float)strlen(q1);
    qf2 = (float)qint2 / (float)strlen(q2);

    qpass = 0;
    if (qf1>=QCUT || qf2>=QCUT || te==0)
      qpass=1;
    else{
      perror = 1;
      if (fd>0){
	if (fsnpt1 != 'D')
	  perror = perror * pow(10, ((fsq1[0]-QOFF) / -10));
      }
      if (fd==2){
	if (fsnpt2 != 'D')
	  perror = perror * pow(10, ((fsq2[0]-QOFF) / -10));
      }

      if (rd>0){
	if (rsnpt1 != 'D')
	  perror = perror * pow(10, ((rsq1[0]-QOFF) / -10));
      }
      if (fd==2){
	if (rsnpt2 != 'D')
	  perror = perror * pow(10, ((rsq2[0]-QOFF) / -10));
      }
      
      if (perror > 0.15) qpass=1;
    }


    isrepeat = 0;
    risrepeat = 0;

    //qpass=1;
    /* check repeat table here */
    if (repfile[0] != 0){

      isrepeat = 1;
      risrepeat = 1;
      
      for (i=fs;i<=fe;i++)
	if (reparr[i]==0) isrepeat=0;
      for (i=rs;i<=re;i++)
	if (reparr[i]==0) risrepeat=0;
      
    }


    if (qpass && !isrepeat && !risrepeat){

      fprintf(stdout, "%s\t%s\t%s\t%d\t%d\t%d\t%c\t%s\t%s\t%d\t%d\t%d\t%c\t%d\t%d\t%s\t%s\t%d\t%d\t", clone, fwdname, fchr, fs, fe, fd, fo, revname, rchr, rs, re, rd, ro, span, td, call, tchr, ts, te);

      if (fd>0)
	fprintf(stdout, "%c\t%d\t%s\t%s\t", fsnpt1, fsnpp1, fsnp1, fsq1);
      if (fd==2)
	fprintf(stdout, "%c\t%d\t%s\t%s\t", fsnpt2, fsnpp2, fsnp2, fsq2);
      
      fprintf(stdout, "%s\t", q1);
      
      if (rd>0)
	fprintf(stdout, "%c\t%d\t%s\t%s\t", rsnpt1, rsnpp1, rsnp1, rsq1);
      if (rd==2)
	fprintf(stdout, "%c\t%d\t%s\t%s\t", rsnpt2, rsnpp2, rsnp2, rsq2);
      
      fprintf(stdout, "%s\n", q2);

      

    }


    //fprintf(stdout, "%s\t%d\t%d\t%d\t%c\n", clone, fd, rd, te, fsnpt1);
    //getchar();
  }

  

}
