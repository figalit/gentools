#include <stdio.h>
#define SEQ_LENGTH 150

char **names;
char **seqs;
int readSingleFasta(FILE *fastaFile);


int main(int argc, char **argv){

  FILE *fasta;
  FILE *masked;
  char fname[1000];
  int nseq;
  int i,j;

  if (argc != 2){
    fprintf(stderr, "CG masking.\n");
    fprintf(stderr, "Usage: %s [fastafile]\n", argv[0]);
    return 0;
  }

  fasta = fopen(argv[1], "r");
  if (fasta == NULL){
    fprintf(stderr, "Unable to open file %s\n", argv[1]);
    return 0;
  }

  nseq = readSingleFasta(fasta);
  sprintf(fname, "%s.cgmasked", argv[1]);
  masked = fopen(fname, "w");
  for (i=0;i<nseq;i++){
    fprintf(masked, ">%s", names[i]);
    fprintf(stderr, "\rMasking Sequence \t%d\tof\t%d", (i+1), nseq);
    for (j=0;j<strlen(seqs[i])-1;j++){
      if (j%60==0)
	fprintf(masked, "\n");
      if (seqs[i][j] == 'C' && seqs[i][j+1]=='G')
	fprintf(masked, "N");
      else
	fprintf(masked, "%c", seqs[i][j]);	
    }
    fprintf(masked, "%c", seqs[i][strlen(seqs[i])-1]);	
    fprintf(masked, "\n");
  }
  
  fclose(masked); fclose(fasta);
  fprintf(stderr, "\nMasked file is: %s\n", fname);
  return 1;
}


int readSingleFasta(FILE *fastaFile){
  int cnt;
  char ch; 
  int i,j;
  int index;
  int seqcnt=0, seqlen=0;
  int maxlen;
  char dummy[SEQ_LENGTH];
  char str[SEQ_LENGTH];

  cnt = 0; i=0;
  fprintf(stderr, "Counting sequences.\n");
  maxlen=0;
  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      if (seqlen>maxlen)
	maxlen=seqlen;
      cnt++;
      seqlen=0;
      fgets(dummy, SEQ_LENGTH, fastaFile);
    }
    else if (!isspace(ch)){
      seqlen++;
      if (seqlen>maxlen)
	maxlen=seqlen;
    }
  }

  seqcnt = cnt;

  //length = (int *) malloc((seqcnt) * sizeof(int));


  cnt = 0; i=0;

  fprintf(stderr, "Allocating memory for %d sequences with max length %d.\n", seqcnt, maxlen);
  
  
  seqs = (char **) malloc((seqcnt) * sizeof(char *));
  
  for (i=0; i<seqcnt; i++)
    seqs[i] = (char *) malloc(maxlen);
  

  names = (char **) malloc((seqcnt) * sizeof(char *));

  for (i=0; i<seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);
  
  
  
  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
  }
  

  fprintf(stderr, "Reading sequences.\n");
  rewind(fastaFile);
  cnt = -1; 
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", (cnt+1), seqcnt );
      fgets(names[cnt], SEQ_LENGTH, fastaFile);
      names[cnt][strlen(names[cnt])-1] = 0;
    }
    i = 0;
    if (cnt != 0){
      seqs[cnt][i++] = ch;
    }
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n'){
	seqs[cnt][i++] = ch;
      }
    } while (ch != '>');

    seqs[cnt][i] = 0;
    
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", (cnt+1), seqcnt );
      if (cnt != seqcnt){
	  fgets(names[cnt], SEQ_LENGTH, fastaFile);
	  names[cnt][strlen(names[cnt])-1] = 0;
      }
    } // if
  } // while
	    

  fprintf(stderr, "\n[OK] %d sequences read from fasta file.\n",seqcnt);


  return seqcnt;

}


