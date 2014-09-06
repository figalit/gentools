/*

Given set of sequences in a fasta file, and a center star sequence
align eveyone to the center and dump the alignments

Marc 23, 2006
    
*/


#include "pimp.h"


char **hornames;
char **horseqs;
char **names;
char **seqs;
int  *clustered;


void readFasta(FILE *, FILE *, int *, int *);

int main(int argc, char **argv){
  char str[1000];
  FILE *fasta;
  FILE *hor;
  FILE *log;
  FILE *aligned;
  int i,j;
  int count;
  int nseq, nhor;
  int *freqs;
  int swapfreq;
  int mismatch;
  int trim;
  /* pattern match stuff */

  FILE *clusterFile;
  char cfbase[100];
  struct cluster *allclusters;
  struct cluster *prevcluster;
  struct cluster *current;
  struct cmember *cm;
  struct wgsread *allreads;
  struct wgsread *wgs1, *wgs2;
  struct asatseq *asat;
  struct asatseq *patternasat;
  char *wgsname;
  int wgsstart, wgsend;
  int iflag;
  int isInserted;
  int cluster_id;
  int pattern_id;
  int occurance;
  int isFINALHOR;

  /* pattern match stuff ends here */

  if (argc != 3 && argc!=4){
    fprintf(stderr, "Aligns sequences to the given center star given a two fasta-centroid files.\nThen dumps the alignments\n");
    fprintf(stderr, "%s [fasta file][hor fasta] <-trim>\n", argv[0]);
    exit(0);
  }
  
  trim = 0;
  fasta = fopen(argv[1], "r");
  hor = fopen(argv[2], "r");
  if (argc == 4 && !strcmp(argv[3], "-trim"))
    trim = 1;
  if (fasta == NULL || hor == NULL)
    return 0;


  strcpy(str, argv[1]);
  for (i=strlen(str)-1;i>=0;i--)
    if (str[i]=='.'){ 
      str[i] = 0;
      break;
    }

  if (trim)
    fprintf(stdout, "Will trim.\n");
  
  makevectors();  

  strcat(str, ".log");
  log = fopen(str, "w");
  sprintf(str, "center-%s", argv[1]);
  aligned = fopen(str, "w");

  readFasta(fasta, hor, &nseq, &nhor);
  fprintf(stderr, "%d monomers, and the center are read into memory.\n", nseq);

  for (i=0;i<nseq;i++){
    align(seqs[i], horseqs[0]);
    /* aligned seq is in Sp, aligned center is in Tp */
    fprintf(aligned, ">%s\n", names[i]);
    fprintf(log, "%s\t%s\n%s\t%s\n", names[i], Sp, hornames[0], Tp);
    for (j=strlen(Sp)-1;j>=0;j--){
      if (!trim)
	fprintf(aligned, "%c", Sp[j]);
      else if (Tp[j]!='-')
 	fprintf(aligned, "%c", Sp[j]);
     if ((strlen(Sp)-j)%60 == 0 && j!=strlen(Sp)-1)
	fprintf(aligned, "\n");
    }
    fprintf(aligned, "\n");    
    if (strchr(Tp, '-')!=NULL)
      fprintf(log, "%s\n", names[i]);
    fprintf(stderr, "\r%d\tof\t%d", (i+1), nseq);
  }
  fprintf(stderr, "\n");    
  fclose(log);
  fclose(aligned);
  return 1;
}



void readFasta(FILE *fastaFile, FILE *horFile, int *nseq, int *nhor){
  int cnt;
  char ch; 
  int i;
  int seqcnt=0, seqlen=0;
  int horseqcnt=0, horseqlen=0;

  cnt = 0; i=0;

  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }

  seqcnt = cnt;
  seqlen = ALPHASATLEN;

  cnt = 0; i=0;

  rewind(horFile);

  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }


  horseqlen = ALPHASATLEN;
  horseqcnt = cnt;
  
  rewind(fastaFile);
  fprintf(stderr, "seqcnt: %d seqlen: %d\n", seqcnt, seqlen);
  fprintf(stderr, "horseqcnt: %d horseqlen: %d\n", horseqcnt, horseqlen);

  horseqs = (char **) malloc((horseqcnt+1) * sizeof(char *));
  
  for (i=0; i<=horseqcnt; i++)
    horseqs[i] = (char *) malloc(horseqlen);
  
  hornames = (char **) malloc((horseqcnt+1) * sizeof(char *));

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen);

  clustered = (int *) malloc((seqcnt+1) * sizeof(int));
  

  for (i=0; i<=horseqcnt; i++)
    hornames[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<horseqcnt; i++){
    horseqs[i][0] = 0;
    hornames[i][0] = 0;
  }

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
    clustered[i] = 0;
  }

  cnt = -1; 
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(names[cnt], SEQ_LENGTH, fastaFile);
      names[cnt][strlen(names[cnt])-1] = 0;
      //printf("seq-%d: %s\n", cnt, names[0]);
    }
    i = 0;
    if (cnt != 0)
      seqs[cnt][i++] = ch;
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	seqs[cnt][i++] = ch;
    } while (ch != '>');
    seqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(names[cnt], SEQ_LENGTH, fastaFile);
	names[cnt][strlen(names[cnt])-1] = 0;
	//printf("seq-%d: %s\n", cnt, names[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read from fasta\n",seqcnt);


  // hor file
  cnt = 0; i=0;

  rewind(horFile);

  cnt = -1; 
  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(hornames[cnt], SEQ_LENGTH, horFile);
      trim(hornames[cnt]);
      //      hornames[cnt][strlen(hornames[cnt])-1] = 0;
      //printf("seq-%d: %s\n", cnt, hornames[0]);
    }
    i = 0;
    if (cnt != 0)
      horseqs[cnt][i++] = ch;
    do{
      if (!(fscanf(horFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	horseqs[cnt][i++] = ch;
    } while (ch != '>');
    horseqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(hornames[cnt], SEQ_LENGTH, horFile);
	trim(hornames[cnt]);
	//hornames[cnt][strlen(hornames[cnt])-1] = 0;
	//printf("horseq-%d: %s\n", cnt, hornames[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read\n",horseqcnt);

  *nseq = seqcnt;
  *nhor = horseqcnt;
} 

