/*
  Given a SORTED (-k 1,1) blastparser output,
  merges rows that have the same first column
  Last update : March 15, 2007
 */

#include <stdio.h>

int main(int argc, char **argv){
  FILE *in;
  FILE *out;
  int i;
  char infile[100];
  char outfile[100];
  int maxcolumn;
  int thiscnt;

  char QNAME[300], QB[300], QE[300], QLEN[300], SNAME[300], SB[300], SE[300], SLEN[300], FRACBPIDENT[300], BPALIGN[300], SIZEALIGN[300], SCORE[300],QDEFN[300],SDEFN[300];
  char prevname[300];
  char ch;
  int qlen;
  int contignum;
  char *connum;
  char SNAME2[300];
  int contiglen;
  int sb, se;
  int nlineput;

  if (argc != 5){
    fprintf(stderr, "%s -i [bo.parse] -o [bo.merge]\n", argv[0]);
    return 0;
  }

  infile[0] = 0;
  outfile[0] = 0;

  for (i=1; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
  }
  
  if (infile[0] == 0 || outfile[0] == 0){
    fprintf(stderr, "In/out file names?\n");
    return 0;
  }

  in = fopen(infile, "r");

  if (in==NULL){
    fprintf(stderr, "Infile not found.\n");
    return 0;
  }

  out = fopen(outfile, "w");

  if (out==NULL){
    fprintf(stderr, "Can not write outfile.\n");
    return 0;
  }

  maxcolumn = (255-15)/13;
  fprintf(out, "QNAME\tQB\tQE\tQLEN\tQQUAL\tSNAME\tSB\tSE\tSLEN\tFRACBPIDENT\tBPALIGN\tSIZEALIGN\tSCORE\tQDEFN\tSDEFN\t");
  for (i=0;i<(255-15)/12;i++)
    fprintf(out, "QB\tQE\tSNAME\tSB\tSE\tSLEN\tFRACBPIDENT\tBPALIGN\tSIZEALIGN\tSCORE\tQDEFN\tSDEFN\t");
  fprintf(out,"\n");
  
  prevname[0]=0;
  thiscnt = 1;
  nlineput = 0;

  //char QNAME[300], QB[300], QE[300], QLEN[300], SNAME[300], SB[300], SE[300], SLEN[300], FRACBPIDENT[300], BPALIGN[300], SIZEALIGN[300], SCORE[300],QDEFN[300],SDEFN[300];
  while(fscanf(in, "%s%s%s%s%s%s%s%s%s%s%s%s", QNAME, QB, QE, QLEN, SNAME, SB, SE, SLEN, FRACBPIDENT, BPALIGN, SIZEALIGN, SCORE) > 0){
    ch=0;
    while(ch != '\t')
      fscanf(in, "%c", &ch);
    i=0; ch=0;
    while(ch != '\t' && ch!='\n' && ch!='\r'){
      fscanf(in, "%c", &ch);
      if (ch!='\t' && ch!='\n' && ch!='\r') QDEFN[i++] = ch;
    }
    QDEFN[i]=0;
    fgets(SDEFN, 300, in);
    i=strlen(SDEFN)-1;
    while (isspace(SDEFN[i]) && i>=0) i--;
    if (i<0) i=0;
    SDEFN[i]=0;
    if (!strcmp(QNAME, "QNAME")) // header
      continue;

    if (SDEFN[0] == 0 && strstr(SNAME, "_")){
      strcpy(SNAME2, SNAME);
      connum = (char *) strtok(SNAME2, "_");
      connum = (char *) strtok(NULL, "_");
      contignum = atoi(connum);
      contiglen = atoi(SLEN);
      sb = atoi(SB);
      se = atoi(SE);
      sb = sb + contignum*contiglen;
      se = se + contignum*contiglen;
      sprintf(SDEFN, "%s:%d-%d", SNAME2, sb, se);
    }

    if (!strcmp(prevname, QNAME) && thiscnt<=maxcolumn){
      thiscnt++; 
      fprintf(out, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t", QB, QE, SNAME, SB, SE, SLEN, FRACBPIDENT, BPALIGN, SIZEALIGN, SCORE, QDEFN, SDEFN);
    }
    else if (!strcmp(prevname, QNAME) && thiscnt>maxcolumn && !nlineput){
      fprintf(out, "\n");
      nlineput = 1;
    }
    else if(strcmp(prevname, QNAME)){
      thiscnt = 1; // right after first QLEN there's an empty  column for QQUAL
      if (!nlineput)
	fprintf(out, "\n");
      fprintf(out, "%s\t%s\t%s\t%s\t\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t", QNAME, QB, QE, QLEN, SNAME, SB, SE, SLEN, FRACBPIDENT, BPALIGN, SIZEALIGN, SCORE, QDEFN, SDEFN);
      strcpy(prevname, QNAME);
      nlineput = 0;
    }
  }

  fprintf(out, "\n");
  fclose(in);
  fclose(out);
  return 1;
}
