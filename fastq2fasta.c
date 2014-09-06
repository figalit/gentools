#include <stdio.h>
#include <zlib.h>

#define MAXLEN 20000

main(int argc, char **argv){
  char name[MAXLEN];
  char seq[MAXLEN];
  char qual[MAXLEN];
  char plus[MAXLEN];
  int i;
  FILE *qualf;
  FILE *fasta; FILE *in;
  FILE *fp;
  char fname[MAXLEN];
  char fastaname[MAXLEN];
  char qname[MAXLEN];
  int GZ=0;
  int qualout=1;
  int ret;
  char line[MAXLEN];
  int seqlen = 0;
  long quals[MAXLEN];
  long basequals[MAXLEN];
  int seqcnt=0;
  long a,c,g,t,n;
  long a2,c2,g2,t2,n2;
  int orient=0;
  char suffix[4];
  int statout=0;
  int standardout=0;
  int standardin;
  int writeout=1;
  int gcstatout = 0;
  int at[MAXLEN];
  int aa[MAXLEN];
  int ac[MAXLEN];
  int ag[MAXLEN];
  int an[MAXLEN];

  float gcratio;

  int QOFFSET=33;

  memset(aa, 0, sizeof(int)*MAXLEN);
  memset(ac, 0, sizeof(int)*MAXLEN);
  memset(ag, 0, sizeof(int)*MAXLEN);
  memset(at, 0, sizeof(int)*MAXLEN);
  memset(an, 0, sizeof(int)*MAXLEN);


  a=0; c=0;g=0;t=0;n=0;
  suffix[0]=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-gz"))
      GZ=1;
    else if (!strcmp(argv[i], "-nq"))
      qualout=0;
    else if (!strcmp(argv[i], "-stat"))
      statout=1;
    else if (!strcmp(argv[i], "-gcstat"))
      gcstatout=1;
    else if (!strcmp(argv[i], "-suf"))
      sprintf(suffix, "/%s", argv[i+1]);
    else if (!strcmp(argv[i], "-stdout"))
      standardout = 1;
    else if (!strcmp(argv[i], "-nowrite"))
      writeout = 0;
    else if (!strcmp(argv[i], "-qo"))
      QOFFSET = atoi(argv[i+1]);
  }

  memset(quals, 0, sizeof(long)*MAXLEN);
  memset(basequals, 0, sizeof(long)*MAXLEN);
  
  if (!writeout) qualout = 0;

  if (!GZ){
    if (!strcmp(fname, "stdin"))
      in = stdin;
    else
      in = fopen(fname,"r");
  }
  else
    in = gzopen(fname,"r");
  
  if (writeout){
    sprintf(fastaname,"%s.fa", fname);
    if (!standardout)
      fasta = fopen(fastaname, "w");
    else
      fasta = stdout; 
  }
  
  if (qualout && writeout){
    sprintf(qname,"%s.qual", fname);
    qualf = fopen(qname, "w");
  }


  if (!GZ){
    //    while (fscanf(in, "%s%s%s%s", name, seq, plus, qual) > 0){
    while (1){
      if (feof(in)) break;
      fgets(name, MAXLEN, in);
      if (feof(in)) break;
      fgets(seq, MAXLEN, in);
      fgets(plus, MAXLEN, in);
      fgets(qual, MAXLEN, in);
      name[strlen(name)-1] = 0;
      seq[strlen(seq)-1] = 0;
      plus[strlen(plus)-1] = 0;
      qual[strlen(qual)-1] = 0;

      if (seqlen == 0)
	seqlen = strlen(seq);
      //      fgets(line, MAXLEN, in);
      if (writeout)
	fprintf(fasta,">%s%s\n", (name+1), suffix);
      seqcnt++;
      if (qualout && writeout)
	fprintf(qualf,">%s%s\n", (name+1), suffix);
      if (writeout)
	fprintf(fasta,"%s\n", seq);
      for (i=0;i<strlen(qual);i++){
	switch(toupper(seq[i])){
	case 'A':
	  a++;
	  break;
	case 'C':
	  c++;
	  break;
	case 'G':
	  g++;
	  break;
	case 'T':
	  t++;
	  break;
	default:
	  n++;
	  break;
	}
	if (qualout)
	  fprintf(qualf, "%d ", qual[i]-QOFFSET);
	quals[qual[i]-QOFFSET]++;
	basequals[i+1]+=qual[i]-QOFFSET;
      }
      if (qualout)
	fprintf(qualf, "\n");
    }
    
  }
  
  else{
    while (!gzeof(in)){
      gzgets(in, name, MAXLEN);
      name[strlen(name)-1]=0;
      gzgets(in, seq, MAXLEN);
      seq[strlen(seq)-1]=0;
      if (seqlen == 0)
	seqlen = strlen(seq);
      gzgets(in, plus, MAXLEN);
      gzgets(in, qual, MAXLEN);
      qual[strlen(qual)-1]=0;

      a2=0; c2=0; g2=0; t2=0; n2=0;
      //printf ("seqlen : %d\t %s\t%s\t%s\t%s\n", seqlen, name, seq, plus, qual);


      if (writeout)
	fprintf(fasta,">%s%s\n", (name+1), suffix);
      seqcnt++;
      if (qualout)
	fprintf(qualf,">%s%s\n", (name+1), suffix);
      if (writeout)
	fprintf(fasta,"%s\n", seq);
      
      for (i=0;i<strlen(qual);i++){
	switch(toupper(seq[i])){
	case 'A':
	  a++; aa[i]++; a2++;
	  break;
	case 'C':
	  c++; ac[i]++; c2++;
	  break;
	case 'G':
	  g++; ag[i]++; g2++;
	  break;
	case 'T':
	  t++; at[i]++; t2++;
	  break;
	default:
	  n++; an[i]++; n2++;
	  break;
	}
	if (qualout)
	  fprintf(qualf, "%d ", qual[i]-QOFFSET);
	quals[qual[i]-QOFFSET]++;
	basequals[i+1]+=qual[i]-QOFFSET;
      }
      if (qualout)
	fprintf(qualf, "\n");      

      gcratio = 100.0 * ( (float)(g2+c2) / (float)(a2+c2+g2+t2+n2) );

      if (gcstatout)
	printf("%d\t%d\t%d\t%d\t%d\t%f\n", a2, c2, g2, t2, n2, gcratio);

    }    
  }

  if (writeout)
    fclose(fasta);
  
  if (statout){

    fprintf(stdout, "Total #seqs:\t%d\n", seqcnt);
    fprintf(stdout, "Sequence length:\t%d\n", seqlen);
    fprintf(stdout, "A: %d\tC: %d\tG: %d\tT: %d\tN: %d\n", a,c,g,t,n);
    
    sprintf(fastaname,"%s.qualcnt", fname);
    fasta = fopen(fastaname, "w");
    for (i=0;i<MAXLEN;i++){
      if (quals[i]!=0){
	fprintf(fasta, "%d\t%ld\n", i, quals[i]);
      }
    }
    fclose(fasta);
    
    sprintf(fastaname,"%s.basequal", fname);
    fasta = fopen(fastaname, "w");
    for (i=1;i<MAXLEN;i++){
      if (basequals[i]!=0){
	fprintf(fasta, "%d\t%f\n", i, ( (double)(basequals[i]) / (double)(seqcnt)));
      }
    }
    fclose(fasta);

    /*
  fp = fopen("A.cnt", "w");
  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)a[i] / nseq));
  }
  fclose(fp);

  fp = fopen("C.cnt", "w");
  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)c[i] / nseq));
  }
  fclose(fp);

  fp = fopen("G.cnt", "w");
  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)g[i] / nseq));
  }
  fclose(fp);

  fp = fopen("T.cnt", "w");
  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)t[i] / nseq));
  }
  fclose(fp);

  fp = fopen("N.cnt", "w");
  for (i=0;i<strlen(seq);i++){
    fprintf(fp,"%d\t%f\n", i+1, ((float)n[i] / nseq));
  }
  fclose(fp);

    */


  }
}






