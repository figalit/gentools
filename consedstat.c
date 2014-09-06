#include <stdio.h>
int main(int argc, char **argv){
  char *consensus;
  int nseq;
  char seqname[100];
  char fname[100];
  char seq[5000];
  char **allseqs;
  int i;
  int len;
  int start, end;
  FILE *info;
  char dummy[100];
  int slen;
  char ch;
  FILE *cfile;
  int j;
  int mmatch;
  
  info = fopen (argv[1],"r");
  cfile = fopen (argv[2],"r");
  
  for (i=0;i<8;i++) fscanf(info, "%s", dummy);
  
  fscanf(info, "%d", &len);
  printf("len: %d\n", len);

  consensus = (char *)malloc(sizeof(char)*len+1);

  for (i=0;i<3;i++) fscanf(info, "%s", dummy);

  fscanf(info, "%d", &nseq);
  printf("nseq: %d\n", nseq);
  
  allseqs = (char **)malloc(sizeof(char *)*nseq);
  for (i=0;i<nseq;i++){
    allseqs[i] = (char *)malloc(sizeof(char)*len+1);
  }

  for(i=0;i<nseq;i++){
    for(j=0;j<len;j++){
      allseqs[i][j] = '-';
    }
    allseqs[i][j] = 0;
  }

  i=0;
  while(fscanf(cfile, "%c", &ch) > 0){
    ch = toupper(ch);
    if (ch=='>')
      fgets(dummy, 100, cfile);
    else if (ch=='A' ||ch=='C' ||ch=='G' ||ch=='T' || ch=='N')
      consensus[i++] = ch;
  }
  consensus[i] = 0;
  printf("consensus: %s\n", consensus);
  fclose(cfile);
  
  j=0;
  while(fscanf(info, "%s%s%d%d%d", seqname, dummy, &slen, &start, &end) > 0){
    sprintf(fname, "%s.seq", seqname);
    cfile = fopen(fname, "r");
    printf("reading %s %d-%d\n", seqname, start, end);
    i=start-1;
    while(fscanf(cfile, "%c", &ch) > 0){
      ch = toupper(ch);
      if (ch=='>')
	fgets(dummy, 100, cfile);
      else if (ch=='A' ||ch=='C' ||ch=='G' ||ch=='T' || ch=='N')
	allseqs[j][i++] = ch;
    }
    j++;
    fclose(cfile);
  }
  
  mmatch = 0;
  for(i=0;i<len;i++){
    ch = consensus[i]; 
    for(j=0;j<nseq;j++){
      
      if (allseqs[j][i] != '-' && allseqs[j][i] != ch){
	printf("discrepency char %c, seq %d pos %d\n", ch, j, i);
	mmatch++;
	break;
      }
    }
  }

  printf("len: %d\tmmatch: %d\n", len, mmatch);
}
