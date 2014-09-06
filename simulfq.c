#include <stdio.h>
#include <time.h>

#define MAX 250000000

char gen[MAX];
void rcomp(char *, char *);

void insert_mm(char *seq, int error);

int main(int argc, char **argv){
  int len=0;
  char fname[100];
  int nseq=0;
  int i,j;
  FILE *in;
  int rnum;
  char seq[100];
  char rseq[100];
  char ch; char line[100];
  int seqlen;
  char seqname[50];
  int readpos;
  int rc;
  int error;
  
  
  fname[0]=0;

  if (argc==1){
    printf("fastq simulator. Generates a number of reads of given length, introduces mismatches with 1%% probability. Kind of...\n\n");
    printf("\t-i [fasta_file]: Read this fasta file to simulate the reads from.\n");
    printf("\t-l [length]    : Read length.\n");
    printf("\t-e [error]    : Number of errors to insert.\n");    
    printf("\t-n [nseq]      : Number of sequences to generate.\n\n");
    return 0;
  }


  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      error = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      nseq = atoi(argv[i+1]);
  }
    
  in = fopen(fname, "r");

  fscanf(in, ">%s", seqname);
  fgets(line, 100, in);

  i=0;
  while (fscanf(in, "%c", &ch) > 0){
    if (!isspace(ch)) gen[i++]=toupper(ch);
  }
  gen[i]=0;
  seqlen = i;
  
  srand(time(NULL));

  i=0;
  while (i<nseq){
    readpos = rand() % (seqlen - len - 1);
    memcpy(seq, gen+readpos, len);
    seq[len]=0;
    if (strchr(seq, 'N') == NULL){
      rnum = rand() % 100;
      if (rnum == 0){
	insert_mm(seq, error);
      }
      rc = rand() % 2;
      if (rc == 1){
	rcomp(seq, rseq);
      }
      printf("@read_%s_%d_%c\n%s\n+\n", seqname,  (readpos+1), ( (rc==0) ? '+' : '-') ,  (rc==0) ? seq : rseq);
      for (j=0;j<len;j++){
	rnum = rand() % 30 + 10  + 33;
	printf("%c", rnum);
      }
      printf("\n");
      i++;
    }
  }
}


void rcomp(char *seq, char *rseq){
  int i;
  int len = strlen(seq);

  for (i=0;i<len;i++){
    switch (seq[i]){
    case 'A':
      rseq[len-i-1] = 'T';
      break;
    case 'C':
      rseq[len-i-1] = 'G';
      break;
    case 'G':
      rseq[len-i-1] = 'C';
      break;
    case 'T':
      rseq[len-i-1] = 'A';
      break;
    default:
      printf("boo\n"); exit(0);
      break;
    }
  }
  rseq[len]=0;
}

void insert_mm(char *seq, int error){
  int len = strlen (seq);
  int nmm = rand() % error + 1;
  int i;
  int pos;
  int mmchar;
  
  for (i=0;i<nmm;i++){
    pos = rand() % len;
    mmchar = rand() % 4;
    switch (mmchar){
    case 0:
      seq[pos] = 'A';
      break;
    case 1:
      seq[pos] = 'C';
      break;
    case 2:
      seq[pos] = 'G';
      break;
    case 3:
      seq[pos] = 'T';
      break;
    default:
      break;
    }
  } 
}
