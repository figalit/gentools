/* 
   employs a trie to find the frequency of each k-mer
*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <pthread.h>

#define NODRAW

#define SEQ_LENGTH 200
#define FILENAME_LENGTH 20
#define FOSMID_LENGTH 5500
#define IDENTITY 0.90
#define INTERNALNODE 1909090

#define HISTSTRETCH 20


//#define DEBUG


typedef struct trie{
  int freq;
  struct trie *kids[4]; //child nodes
}strie;

int WINDOW_SIZE;
int SLIDE_SIZE = 1;
int DUMPTREE;
int SAVEINDEX;
int MINWIN;
int THREADS;
int INDEXSTART;
int NOREV = 0;
int UNIQ = 0;

int genomelen;

void help(char *);


char *readFasta(FILE *, int *);
int getWindow(char *, int, char **, int);

int getIndexWindow(FILE *, char **, int *);
int cindex(char);
void alloc_trie(struct trie **);
void insert(struct trie *, char *, int, char *, int);

void saveTrie(struct trie *, char *);
void saveIndex(struct trie *, FILE *,char *);

void dumpLocation(struct trie *, FILE *);

FILE *myfopen(char *, char *);
void rcomp(char *, char *);
int do_file(char *, struct trie *, int);
void *mymalloc(size_t);
void freeTrie(struct trie *);
void printSeq(FILE *, char *, int, int);


int identifier;
int VERBOSE;
long long triemem;
long long slocmem;
int allocs;
int frees;
int callocs;
int cfrees;
int clallocs;
int clfrees;
int nfosmid;

char QUALFILE[SEQ_LENGTH];

char *genome;

int glen; // length of the genome


pthread_mutex_t  logMutex;
pthread_mutex_t  readMutex;

int main(int argc, char **argv){

  int i, j;
  FILE *fasta;
  FILE *treefile;
  struct trie *root;
  char fname[SEQ_LENGTH*2];
  char idxname[SEQ_LENGTH];
  char command[SEQ_LENGTH*5];
  char fgnuplot[SEQ_LENGTH*5];
  FILE *gnuplot;

  int filestat;

  int LOADED;

  char searchFile[SEQ_LENGTH];
  char seqname[SEQ_LENGTH];

  char gnuseq[SEQ_LENGTH];
  int gnufreq;
  FILE *freqFile;
  FILE *freqList;
  struct timeval start, end;
  struct timezone tz;

  /* directory stuff */
  DIR *dp;
  struct dirent *ep;

  triemem = 0;
  slocmem = 0;
  identifier = 0;

  DUMPTREE = 0;
  VERBOSE = 0;
  SAVEINDEX = 0;
  LOADED = 0;
  MINWIN = 1;
  THREADS = 1;
  idxname[0] = 0;
  genomelen = 0;

  allocs = 0;
  frees = 0;

  if (argc < 3){
    help(argv[0]);
    return 0;
  } 

  fname[0] = 0;

  searchFile[0] = 0;

  QUALFILE[0] = 0;

  pthread_mutex_init(&logMutex, NULL);
  pthread_mutex_init(&readMutex, NULL);

  INDEXSTART = 0;
  seqname[0] = 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-ws"))
      WINDOW_SIZE = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-ss"))
      SLIDE_SIZE = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-nr"))
      NOREV = 1;
    else if (!strcmp(argv[i], "-uniq"))
      UNIQ = 1;
    else if (!strcmp(argv[i], "-v"))
      VERBOSE=1;	
    else if (!strcmp(argv[i], "-f"))
      strcpy(fname, argv[i+1]);	
    else if (!strcmp(argv[i], "-seqname"))
      strcpy(seqname, argv[i+1]);
  }


  if (fname[0] == 0){
    help(argv[0]);
    return 0;
  }
  
  alloc_trie(&root);
  
  if (fname[0] != 0)
    fprintf(stderr,"\tfile:\t\t%s\n",fname);
  fprintf(stderr,"\twindow size:\t%d\n\tslide size:\t%d\n", WINDOW_SIZE, SLIDE_SIZE);
  if (VERBOSE)
    fprintf(stderr,"\tverbose:\t\ton\n");

  filestat = do_file(fname, root, 1);
  if (filestat == 0)
    return 0;

  if (WINDOW_SIZE == 0){ 
    fprintf(stderr, "Strange ws.\n");
    return 0;
  }



  fprintf(stderr, "Total memory for the trie: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", triemem, (float)triemem/1024.0, (float)triemem/1024.0/1024.0);
  
  slocmem = 0;

  allocs = 0;
  frees = 0;
  callocs = 0;
  cfrees = 0;
  clallocs = 0;
  clfrees = 0;

  sprintf(idxname, "%s.k%d.txt", fname, WINDOW_SIZE);

  saveTrie(root, idxname);

  return 1;
}



void help(char *pname){

  fprintf(stderr, "\n%s <options>\n", pname);
  fprintf(stderr, "-f [fastafile]\t:\tInput fastafile for trie indexing.\n");
  fprintf(stderr, "-ws [win_size]\t:\tSet window size for trie indexing.\n");
  fprintf(stderr, "-saveindex\t:\tSave index in a file.\n");

  fprintf(stderr, "-v\t\t:\tVerbose.");
  fprintf(stderr, "-nr\t\t:\tDo not calculate reverse complements.");
  fprintf(stderr, "-uniq\t\t:\tPrint only unique kmers\n\n\n");
}

char *readFasta(FILE *fastaFile, int *length){

  /* there is supposed to be only one long long long sequence */

  int cnt;
  char ch; 
  int i;
  int seqlen=0;
  char dummy[SEQ_LENGTH];
  char *sequence;

  fprintf(stderr, "Checking length.\n");
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      fgets(dummy, SEQ_LENGTH, fastaFile);
    }
    else if (!isspace(ch))
      seqlen++;
  }

  cnt = 0; i=0;

  fprintf(stderr, "Allocating memory for sequence with length %d.\n", seqlen);
  *length = seqlen;
  
  sequence = (char *) mymalloc((seqlen+1) * sizeof(char));
  
  sequence[0] = 0;

  fprintf(stderr, "Reading sequence.\n");

  rewind(fastaFile);

  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      fgets(dummy, SEQ_LENGTH, fastaFile);
    else if (isalpha(ch))
      sequence[i++] = toupper(ch);
  }

  sequence[i] = 0;
  fprintf(stderr, "\n[OK] sequence is read from fasta file.\n");

  return sequence;

}


int getIndexWindow(FILE *fp, char **window, int *willInsert){

  // read non overlapping window from file


  int i;
  char ch;

  i = 0;

  *willInsert = 1;

  while (i<WINDOW_SIZE){
    if (fscanf(fp, "%c", &ch) > 0){
      if (isalpha(ch)){
	(*window)[i++] = ch;
	genomelen++;
      }
      if (ch == 'N' || ch == 'O')
	*willInsert = 0;
    }
    else
      return 0;
  }
  (*window)[i] = 0;
  
  return 1;
}

int getWindow(char *genome, int glen, char **window, int win_start){
  int i;
  int win_end;
  win_end = win_start+WINDOW_SIZE;
  if (win_start >= glen)
    return 0;
  if (win_end>=glen)
    return 0;
  //    win_end = glen;
  for (i=win_start;i<win_end;i++){
    if (genome[i] == 'N' || genome[i] == 'O'){
      return 0;
    }
    (*window)[i-win_start] = genome[i];
    
  }

  (*window)[i-win_start] = 0;


  return 1;
}

int cindex(char ch){
  switch(toupper(ch)){
  case 'A':
    return 0;
    break;
  case 'C':
    return 1;
    break;
  case 'G':
    return 2;
    break;
  case 'T':
    return 3;
    break;
  }
  return 3;
}

void alloc_trie(struct trie **t){
  (*t) = (struct trie *) mymalloc(sizeof(struct trie));
  if (*t == NULL){
    fprintf(stderr, "Insufficient memory\n");
    exit(0);
  }
  triemem+=sizeof(struct trie);
  (*t)->freq = INTERNALNODE;

  (*t)->kids[0] = NULL;
  (*t)->kids[1] = NULL;
  (*t)->kids[2] = NULL;
  (*t)->kids[3] = NULL;
  
}

void insert(struct trie *root, char *window, int start, char *seqname, int which){
  struct trie *current = root;
  int i;
  int loc;

  for (i=0;i<strlen(window);i++){
    loc = cindex(window[i]);
    if (current->kids[loc] == NULL){
      if (VERBOSE)
	printf("alloc for %c - %d\n", window[i], loc);
      alloc_trie(&(current->kids[loc]));
    }
    current = current->kids[loc];
  }

  /* not a leaf node any more */
  if (current->freq == INTERNALNODE) 
    current->freq = 0;
    

  if (which == 1)
    current->freq++;
  else
    current->freq--;


  // we're at the leaf now; fix this for multiple hits.
  // insert_sloc(&(current->location), start, seqname);
  //strcpy(current->seq, seqname);
  if (VERBOSE)
    printf("inserted %d\n", start);
  //current->location = start;
}


int do_file(char *fname, struct trie *root, int which){

  FILE *fasta;
  char *genome;
  int glen;
  int i;
  char *window;
  char *rcwindow;
  int willInsert;
  char dummy[SEQ_LENGTH];
  int win = 0;

  willInsert = 1;

  
  fprintf(stderr, "\n\nInserting sequence from file %s\n", fname);
  fasta = myfopen(fname, "r");
  fgets(dummy, SEQ_LENGTH, fasta);

  genome = readFasta(fasta, &glen);  
  
  window = (char *) mymalloc(sizeof(char) * WINDOW_SIZE+1);
  rcwindow = (char *) mymalloc(sizeof(char) * WINDOW_SIZE+1);
  
  i = 0;


  while (i < glen - WINDOW_SIZE){

    if  (getWindow(genome, glen, &window, i) == 0 ){
      i+=SLIDE_SIZE;
      continue;    
    }
      
    //while (getIndexWindow(fasta, &window, &willInsert) > 0){
    //fprintf(stderr, "\rGetting windows from %s\t: %d%%", fname, ((int)(100.0*(float)(i+strlen(window))/(float)strlen(genome))));
    if (VERBOSE)
      printf("%s\n", window);
    /* insert the window to the trie here */
    if (willInsert){
      // start location is i+1
      // because i starts from 0 here; but 
      // we want the sequence index starting from 1
      // STARTING POSITION FOR FORWARD
     

      rcomp(window, rcwindow);
      if (NOREV){
	if (strcmp(window, rcwindow) < 0 )
	  insert(root, window, (i+1), fname, which);      
	else if (strcmp(window, rcwindow) > 0 )
	  insert(root, rcwindow, (-1*(i+1)), fname, which);      
      }
      else{
	insert(root, window, (i+1), fname, which);      
	insert(root, rcwindow, (-1*(i+1)), fname, which);      
      }
      /*
      insert(root, window, (i+1), fname, which);      
      if (!NOREV){
      // reverse complement of this window is indexed by its negative
      // value ENDING POSITION FOR REVERSE COMPLEMENT
	rcomp(window, rcwindow);
	if (strcmp(window, rcwindow))
	  insert(root, rcwindow, (-1*(i+1)), fname, which);      
	  }*/
    }
    i+=SLIDE_SIZE;
    if (i % 10000 == 0)
      fprintf(stderr,"\r [%i] of [%d]", i, glen);
  }
  fprintf(stderr, "\n");
  
  //free(genome);
  fclose(fasta);
  free(window);
  free(rcwindow);
  
  return 1;
}


void saveTrie(struct trie *root, char *fprefix){
  char fname[SEQ_LENGTH];
  FILE *indexFile;
  char window[WINDOW_SIZE];
  
  sprintf(fname, "%s.index", fprefix);
  
  indexFile = myfopen(fname, "w");

  //fprintf(indexFile, "WS %d\tSS %d %s %d\n", WINDOW_SIZE, SLIDE_SIZE, fprefix, genomelen);
  window[0] = 0;
  saveIndex(root, indexFile, window);

  fclose(indexFile);
  
}




void saveIndex(struct trie *root, FILE *fp, char *window){
  struct trie *current;
  char newWindow[WINDOW_SIZE];
  current = root;
  if (current == NULL)
    return;
  if (current->freq != INTERNALNODE){ // leaf node
    if (!UNIQ || current->freq==1)
      fprintf(fp, "%s\t\t%d\n", window, current->freq);
  }
  else{
    sprintf(newWindow, "%sA", window);
    saveIndex(current->kids[0], fp, newWindow);
    sprintf(newWindow, "%sC", window);
    saveIndex(current->kids[1], fp, newWindow);
    sprintf(newWindow, "%sG", window);
    saveIndex(current->kids[2], fp, newWindow);
    sprintf(newWindow, "%sT", window);
    saveIndex(current->kids[3], fp, newWindow);
  }
}


FILE *myfopen(char *fname, char *mode){
  FILE *fp;
  fp = fopen(fname, mode);
  if (fp == NULL){
    fprintf(stderr, "Unable to open file %s\n", fname);
    exit (0);
  }
  return fp;
}

void rcomp(char *window, char *rcomp){
  /* reverse complement */
  int i;
  for (i=0;i<WINDOW_SIZE;i++)
    switch (window[i]){
    case 'A':
      rcomp[WINDOW_SIZE-i-1] = 'T';
      break;
    case 'C':
      rcomp[WINDOW_SIZE-i-1] = 'G';
      break;
    case 'G':
      rcomp[WINDOW_SIZE-i-1] = 'C';
      break;
    case 'T':
      rcomp[WINDOW_SIZE-i-1] = 'A';
      break;
    default:
      break;
    }    
  rcomp[WINDOW_SIZE] = 0;
}


void *mymalloc(size_t size){
  void *ret;
  ret = malloc(size);
  
  if (ret == NULL){
    fprintf(stderr, "Insufficient memory.\n");
    fprintf(stderr, "Sloc usage: %lld\n", slocmem);
    fprintf(stderr, "Allocs: %d\nFrees: %d\n", allocs, frees);
    exit (0);
  }

  return ret;
}

