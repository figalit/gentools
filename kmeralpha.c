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
#define GEN_LENGTH 250000000
#define IDENTITY 0.90
#define INTERNALNODE 1909090

#define HISTSTRETCH 20


//#define DEBUG


/*
typedef struct cmember{
  //int start; // location start
  int windowID;
  struct cmember *next;
}scmember;
*/

/*
typedef struct sloc{
  int start; // start index
  /* if start index is negative, then that window is
     reverse-complemented 
  struct sloc *next;
  //int fromWindow;
  struct cmember *windows;
}slocation;
*/

typedef struct trie{
  /*  struct sloc *location; 
#ifndef NODRAW
  int pid;
  int id;
  #endif
  */
  
  int freq;
  struct trie *kids[4]; //children nodes
}strie;


typedef struct clonenode{
  int count;
  char name[20];
  struct clonenode *next;
}sclone;


/*

typedef struct cluster{
  int nmember; // no of members
  int start;
  int end;
  int startWin;
  struct cmember *members;
  struct cluster *next;
}scluster;


*/

typedef struct threadParam{
  FILE *in;
  FILE *out;
  FILE *qual;
  struct trie *trieRoot;
}tparam;



int WINDOW_SIZE;
int SLIDE_SIZE;
int DUMPTREE;
int SAVEINDEX;
int MINWIN;
int THREADS;
int INDEXSTART;
int totseqs;

int genomelen;

void help(char *);
int readFosmid(FILE *, char *, char *);
int readQual(FILE *, int *, char *);
char *readFasta(FILE *, int *);
int getWindow(char *, int, char **, int);
  //int getWindow(char *, int, char **, int, int *);
int getIndexWindow(FILE *, char **, int *);
int cindex(char);
void alloc_trie(struct trie **);
void insert(struct trie *, char *, int, char *, int);
//void drawTrie(FILE *, struct trie *, char);
void saveTrie(struct trie *, char *);
void saveIndex(struct trie *, FILE *,char *);
void loadTrie(char *, struct trie **);
void dumpLocation(struct trie *, FILE *);

/*
void concatLocations(struct sloc **, struct sloc *, int);
void freeLocation(struct sloc *);
void findPositions(struct sloc *, char *, int, FILE *, int, char *);
*/
void do_search(struct trie *, char *, char *);

FILE *myfopen(char *, char *);
void rcomp(char *, char *);
//int search(struct trie *, char *, struct sloc **, int);
int search(struct trie *, char *, int *, int);
//void insert_sloc(struct sloc **, int, char *);
int do_file(char *, struct trie *, int);
//struct sloc *copyLocation(struct sloc *, int);
void *mymalloc(size_t);
void freeTrie(struct trie *);
void printSeq(FILE *, char *, int, int);


/* cluster stuff from pimp.c */
/*

void alloc_cmember(struct cmember **);
void insert_cmember(struct sloc **, int);
void free_cmember(struct cmember *);
void alloc_cluster(struct cluster **cl);
void free_cluster(struct cluster *cl);
void insert_cmember_to_cluster(struct cluster **, int, int);
void insertCluster(struct cluster **, struct sloc *, struct sloc *, char *, FILE *, int, int, char *);

*/


void getCloneName(char *, char *);
void alloc_clone(struct clonenode **);
void insert_clone(struct clonenode **, int, char *, FILE *);
void *threadSearch(void *);
int isReverse(char *);

/* alignment stuff from pimp.c */
/*
float sig(char s, char t); // character similarity score
float max3(float, float, float);
float max(float, float);
float align(char *, char *);
*/
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

char DBNAME[SEQ_LENGTH];
char DBNAME2[SEQ_LENGTH];

char QUALFILE[SEQ_LENGTH];

char *genome;
//char genome[GEN_LENGTH];
char GENOME[GEN_LENGTH];
int glen; // length of the genome


pthread_mutex_t  logMutex;
pthread_mutex_t  readMutex;

int main(int argc, char **argv){

  int i, j;
  FILE *fasta;
  FILE *treefile;
  struct trie *root;
  char fname[SEQ_LENGTH*2];
  char fname2[SEQ_LENGTH*2];
  char idxname[SEQ_LENGTH];
  char command[SEQ_LENGTH*5];
  char fgnuplot[SEQ_LENGTH*5];
  FILE *gnuplot;
  //  char seqname[SEQ_LENGTH];
  int filestat;
  /* regular expression stuff */
  regex_t preg;
  int status;
  char startdir[SEQ_LENGTH];
  char regexpr[SEQ_LENGTH];
  int LOADED;
  int build_start;
  int build_end;
  int build_cnt;
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

  regexpr[0]=0;


  startdir[0]=0;
  fname[0] = 0;
  fname2[0] = 0;
  build_start = 0;
  build_end = 0;
  build_cnt = 0;
  searchFile[0] = 0;
  DBNAME[0] = 0;
  GENOME[0] = 0;
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
    else if (!strcmp(argv[i], "-v"))
      VERBOSE=1;	
    else if (!strcmp(argv[i], "-dir"))
      strcpy(startdir, argv[i+1]);	
    else if (!strcmp(argv[i], "-re"))
      strcpy(regexpr, argv[i+1]);	
    else if (!strcmp(argv[i], "-f"))
      strcpy(fname, argv[i+1]);	
    else if (!strcmp(argv[i], "-f2"))
      strcpy(fname2, argv[i+1]);	
    else if (!strcmp(argv[i], "-start"))
      build_start = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-end"))
      build_end = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-seqname"))
      strcpy(seqname, argv[i+1]);
    else if (!strcmp(argv[i], "-qualfile"))
      strcpy(QUALFILE, argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(searchFile, argv[i+1]);
    else if (!strcmp(argv[i], "-loadindex")){
      LOADED = 1;
      strcpy(idxname, argv[i+1]);
    }
  }


  if ((build_start == 0) ^ (build_end == 0)){
    fprintf(stderr, "Strange build_start and build_end.\n");
    return 0;
  }


  if (fname[0] && startdir[0]){
    help(argv[0]);
    return 0;
  }
  
  alloc_trie(&root);
  
  if (fname[0] != 0)
    fprintf(stderr,"\tfile:\t\t%s\n",fname);
  if (startdir[0] != 0)
    fprintf(stderr,"\tdir:\t\t%s\n",startdir);
  fprintf(stderr,"\twindow size:\t%d\n\tslide size:\t%d\n", WINDOW_SIZE, SLIDE_SIZE);
  if (VERBOSE)
    fprintf(stderr,"\tverbose:\t\ton\n");
  if (regexpr[0] != 0)
    fprintf(stderr,"\tregexp:\t\t%s\n",regexpr);
  if (build_start != 0)
    fprintf(stderr,"\tbuild_start:\t%d\n\tbuild_end:\t%d\n",build_start, build_end);
  
  
  if (regcomp(&preg, regexpr, 0) != 0) {
    printf("cant compile regular expression\n");
    return 0;      /* Report error. */
  }
  

  if (!LOADED){

  
    if (startdir[0] != 0){
      dp=opendir(startdir);
      while((ep=readdir(dp))){
	if (ep->d_name[0] == '.')
	  continue;
	if (ep->d_type == DT_DIR)
	  continue;
	if (regexpr[0] != 0){
	  status = regexec(&preg, ep->d_name, (size_t) 0, NULL, 0);
	  if (status != 0)
	    continue;
	}
	build_cnt++;
	if (build_start != 0){
	  if (build_cnt<build_start || build_cnt>build_end)
	    continue;
	}
	sprintf(fname, "%s/%s", startdir, ep->d_name);
	filestat = do_file(fname, root, 1);
	if (filestat == 0)
	  return 0;
      }
      closedir(dp);
    }
    else if (fname[0]!=0){ 
      
      filestat = do_file(fname, root, 1);
      if (filestat == 0)
	return 0;
      
      if (fname2[0] != 0){
	filestat = do_file(fname2, root, -1);
	if (filestat == 0)
	  return 0;
      }
      
      
    }

    sprintf(idxname, "%s.%s.k%d.txt", fname, fname2, WINDOW_SIZE);
    
    saveTrie(root, idxname);
    
  }
  
  else if (LOADED){
    if (idxname[0] == 0)
      return 0;
    loadTrie(idxname, &root);
  }


  if (WINDOW_SIZE == 0){ // deprecated: || SLIDE_SIZE == 0 || SLIDE_SIZE>WINDOW_SIZE){
    fprintf(stderr, "Strange ws.\n");
    return 0;
  }


  
  
  if (GENOME[0] != 0){
    fasta = myfopen(fname, "r");    
    genome = readFasta(fasta, &glen);  
  }


  fprintf(stderr, "Total memory for the trie: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", triemem, (float)triemem/1024.0, (float)triemem/1024.0/1024.0);
  fprintf(stderr, "Total memory for the locations: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
  
  slocmem = 0;

  allocs = 0;
  frees = 0;
  callocs = 0;
  cfrees = 0;
  clallocs = 0;
  clfrees = 0;


  if (seqname[0]!=0)
    strcpy(DBNAME, seqname);


  if (searchFile[0] != 0)
    do_search(root, searchFile, "");

  return 1;
}



void help(char *pname){
  //fprintf(stderr, "%s <-f [fastafile] OR -dir [startdir]> -ws [window_size] -ss [slide_size] <-dumptree> <-v>\n", pname);
  fprintf(stderr, "\n%s <options>\n", pname);
  fprintf(stderr, "\nINDEXING OPTIONS:\n\n");
  fprintf(stderr, "-f [fastafile]\t:\tInput fastafile for trie indexing.\n");
  fprintf(stderr, "-dir [startdir]\t:\tInput all files in directory startdir for trie indexing.\n");
  fprintf(stderr, "-start [start]\t:\tWhen indexing with -dir option, start from file #start.\n");
  fprintf(stderr, "-end [end]\t:\tWhen indexing with -dir option, stop at file #end.\n");
  fprintf(stderr, "-re [reg_exp]\t:\tWhen indexing with -dir option, index files with names that match to reg_exp only.\n");
  fprintf(stderr, "-ws [win_size]\t:\tSet window size for trie indexing.\n");
  fprintf(stderr, "-saveindex\t:\tSave index in a file.\n");

  /*
  fprintf(stderr, "\nSEARCHING OPTIONS:\n\n");
  fprintf(stderr, "-loadindex [inf]:\tLoad index from [inf].\n");
  fprintf(stderr, "-qualfile [quff]:\tLoad quality values from [quf]\n");
  fprintf(stderr, "-threads [n_th]\t:\tUse multithread version with n_th number of threads.\n");
  fprintf(stderr, "-search [sfile]\t:\tSearch fosmids in file sfile.\n");
  fprintf(stderr, "-istart [istart]:\tUpdate loaded indexes by adding istart-1; use for contigs.\n");
  fprintf(stderr, "-seqname [sname]:\tUpdate sequence name with sname; use for contigs.\n");
  */
  fprintf(stderr, "\nCOMMON OPTIONS:\n\n");
  fprintf(stderr, "-v\t\t:\tVerbose.\n\n\n");
}

char *readFasta(FILE *fastaFile, int *length){

  /* there is supposed to be only one long long long sequence */

  int cnt;
  char ch; 
  int i;
  int seqlen=0;
  char dummy[SEQ_LENGTH];
  char *sequence;

  /*

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
      sequence[i++] = ch;
  }

  sequence[i] = 0;
  fprintf(stderr, "\n[OK] sequence is read from fasta file.\n");
  */

  fgets(dummy, SEQ_LENGTH, fastaFile);

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
  /*
  (*t)->location = NULL;
#ifndef NODRAW
  (*t)->id = identifier++;
#endif
*/
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


int search(struct trie *root, char *window, int *count, int windowID){
  int i;
  int loc;
  struct trie *current = root;
  

  for (i=0;i<strlen(window);i++){
    loc = cindex(window[i]);
    if (current->kids[loc] == NULL){
      return 0; // not found
    }
    current = current->kids[loc];
  }
  if (current == NULL){
    return 0;
  }
 
  *count = current->freq;
  
  //concatLocations(location, current->location, windowID);

  return 1;
  
}


void alloc_clone(struct clonenode **loc){
  (*loc) = (struct clonenode *) mymalloc (sizeof(struct clonenode));
  if (*loc == NULL){
    fprintf(stderr, "Insufficient memory\n");
    exit(0);
  }
  (*loc)->next = NULL;
  (*loc)->name[0] = 0;
  (*loc)->count = 0;
}


void insert_clone(struct clonenode **location, int count, char *seqname, FILE *cloneout){

  struct clonenode *new;
  struct clonenode *posparent;
  struct clonenode *pos;

  //printf("inserting %d\n", count);

  if ((*location) == NULL){
    alloc_clone(location);
    //if (count > 0) // forward
    (*location)->count = count;
    strcpy((*location)->name, seqname);
    //else  // reverse complement
    //(*location)->rccount = count;
      
    return;
  }

  
  pos = (*location);
  posparent = pos;

  while (pos != NULL){
    if (!strcmp(pos->name, seqname)){
      pos->count += count;
      fprintf(cloneout, "%s\t%d\n", pos->name, pos->count);
      if (pos == (*location)){
	(*location) = (*location)->next;
	free(pos);
	//*location = NULL;
	return;
      }
      posparent->next = pos->next;
      free(pos);
      return;
    } 
    posparent = pos;
    pos = pos->next;
  }


  // couldn't find the clone 
  alloc_clone(&new);
  new->count = count; 
  strcpy(new->name, seqname);

  new->next = (*location);
  (*location) = new;

}


/*


void alloc_sloc(struct sloc **loc){
  (*loc) = (struct sloc *) mymalloc (sizeof(struct sloc));
  if (*loc == NULL){
    fprintf(stderr, "Insufficient memory\n");
    exit(0);
  }
  allocs++;
  slocmem+=sizeof(struct sloc);
  (*loc)->next = NULL;
  (*loc)->windows = NULL;
  (*loc)->start = 0;
  //(*loc)->rcstart = 0;
}

*/

/*
void insert_sloc(struct sloc **location, int start, char *seqname){

  struct sloc *new;
  struct sloc *pos;
  struct sloc *posparent;

  //printf("inserting %d\n", start);

  if ((*location) == NULL){
    alloc_sloc(location);
    //if (start > 0) // forward
    (*location)->start = start;
    //else  // reverse complement
    //(*location)->rcstart = start;
      
    return;
  }

  alloc_sloc(&new);
  new->start = start; 

  // insertion sort :(

  
  if (*location == NULL || start > (*location)->start){
    new->next = (*location);
    (*location) = new;
    //printf("inserted %d\n", start);
    return;
  }

  pos = *location;
  posparent = pos;
  
  while (pos != NULL && pos->start > start){
    //printf("posnotnull %d / %d\n", pos->start, start);
    posparent = pos;
    pos = pos->next;
  }

  // insert new in front of pos
  new->next = pos;
  posparent->next = new;


  // printf("inserted %d\n", start);


}

*/

int do_file(char *fname, struct trie *root, int which){

  FILE *fasta;
  //  char genome[GEN_LENGTH];
  //int qual[FOSMID_LENGTH];
  //char *genome;
  int glen;
  int i,j;
  char *window;
  char *rcwindow;
  int willInsert;
  char dummy[SEQ_LENGTH];
  char name[SEQ_LENGTH];
  char qname[SEQ_LENGTH];
  char ch;
  int cnt = 0;
  FILE *qfile = NULL;
  int qlen;
  int lowqual;

  if (QUALFILE[0] != 0){
    qfile = myfopen(QUALFILE, "r");
    fscanf(qfile, "%c", &ch);  // pass first >
  }
  


  
  fprintf(stderr, "\n\nInserting sequence from file %s\n", fname);
  fasta = myfopen(fname, "r");
  //fgets(dummy, SEQ_LENGTH, fasta);

  // genome = readFasta(fasta, &glen);  
  
  window = (char *) mymalloc(sizeof(char) * WINDOW_SIZE+1);
  rcwindow = (char *) mymalloc(sizeof(char) * WINDOW_SIZE+1);
  
  
  //readFosmid(fasta, genome, dummy, NULL. NULL);
  
  //fgets(genome, SEQ_LENGTH, fasta);
  //if (genome[0] == '>')
  // fgets(genome, SEQ_LENGTH, fasta);
  //genome[strlen(genome)-1] = 0;

  fscanf(fasta, "%c", &ch);  // pass first >
  
  while (readFosmid(fasta, GENOME, name)){
    glen = strlen(GENOME);
    
    /*
    if (QUALFILE[0] != 0){
      qlen = readQual(qfile, qual, qname);
      if (strcmp(qname, name)){
	fprintf(stderr, "Fosmid and quality names do not match: %s\t%s !!!.\n", name, qname);
	exit(0);
      }
      
      if (qlen != glen){
	fprintf(stderr, "Fosmid and quality lengths do not match: %s/%d\t%s/%d !!!.\n", name, i, qname, qlen);
	exit(0);
      }
    }
    
    */

    cnt++;
    fprintf(stderr,"\r%d", cnt);
    i = 0;
    if (VERBOSE)
      printf("%s\n", GENOME);

    while (getWindow(GENOME, glen, &window, i) > 0){
      willInsert = 1;

      if (VERBOSE)
	printf("%s\n", window);
      /* insert the window to the trie here */
      lowqual = 0;
      for (j=0; j<strlen(window); j++){
	ch = toupper(window[j]);
	if (ch!='A' && ch!='T' && ch!='G' && ch!='C')
	  willInsert = 0;
	/*
	if (QUALFILE[0] != 0){
	  if (qual[i+j] < 20)
	    lowqual++;
	  if (lowqual > strlen(window)/2)
	    willInsert = 0;
	    }*/
      }
      if (willInsert){
	// start location is i+1
	// because i starts from 0 here; but 
	// we want the sequence index starting from 1
	// STARTING POSITION FOR FORWARD
	if (isReverse(name)){
	  rcomp(window,rcwindow);
	  insert(root, rcwindow, (i+1), fname, which);      
	}
	else
	  insert(root, window, (i+1), fname, which);      	  
	// I dont really need the reverse complement
	// rcomp(window, rcwindow);
	// reverse complement of this window is indexed by its negative
	// value ENDING POSITION FOR REVERSSE COMPLEMENT
	// insert(root, rcwindow, (-1*(i+1)), fname, which);      
      }
      i+=SLIDE_SIZE;
    }
    //readFosmid(fasta, genome, dummy, NULL. NULL);
    //fgets(genome, SEQ_LENGTH, fasta);
    //genome[strlen(genome)-1] = 0;
    //glen = strlen(genome);
  }
  fprintf(stderr, "\n");
  
  //free(genome);
  fclose(fasta);
  free(window);
  free(rcwindow);
  
  totseqs=cnt;
  
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



/*
void freeTrie(struct trie *root){

  if (root == NULL)
    return;
  if (root->location != NULL) // leaf node
    freeLocation(root->location);
  
  freeTrie(root->kids[0]);
  freeTrie(root->kids[1]);
  freeTrie(root->kids[2]);
  freeTrie(root->kids[3]);

  free(root);
  triemem-=sizeof(struct trie);

}
*/

/*
void dumpLocation(struct trie *root, FILE *fp){
  struct sloc *location;
  char seq[SEQ_LENGTH];
  location = root->location;
  seq[0] = 0;
  while (location != NULL){
    //if (location->start != 0)
    fprintf(fp,"%d\t", location->start);
    //if (location->rcstart != 0)
    //fprintf(fp,"%d\t", location->rcstart);
    location = location->next;
  }
}

*/

/*
struct sloc *copyLocation(struct sloc *source, int windowID){
  struct sloc *new;
  struct sloc *current;
  struct sloc *ret;
  struct sloc *this;


  if (source == NULL)
    return NULL;

  
  // NOTE: locations are already reverse-sorted in descending order
  

  current = source;
  ret = NULL;
  alloc_sloc(&new);
  new->start = current->start;
  insert_cmember(&new, windowID);
  ret = new;
  this = ret;
  current = current->next;

  while (current != NULL){
      alloc_sloc(&new);
      new->start = current->start;
      insert_cmember(&new, windowID);
      this->next = new;
      this = this->next;
      current = current->next;
  }

  return ret;
}

*/

/*

void concatLocations(struct sloc **s1, struct sloc *s2, int windowID){
  struct sloc *new;
  struct sloc *s1t, *s2t;
  struct sloc *s1p;

  /* inputs are sorted IN DESCENDING ORDER 

  if (s2 == NULL)
    return;

  if (*s1 == NULL){
    (*s1) = copyLocation(s2, windowID);
    return;
  }

  s1t = *s1;
  s1p = s1t;
  s2t = s2;  
  
  while (s2t != NULL){
    if (s2t->start > s1t->start){
      alloc_sloc(&new);
      new->start = s2t->start;
      new->next = s1t;
      insert_cmember(&new, windowID);
      if (s1t == (*s1)){
	(*s1) = new;
	s1p = (*s1);
	s1t = (*s1);
      }
      else{
	s1t = new;
	s1p->next = new;
      }
      s2t = s2t->next;
      // s1p and s1t stays the same
    }
    else if (s2t->start < s1t->start){
      // no copy here
      // s2t stays the same
      if (s1p == s1t->next){
	printf("loop\n");
	exit(0);
      }
      s1p = s1t;
      s1t = s1t->next;
    }
    
    else{ // if they are equal
      //nothing to do here, just move; keep windowID's
      s1p = s1t;
      insert_cmember(&s1t, windowID);
      s1t = s1t->next;
      s2t = s2t->next;
    }

    if (s1t == NULL && s2t!=NULL){
      s1p->next = copyLocation(s2t, windowID);
      break;
    }
  }
  

}

*/

/*

void freeLocation(struct sloc *newloc){
  struct sloc *tmp;
 
  while (newloc != NULL){
    tmp = newloc->next;
    free_cmember(newloc->windows);
    free(newloc);
    frees++;
    slocmem = slocmem - sizeof(struct sloc);
    newloc = tmp;
  }
}

*/


/*
void free_cmember(struct cmember *cm){
  struct cmember *tmp;
  while (cm != NULL){
    tmp = cm->next;
    free(cm);
    cfrees++;
    cm = tmp;
  }
}

*/
/*
void free_cluster(struct cluster *cl){
  struct cluster *tmp;

  while (cl != NULL){
    tmp = cl->next;
    free_cmember(cl->members);  
    free(cl);
    clfrees++;
    cl = tmp;
  }

}

*/


void loadTrie(char *fname, struct trie **root){
  FILE *fp;
  int ws, ss;
  char dummy[SEQ_LENGTH];
  char dummy2[SEQ_LENGTH];
  char name[SEQ_LENGTH];
  char *window;
  int start;
  int i, j;
  int loc;
  struct trie *current = *root;

  fp = myfopen(fname, "r");
  fprintf(stderr, "Loading %s\n", fname);
  
  alloc_trie(root);
  


  //  fscanf(fp, "%s %d %s %d %s %d", dummy, &ws, dummy2, &ss, DBNAME, &genomelen);

  //genome = (char *) mymalloc(sizeof(char) * genomelen+1);

  strcpy(DBNAME2, DBNAME);
  // WINDOW_SIZE = ws;
  //SLIDE_SIZE = ss;
  
  window = (char *) mymalloc(sizeof(char) * (ws+1));
  name[0] = 0;
  
  while (fscanf(fp, "%s %d", window, &start) > 0){


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
  
  
  
    current->freq = start;
    
   

  }
  

  fclose(fp);
  free(window);
}
 



void do_search(struct trie *root, char *sfile, char *qfile){
  FILE *fp;
  FILE *qp;
  char fosmidName[SEQ_LENGTH];
  char ch;
  FILE *output;
  struct threadParam *params;
  int tid; int rc;
  char cfilename[100];
  pthread_t *threads;
  
  
  threads = (pthread_t *) mymalloc(sizeof(pthread_t) * THREADS);

  sprintf(fosmidName, "%s.%s.out", sfile, DBNAME2); // reuse :)
  output = myfopen(fosmidName, "w");
  fosmidName[0] = 0;

  nfosmid = 0;
  fprintf(stderr, "\n\nStart Searching %s ...\n\n", sfile);
    
  fp = myfopen(sfile, "r");
  fscanf(fp, "%c", &ch);  // pass first >
  if (qfile[0] != '\0'){
    qp = myfopen(qfile, "r");
    fscanf(qp, "%c", &ch);  // pass first >
  }
  else
    qp = NULL;

  // qp is replaced to mean the cloneout
  sprintf(cfilename, "%s.%s.clone", sfile, DBNAME2); // reuse :)
  qp = myfopen(cfilename, "w");
  
  params = (struct threadParam *) mymalloc(sizeof(struct threadParam));
  params->in = fp;
  params->qual = qp;
  params->out = output;
  params->trieRoot = root;
  
  for(tid = 0; tid < THREADS; tid++){
    rc = pthread_create(&(threads[tid]), NULL, threadSearch, (void *) params );
    if (rc){
      fprintf(stderr, "Error creating thread #%d\n", (tid+1));
      exit(-1);
    }
  }

  for(tid = 0; tid < THREADS; tid++){
    rc = pthread_join(threads[tid], NULL);
    if (rc){
      fprintf(stderr, "Error (%d) returned from thread #%d\n", rc, (tid+1));
      exit(-1);
    }
  }

  free(threads);

}


void *threadSearch(void *parm){
  
  struct trie *root;
  FILE *fp;
  FILE *qp;
  FILE *output;
  char fosmid[FOSMID_LENGTH];
  int  fqual[FOSMID_LENGTH];
  char *window;
  char *rcwindow;
  int qflen; // quality based flen
  int flen;
  int minwin;
  int i,j;
  char fosmidName[SEQ_LENGTH];
  int willSearch;
  int skipN;
  int count;
  int wincount;
  struct clonenode *clones;
  char clonename[25];
  int cnt = 0;
  char **uniqWindows;
 
  root = (struct trie *) (((struct threadParam *)parm)->trieRoot);
  fp = (FILE *) (((struct threadParam *)parm)->in);
  qp = (FILE *) (((struct threadParam *)parm)->qual);
  output = (FILE *) (((struct threadParam *)parm)->out);

  
  window = (char *)mymalloc(sizeof(char) * (WINDOW_SIZE+1));
  rcwindow = (char *)mymalloc(sizeof(char) * (WINDOW_SIZE+1));
  fosmidName[0] = 0;
  clones = NULL;

  while (readFosmid(fp, fosmid, fosmidName)){
    count = 0;
    
    if (VERBOSE)
      fprintf(stderr, "Searching %s\n", fosmidName);

    flen = strlen(fosmid);
#ifdef DEBUG
    printf("flen: %d\n", flen);
#endif
    qflen = flen;

    i=0;

    /*
    uniqWindows = (char **)malloc(sizeof(char *)*flen+1);
    for (i=1;i<flen;i++)
      uniqWindows[i] = (char *)malloc(sizeof(char)*WINDOW_SIZE+1);
    for (i=1;i<flen;i++)
      uniqWindows[i][0] = 0;
    */

    while (i<flen-WINDOW_SIZE+1){
      willSearch = getWindow(fosmid, flen, &window, i);  
      
      /*
      if (willSear
      for (j=0;j<flen;j++){
	
      }*/

      if (willSearch){
	if (isReverse(fosmidName)){
	  rcomp(window, rcwindow);
	  search(root, rcwindow, &wincount, i);	
	}
	else
	  search(root, window, &wincount, i);	
	
	count+=wincount;
      }
      i++;
    }
    getCloneName(clonename, fosmidName);
    if (qp != NULL)
      insert_clone(&clones, count, clonename, qp);
    fprintf(output, "%s\t%d\n", fosmidName, count);

    cnt++;
    printf("%d\tof\t%d\r", cnt, totseqs);
    }
  
  printf("\n");
  /*
  if (qp != NULL){
    while (clones!=NULL){
      fprintf(qp, "%s\t%d\n", clones->name, clones->count);
      clones = clones->next;
    }
    }*/
    
 
  free(window);
  pthread_exit(NULL);
  return NULL;
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
  int len = strlen(window);
  for (i=0;i<len;i++)
    switch (toupper(window[i])){
    case 'A':
      rcomp[len-i-1] = 'T';
      break;
    case 'C':
      rcomp[len-i-1] = 'G';
      break;
    case 'G':
      rcomp[len-i-1] = 'C';
      break;
    case 'T':
      rcomp[len-i-1] = 'A';
      break;
    default:
      break;
    }    
  rcomp[len] = 0;
}

int readFosmid(FILE *fp, char *fosmid, char *name){
  int i;
  int qlen;
  char ch=0;


  pthread_mutex_lock(&readMutex);  	

  i = 0;

  if (!(fscanf(fp, "%s", name) > 0)){
    pthread_mutex_unlock(&readMutex);  	
    return 0;
  }
  
  //printf("name: %s\n", name);
  while (ch != '\n' && ch != '\r') 
    fscanf(fp, "%c", &ch);
  
  ch = 0;
  
  while (ch != '>'){
    if (fscanf(fp, "%c", &ch) > 0){
      if (isalpha(ch))
	fosmid[i++] = ch;
    }    
    else
      break;
    
  }
  fosmid[i] = 0;
  

  pthread_mutex_unlock(&readMutex);  	

  return i;
}


int readQual(FILE *qp, int *qual, char *name){
  int i;
  char ch;
  char val[5];
  int k;

  if (qp == NULL)
    return 0;

  i = 0;
  k = 0;
  
  if (!(fscanf(qp, "%s", name) > 0)){
    return 0;
  }
  
  while (ch != '\n' && ch != '\r') 
    fscanf(qp, "%c", &ch);
  
  ch = 0;
  
  while (ch != '>'){
    if (fscanf(qp, "%c", &ch) > 0){
      if (isdigit(ch))
	val[k++] = ch;
      else if (isspace(ch)){
	val[k] = 0; k = 0;
	if (val[0]!=0)
	  qual[i++] = atoi(val);
      }
    }    
    else
      break;
    
  }
  qual[i] = -1;


  return i;
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


int isReverse(char *name){
  int i, len;
  int isNum = 0;

  if(strstr(name, "REVERSE"))
    return 1;
  if(strstr(name, "FORWARD"))
    return 0;


  len = strlen(name);
  

  for (i=0;i<len;i++){
    if (isdigit(name[i]))
      isNum = 1;
    if (isNum && name[i] == 'R')
      return 1;
    if (isNum && name[i] == 'F')
      return 0;
  }
  
  return 0;
}

void getCloneName(char *newclone, char *clone){
  int i, len, j;
  int flag = 0;

  len = strlen(clone);

  if(strstr(clone, "REVERSE") || strstr(clone, "FORWARD")){
    strcpy(newclone, clone);
    newclone[len-1-7-1] = 0;
  }


  j=0;
  for (i=0; i<strlen(clone); i++){
    if ((clone[i] != 'F' && clone[i] != 'R') || flag){
      newclone[j++] = clone[i];  
    }
    else if (clone[i] != 'F' || clone[i] != 'R'){
      if (flag)
	newclone[j++] = clone[i];  
      else{
	flag = 1;
      }
    } 
    
  }
  newclone[j] = 0;
  
}



/*

void findPositions(struct sloc *locations, char *fosmidName, int flen, FILE *out, int minwin, char *fosmid){
  struct sloc *current;
  struct sloc *this;
  struct sloc *last;
  int thisStart;
  int lastStart;
  int count;
  int threshold;
  int cnthreshold;
  int maxStart;
  int maxEnd;
  int maxCount;
  int dumpstart, dumpend;
  struct cluster *cRoot;


  
  //insertCluster(&cRoot, locations, NULL, fosmidName, out, minwin, flen);
	
  //  return;

  maxStart = 0;
  maxEnd = 0;
  maxCount = 0;

  dumpstart = dumpend = 0;

  //threshold = (int)ceil((float)(flen+WINDOW_SIZE)/IDENTITY);
  //threshold = 20*(int)ceil((float)(flen+WINDOW_SIZE));
  threshold = flen;
  cnthreshold = (int)ceil((float)(flen/WINDOW_SIZE));

  cnthreshold = cnthreshold*IDENTITY;


  thisStart = locations->start;
  this = locations;
  last = locations;
  lastStart = thisStart;


  current = locations->next;
  count = 0;
  while (current!=NULL){
#ifdef DEBUG
    printf("ts:%d\tcs:%d\tls:%d\n", thisStart, current->start, lastStart);
#endif
    if (!((thisStart > 0) ^ (current->start > 0))){
#ifdef DEBUG
      printf("i am in1 ts:%d\tcs:%d\tls:%d\n", thisStart, current->start, lastStart);
#endif
      //      if (abs(thisStart + WINDOW_SIZE - 1 - current->start) < threshold){
      if (abs(lastStart - current->start) < threshold){
	count++;
#ifdef DEBUG
	printf("i am in2 ts:%d\tcs:%d\tls:%d\tCOUNT:%d\n", thisStart, current->start, lastStart, count);
#endif
      }

      else {   // if (current->next != NULL){
	//thisStart = current->next->start;
#ifdef DEBUG
	printf("i am in3 ts:%d\tcs:%d\tls:%d\tCOUNT:%d\n", thisStart, current->start, lastStart, count);
#endif
	if (count > maxCount){
	  maxCount = count;
	  maxStart = thisStart;
	  maxEnd = lastStart;
	}

	if (count >= minwin && last!=NULL){
#ifdef DEBUG
	  printf("insert cluster here\n");
	  printf("t: %d\tl: %d\n", this->start, last->start);
#endif
	  insertCluster(&cRoot, this, last->next, fosmidName, out, minwin, flen, fosmid);
	  dumpstart = this->start;
	  ////dumpend = last->end;
	  free_cluster(cRoot);
	}
	else if (count >= minwin && last==NULL){
	  insertCluster(&cRoot, this, NULL, fosmidName, out, minwin, flen, fosmid);
	  dumpstart = this->start;
	  //dumpend = this->end;
	  free_cluster(cRoot);
	}
	this = current;
	thisStart = current->start;
	count=0;
      }
    }  
    else{

#ifdef DEBUG
      printf("i am in4 ts:%d\tcs:%d\tls:%d\tCOUNT:%d\n", thisStart, current->start, lastStart, count);
#endif
      if (count > maxCount){
	maxCount = count;
	maxStart = thisStart;
	maxEnd = lastStart;
      }

      if (count >= minwin && last!=NULL){
#ifdef DEBUG
	printf("insert cluster here2\n");
	printf("2t: %d\tl: %d\n", this->start, last->start);	
#endif
	insertCluster(&cRoot, this, last->next, fosmidName, out, minwin, flen, fosmid);
	dumpstart = this->start;
	////dumpend = last->end;
	free_cluster(cRoot);
      }
      else if (count >= minwin && last==NULL){
	insertCluster(&cRoot, this, NULL, fosmidName, out, minwin, flen, fosmid);
	dumpstart = this->start;
	//dumpend = this->end;
	free_cluster(cRoot);
      }

      this = current;
      thisStart = current->start;
      count = 0;      
    }
    last = current;
    lastStart = current->start;
    current = current->next;
  }

  
  if (dumpstart != this->start){
    if (count >= minwin && last!=NULL){
#ifdef DEBUG
      printf("insert cluster here3\n");
      printf("3t: %d\tl: %d\n", this->start, last->start);
#endif
      insertCluster(&cRoot, this, last->next, fosmidName, out, minwin, flen, fosmid);
      free_cluster(cRoot);
    }
    else if (count >= minwin && last==NULL){
      insertCluster(&cRoot, this, NULL, fosmidName, out, minwin, flen, fosmid);
      free_cluster(cRoot);
    }
    
  }

}

*/


/* cluster stuff */

/*
void alloc_cmember(struct cmember **cm){
  (*cm) = (struct cmember *) mymalloc (sizeof(struct cmember));
  (*cm)->next = NULL;
  //(*cm)->start=0;
  (*cm)->windowID = -1;
  callocs++;
}
*/

/*

void insert_cmember(struct sloc **cl, int windowID){
  //struct cmember *cm2;
  struct cmember *new;

  if ((*cl)->windows == NULL){
    alloc_cmember(&((*cl)->windows));
    (*cl)->windows->windowID = windowID;
    return;
  }
  
  alloc_cmember(&new);
  new->windowID = windowID;
  new->next = (*cl)->windows;
  (*cl)->windows = new;


}

*/




/*
void alloc_cluster(struct cluster **cl){
  (*cl) = (struct cluster *) mymalloc (sizeof(struct cluster));
  (*cl)->members = NULL;
  (*cl)->next = NULL;
  (*cl)->nmember = 0;
  clallocs++;
}

*/

/*
void insert_cmember_to_cluster(struct cluster **cl, int start, int windowID){

  struct cmember *new;

  if ((*cl)->members == NULL){
    alloc_cmember(&((*cl)->members));
    (*cl)->members->windowID = windowID;
    ((*cl)->nmember)++;
    (*cl)->start = start;
#ifdef DEBUG
  printf("Creating newstart %d newwin %d\tcnt %d\n", start, windowID,((*cl)->nmember));
#endif
    return;
  }
  
#ifdef DEBUG
  printf("Adding newstart %d newwin %d\tinfrontof\tstart %d win %d cnt %d\n", start, windowID, (*cl)->start, (*cl)->members->windowID, ((*cl)->nmember+1));
#endif
  alloc_cmember(&new);
  new->windowID = windowID;
  new->next = (*cl)->members;
  (*cl)->members = new;
  (*cl)->start = start;

  ((*cl)->nmember)++;

}
*/


/*
void insertCluster(struct cluster **cRoot, struct sloc *newloc, struct sloc *endloc, char *fosmidName, FILE *out, int minwin, int qflen, char *fosmid){
  struct cluster *current;
  struct cluster *bestfit;
  struct cluster *prev;
  struct cluster *new;
  struct sloc *loc;
  struct sloc *ploc;
  //struct sloc *nloc;
  struct cmember *windows;
  int maxWin; 
  int maxStart;
  int maxEnd;
  int maxWinStart, maxWinEnd;
  //int willInsert;
  int thisend, thisstart, thiswinstart, thiswinend;
  int thishitlen, thischrlen;
  int thisupratio, thisbottomratio;
  int thiswin;
  int mindist;
  int isShortOK;
  int wincnt;
  int expectedwin;
  
  loc = newloc;
  ploc = loc;
  maxWin = 0;


  /* newloc is already sorted 

  if (loc == endloc)
    return;

  alloc_cluster(cRoot);
  windows = loc->windows;
  if (windows==NULL){
    printf("woa?\n");
    exit(0);
  }
  insert_cmember_to_cluster(cRoot, loc->start, windows->windowID);
  (*cRoot)->end = loc->start;
  (*cRoot)->startWin = windows->windowID;
  windows = windows->next;
  current = (*cRoot);
  while (windows != NULL){
    alloc_cluster(&new);
    new->end = loc->start; 
    new->startWin = windows->windowID;
    insert_cmember_to_cluster(&new, loc->start, windows->windowID);
    current->next = new;
    current = current->next;
    windows = windows->next;
  }

  ploc = loc;
  loc = loc->next;

  // *cRoot is not NULL


  while(loc != endloc){
    /*

    willInsert = 1;
    nloc = loc->next;

    if (nloc != endloc){ // look forward
      if (abs(loc->start - nloc->start) == WINDOW_SIZE)
	willInsert = 1;
    }
    
    if (abs(loc->start - ploc->start) == WINDOW_SIZE)
      willInsert = 1;

    //printf("for %d wi: %d\n", loc->start, willInsert);


    if (willInsert){
    
   
    windows = loc->windows;
    while (windows != NULL){
      current = (*cRoot);
      bestfit = NULL;
      mindist = 1000000;
      wincnt = 0;

      while (current != NULL){
	/* 
	     numbers are in descending order
	     therefore
	     current->start should be greater than loc->start
	     and
	     current->members->windowID should be greater than
	     windows->windowID
	
	if (((current->members->windowID - windows->windowID) >= WINDOW_SIZE-1)  &&  
	    //((current->members->windowID - windows->windowID) <= 5*WINDOW_SIZE ) &&  
	    ((current->start - loc->start) >= WINDOW_SIZE-1) //) {
	    &&    (abs((current->start - loc->start)-(current->members->windowID - windows->windowID)) <= WINDOW_SIZE )  ){
	  if (mindist > (current->start - loc->start) && current->nmember > wincnt){
	    mindist = current->start - loc->start;
	    bestfit = current;
	  }
	  
	  if (current->nmember > wincnt && (current->start - loc->start)>=WINDOW_SIZE-1){
	    mindist = current->start - loc->start;
	    bestfit = current;
	    wincnt = current->nmember;
	    break;
	  }
	}
	prev = current;
	current = current->next;
      }
      //((current->start - loc->start) <= 5*WINDOW_SIZE)){
      if (bestfit != NULL){
	insert_cmember_to_cluster(&bestfit, loc->start, windows->windowID);
      }
    

      if (bestfit == NULL){
#ifdef DEBUG
	printf("create new for %d\n", loc->start);
#endif
	alloc_cluster(&new);
	new->end = loc->start;
	new->startWin = windows->windowID;
	insert_cmember_to_cluster(&new, loc->start, windows->windowID);
	prev->next = new;
      }
      windows = windows->next;
    }
    // }
    ploc = loc;
    loc = loc->next;
  }


  maxWin = -1;
  current = (*cRoot);
  while (current != NULL){
    thisend = current->end;
    thisstart = current->start;
    thiswinend = current->startWin + WINDOW_SIZE - 1;
    thiswinstart = current->members->windowID;
    thishitlen = abs(thiswinend - thiswinstart) + 1 ;
    thischrlen = abs(thisend - thisstart) + 1;
    thisupratio =  (int)ceil((float)(thischrlen)/IDENTITY);
    thisbottomratio =  (int)ceil((float)(thischrlen)*IDENTITY);
    thiswin = current->nmember;
    expectedwin = thishitlen/WINDOW_SIZE;

    isShortOK = 1;

    if (thiswin < (int)ceil((float)(qflen/WINDOW_SIZE)) / 4){
      
     if (thiswin*WINDOW_SIZE != thishitlen){
	isShortOK = 0;
     }
     /* if (thischrlen != thishitlen){
	isShortOK = 0;
	}
    }
    
    if (current->nmember >= maxWin && 
	((float)(expectedwin-thiswin)/(float)thishitlen) < (1-IDENTITY) &&
	//isShortOK &&
	//thiswin >= (thishitlen/WINDOW_SIZE)*IDENTITY &&
	//thishitlen >= (thiswin*WINDOW_SIZE)*IDENTITY &&	
	thishitlen <= thisupratio && 
	thishitlen >= thisbottomratio){
	maxWin = current->nmember;
	maxEnd  = current->end;
	maxStart = current->start;
	maxWinEnd = current->startWin + WINDOW_SIZE - 1;
	maxWinStart = current->members->windowID;
	//}	
    }

    /*
	pthread_mutex_lock(&logMutex);  	
	
	if (maxStart > 0 && maxEnd > 0) 
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tF\t%s\n", fosmidName, maxStart, maxEnd+WINDOW_SIZE-1, maxWinStart, maxWinEnd, maxWin, DBNAME);
	else if (maxStart < 0 && maxEnd < 0) 
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tR\t%s\n", fosmidName, (-1*maxEnd), (-1*maxStart+WINDOW_SIZE-1), maxWinStart, maxWinEnd, maxWin, DBNAME);
	
	pthread_mutex_unlock(&logMutex);  	
    

    current = current->next;
  }

  /*
  printf("bee %d %d\n", maxStart, maxEnd);
  printf("maxwin %d\n", maxWin);
  

  
  if (maxWin < minwin)
    return;

  current = (*cRoot);

  //while (current != NULL){


  pthread_mutex_lock(&logMutex);  	
  
  
  if (maxStart > 0 && maxEnd > 0) {
    fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tF\t%s\t", fosmidName, maxStart, maxEnd+WINDOW_SIZE-1, maxWinStart, maxWinEnd, maxWin, DBNAME);
    /*printSeq(out, fosmid, maxWinStart, maxWinEnd);
    fprintf(out, "\t");
    printSeq(out, genome, maxStart-INDEXSTART, maxEnd+WINDOW_SIZE-1-INDEXSTART);
    fprintf(out, "\n");
  }
  else if (maxStart < 0 && maxEnd < 0) {
    fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tR\t%s\t", fosmidName, (-1*maxEnd), (-1*maxStart+WINDOW_SIZE-1), maxWinStart, maxWinEnd, maxWin, DBNAME);
    /*    printSeq(out, fosmid, maxWinStart, maxWinEnd);
    fprintf(out, "\t");
    printSeq(out, genome, (-1*maxEnd-INDEXSTART), (-1*maxStart+WINDOW_SIZE-1-INDEXSTART));
    fprintf(out, "\n");
  }

  pthread_mutex_unlock(&logMutex);  	

    // current = current->next;  
    // }
}
    */

/*

float align(char *S, char *T){
  int sSize = strlen(S);
  int tSize = strlen(T);
  int i,j,k;
  int ii, jj;
  int larger=sSize;
  float Wg, Ws; // gap penalty , space penalty
  char Sp[FOSMID_LENGTH], Tp[FOSMID_LENGTH];
  float V[FOSMID_LENGTH][FOSMID_LENGTH];
  float F[FOSMID_LENGTH][FOSMID_LENGTH];
  float E[FOSMID_LENGTH][FOSMID_LENGTH];
  float G[FOSMID_LENGTH][FOSMID_LENGTH];
  int   P[FOSMID_LENGTH][FOSMID_LENGTH];
  int mismatch;

  if (tSize > larger)
    larger = tSize;
  Wg=12; Ws=2;


  V[0][0] = 0;
  P[0][0] = 0;
  E[0][0] = 0;
  F[0][0] = 0;
  G[0][0] = 0;
  for (i=1;i<=sSize;i++){
    V[i][0] = 0 - Wg - i*Ws;
    //V[i][0] = 0;
    E[i][0] = -1000000; /* -infinity 
    P[i][0] = 1;
  }
  for (j=1;j<=tSize;j++){
    V[0][j] = 0 - Wg - j*Ws;
    //V[0][j] = 0;
    F[0][j] = -1000000; /* -infinity 
    P[0][j] = 0;
  }

  
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      G[i][j] = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
      if (V[i][j] == F[i][j])
	P[i][j] = 1; // match S[i]-"-"
      else if (V[i][j] == E[i][j])
	P[i][j] = 0; // match T[i]-"-"
      else
	P[i][j] = 2; // match S[i]-T[j]
    }
  


  /* vectors ready, set, go! 

  

  Tp[0]=0;
  Sp[0]=0;
  ii=0; jj=0;
  i = sSize; j = tSize;
  while (!(i==0 && j==0)){
    if (P[i][j] == 2){ // diagonal: match S[i]-T[j] 
      //while (i>0 && j>0 && V[i][j] == G[i][j]){ 
      Tp[jj++] = T[--j];
      Sp[ii++] = S[--i];
	//}
    } // if
    else if (P[i][j] == 1){ // toUp: match S[i]-"-"
      //while (i>0 && j>0 && V[i][j] == F[i][j]){
	Tp[jj++] = '-';
	Sp[ii++] = S[--i];      
	//     }
    } // else if   
    else { // toLeft: match T[j]-"-"
      //while (i>0 && j>0 && V[i][j] == E[i][j]){
	Tp[jj++] = T[--j];
	Sp[ii++] = '-';
	//}
    } // else
    if (i==0){
      for (k=0;k<j;k++){
	Sp[ii++] = '-';
      }
      for (k=j;k>0;k--){
	Tp[jj++] = T[k-1];
      }
      break;
    } // if i==0
    if (j==0){
      for (k=0;k<i;k++){
	Tp[jj++] = '-';
      }
      for (k=i;k>0;k--){
	Sp[ii++] = S[k-1];
      }
      break;
    } // if j==0
    //ii++; jj++;
  } // while
  Sp[ii] = 0;
  Tp[jj] = 0;

  //ii and jj should be the same

  mismatch=0;
  for (i=0;i<ii;i++){
    if (Sp[i]!='-' && Tp[i]!='-' && Tp[i]!=Sp[i])
      mismatch++;
  }

  return ((float)mismatch / (float)ii);

} // align

  */

/*
float max3(float a, float b, float c){
  if (a>=b){
    if (a>=c)
      return a;
    else
      return c;
  }
  else{
    if (b>=c)
      return b;
    else
      return c;
  } 
} // max3

float max(float a, float b){
  if (a>b)
    return a;
  return b;
} // max


float sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t) && s!='-')
    return 5;
  else
    return -1;
} // G

 
void printSeq(FILE *out, char *seq, int start, int end){
  int i;
  for (i=start-1; i < end; i++)
    fprintf(out, "%c", seq[i]);
}

*/
