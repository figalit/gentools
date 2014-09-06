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
#include <assert.h>

#define NODRAW

#define SEQ_LENGTH 200
#define FILENAME_LENGTH 20
#define FOSMID_LENGTH 5500
#define IDENTITY 0.90

// ONUR: For compression
#define BITS_PER_ENCODING_DATA_TYPE 8
#define BITS_TO_ENCODE_BASE 2
#define BASES_PER_ENCODING_UNIT (BITS_PER_ENCODING_DATA_TYPE/BITS_TO_ENCODE_BASE)

//#define DEBUG

typedef struct cmember{
  //int start; // location start
  int windowID;
  struct cmember *next;
}scmember;


typedef struct sloc{
  int start; // start index
  /* if start index is negative, then that window is
     reverse-complemented */
  struct sloc *next;
  //int fromWindow;
  struct cmember *windows;
}slocation;


typedef struct trie{
  struct sloc *location; 
#ifndef NODRAW
  int pid;
  int id;
#endif
  struct trie *kids[4]; //children nodes
}strie;



typedef struct cluster{
  int nmember; // no of members
  int start;
  int end;
  int startWin;
  struct cmember *members;
  struct cluster *next;
}scluster;


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
int TEXTFORMAT = 0;

int genomelen;

void help(char *);
int readFosmid(FILE *, char *, char *, FILE *, int *);
int readQual(FILE *, int *, char *);
char *readFasta(FILE *, int *);
int getWindow(char *, int, char **, int, int *);
int getIndexWindow(FILE *, char **, int *);
int cindex(char);
char reverseCindex(int);
void alloc_trie(struct trie **);
void insert(struct trie *, char *, int, char *);
void drawTrie(FILE *, struct trie *, char);
void saveTrie(struct trie *, char *);
void saveIndex(struct trie *, FILE *,char *);
void loadTrie(char *, struct trie **);
void dumpLocation(struct trie *, FILE *, unsigned int);
void concatLocations(struct sloc **, struct sloc *, int);
void freeLocation(struct sloc *);
void findPositions(struct sloc *, char *, int, FILE *, int, char *);
void do_search(struct trie *, char *, char *);
FILE *myfopen(char *, char *);
void rcomp(char *, char *);
int search(struct trie *, char *, struct sloc **, int);
void insert_sloc(struct sloc **, int, char *);
int do_file(char *, struct trie *);
struct sloc *copyLocation(struct sloc *, int);
void *mymalloc(size_t);
void freeTrie(struct trie *);
void printSeq(FILE *, char *, int, int);


/* cluster stuff from pimp.c */

void alloc_cmember(struct cmember **);
void insert_cmember(struct sloc **, int);
void free_cmember(struct cmember *);
void alloc_cluster(struct cluster **cl);
void free_cluster(struct cluster *cl);
void insert_cmember_to_cluster(struct cluster **, int, int);
void insertCluster(struct cluster **, struct sloc *, struct sloc *, char *, FILE *, int, int, char *);

void *threadSearch(void *);

/* alignment stuff from pimp.c */
float sig(char s, char t); // character similarity score
float max3(float, float, float);
float max(float, float);
float align(char *, char *);

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
char OUTFOLDER[SEQ_LENGTH];

char QUALFILE[SEQ_LENGTH];

//char *genome;
char GENOME[SEQ_LENGTH];
int glen; // length of the genome


pthread_mutex_t  logMutex;
pthread_mutex_t  readMutex;

int main(int argc, char **argv){

  int i;
  //FILE *fasta;
  FILE *treefile;
  struct trie *root;
  char fname[SEQ_LENGTH*2];
  char idxname[SEQ_LENGTH];
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
  build_start = 0;
  build_end = 0;
  build_cnt = 0;
  searchFile[0] = 0;
  DBNAME[0] = 0;
  GENOME[0] = 0;
  QUALFILE[0] = 0;
  OUTFOLDER[0] = 0;

  pthread_mutex_init(&logMutex, NULL);
  pthread_mutex_init(&readMutex, NULL);

  INDEXSTART = 0;
  seqname[0] = 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-ws"))
      WINDOW_SIZE = atoi(argv[i+1]);
    /*
    else if (!strcmp(argv[i], "-ss"))
      deprecated
      SLIDE_SIZE = atoi(argv[i+1]);
      
    */
    else if (!strcmp(argv[i], "-dumptree"))
      DUMPTREE=1;
    else if (!strcmp(argv[i], "-text"))
      TEXTFORMAT=1;
    else if (!strcmp(argv[i], "-v"))
      VERBOSE=1;	
    else if (!strcmp(argv[i], "-dir"))
      strcpy(startdir, argv[i+1]);	
    else if (!strcmp(argv[i], "-re"))
      strcpy(regexpr, argv[i+1]);	
    else if (!strcmp(argv[i], "-f"))
      strcpy(fname, argv[i+1]);	
    else if (!strcmp(argv[i], "-saveindex"))
      SAVEINDEX = 1;
    else if (!strcmp(argv[i], "-loadindex")){
      LOADED = 1;
      strcpy(idxname, argv[i+1]);
    }
    else if (!strcmp(argv[i], "-qualfile"))
      strcpy(QUALFILE, argv[i+1]);
    else if (!strcmp(argv[i], "-start"))
      build_start = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-end"))
      build_end = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(searchFile, argv[i+1]);
    else if (!strcmp(argv[i], "-minwin")){
      MINWIN = atoi(argv[i+1]);
    }
    else if (!strcmp(argv[i], "-threads"))
      THREADS = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-istart"))
      INDEXSTART = atoi(argv[i+1]) - 1;
    else if (!strcmp(argv[i], "-seqname"))
      strcpy(seqname, argv[i+1]);
    else if (!strcmp(argv[i], "-genome"))
      strcpy(GENOME, argv[i+1]);
    else if (!strcmp(argv[i], "-outfolder"))
      strcpy(OUTFOLDER, argv[i+1]);

  }

  if (MINWIN <= 0){
    fprintf(stderr, "Strange MINWIN=%d\n", MINWIN);
    return 0;
  }

  if (INDEXSTART < 0){
    fprintf(stderr, "Strange INDEXSTART=%d\n", INDEXSTART);
    return 0;
  }

  if ((build_start == 0) ^ (build_end == 0)){
    fprintf(stderr, "Strange build_start and build_end.\n");
    return 0;
  }

  if (THREADS < 1){
    fprintf(stderr, "Threads should be >= 1\n");
    return 0;
  }
    
  
  if (SAVEINDEX && LOADED){
    fprintf(stderr, "Either save the index or load it.\n");
    return 0;
  }


  if (!LOADED){
    if (fname[0] && startdir[0]){
      help(argv[0]);
      return 0;
    }

    alloc_trie(&root);

    if (fname[0] != 0)
      fprintf(stdout,"\tfile:\t\t%s\n",fname);
    if (startdir[0] != 0)
      fprintf(stdout,"\tdir:\t\t%s\n",startdir);
    fprintf(stdout,"\twindow size:\t%d\n\tslide size:\t%d\n", WINDOW_SIZE, SLIDE_SIZE);
    if (VERBOSE)
      fprintf(stdout,"\tverbose:\t\ton\n");
    if (DUMPTREE)
      fprintf(stdout,"\tdumptrie:\t\ton\n");
    if (TEXTFORMAT)
      fprintf(stdout,"\tinput/output in text format:\t\tyes\n");
    if (regexpr[0] != 0)
      fprintf(stdout,"\tregexp:\t\t%s\n",regexpr);
    if (build_start != 0)
      fprintf(stdout,"\tbuild_start:\t%d\n\tbuild_end:\t%d\n",build_start, build_end);
    
    /*
    if (regcomp(&preg, regexpr, 0) != 0) {
      printf("cant compile regular expression\n");
      return 0;     
    }*/

    /*
      status = regexec(&preg, argv[2], (size_t) 0, NULL, 0);
      printf("%d\n", status);
    */

    /*  
    if (startdir[0] != 0){
      dp=opendir(startdir);
      while((ep=readdir(dp))){
	//printf("traverse\n");
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
	filestat = do_file(fname, root);
	if (filestat == 0)
	  return 0;
      }
      closedir(dp);
    }
    else */

    if (fname[0]!=0){
      filestat = do_file(fname, root);
      if (filestat == 0)
	return 0;
    }

  }

  else{
    gettimeofday(&start, &tz);
    loadTrie(idxname, &root);
    gettimeofday(&end, &tz);
    fprintf(stdout, "load time %d microseconds\n", ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec)));
  }

  /*
  if (GENOME[0] != 0){
    fasta = myfopen(fname, "r");    
    genome = readFasta(fasta, &glen);  
    }*/

  if (WINDOW_SIZE == 0){ // deprecated: || SLIDE_SIZE == 0 || SLIDE_SIZE>WINDOW_SIZE){
    fprintf(stderr, "Strange ws.\n");
    return 0;
  }



  if (DUMPTREE){
    treefile = myfopen("searchtrie.viz", "w");
    fprintf(treefile, "digraph G {\n\tsize=\"8,11\";\n");
    drawTrie(treefile, root, '0');
    fprintf(treefile, "}\n");
    fclose(treefile);
  }

  if (SAVEINDEX){
    if (LOADED)
      strcpy(fname, "preloaded");
    if (build_start!=0 && build_end!=0){
      sprintf(fname, "%s.%d-%d", startdir, build_start, build_end);
      startdir[0] = 0;
    }
    if (fname[0] != 0 && startdir[0] == 0)
      saveTrie(root, fname);
    else if (startdir[0] != 0)
      saveTrie(root, startdir);      
  }

  fprintf(stdout, "Total memory for the trie: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", triemem, (float)triemem/1024.0, (float)triemem/1024.0/1024.0);
  fprintf(stdout, "Total memory for the locations: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
  
  slocmem = 0;

  allocs = 0;
  frees = 0;
  callocs = 0;
  cfrees = 0;
  clallocs = 0;
  clfrees = 0;

  if (searchFile[0] != 0)
    fprintf(stdout,"\tsearch:\t\t%s\n",searchFile);
  if (THREADS != 1)
    fprintf(stdout,"\tmultithread:\t\t%d\n",THREADS);
  if (INDEXSTART != 0)
    fprintf(stdout,"\tindexstart:\t\t%d\n",INDEXSTART);
  if (seqname[0] != 0)
    fprintf(stdout,"\tseqname:\t\t%s\n",seqname);
  

  if (seqname[0]!=0)
    strcpy(DBNAME, seqname);

  if (searchFile[0] != 0)
    do_search(root, searchFile, QUALFILE);
  
  /*
  fprintf(stderr, "Total memory for the locations afterwards: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
  fprintf(stderr, "Allocs %d\nFrees %d\n", allocs, frees);
  fprintf(stderr, "cAllocs %d\ncFrees %d\n", callocs, cfrees);
  fprintf(stderr, "clAllocs %d\nclFrees %d\n", clallocs, clfrees);
  */
  
  //freeTrie(root); 

  /*
  fprintf(stderr, "Free trie.\n");
  gettimeofday(&start, &tz);


  gettimeofday(&end, &tz);
  fprintf(stderr, "free time %d microseconds\n", ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec)));
  

  fprintf(stderr, "Total memory for the trie: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", triemem, (float)triemem/1024.0, (float)triemem/1024.0/1024.0);
  fprintf(stderr, "Total memory for the locations: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
  */

  return 1;
}



void help(char *pname){
  //fprintf(stderr, "%s <-f [fastafile] OR -dir [startdir]> -ws [window_size] -ss [slide_size] <-dumptree> <-v>\n", pname);
  fprintf(stdout, "\n%s <options>\n", pname);
  fprintf(stdout, "\nINDEXING OPTIONS:\n\n");
  fprintf(stdout, "-f [fastafile]\t:\tInput fastafile for trie indexing.\n");
  fprintf(stdout, "-dir [startdir]\t:\tInput all files in directory startdir for trie indexing.\n");
  fprintf(stdout, "-start [start]\t:\tWhen indexing with -dir option, start from file #start.\n");
  fprintf(stdout, "-end [end]\t:\tWhen indexing with -dir option, stop at file #end.\n");
  fprintf(stdout, "-re [reg_exp]\t:\tWhen indexing with -dir option, index files with names that match to reg_exp only.\n");
  fprintf(stdout, "-ws [win_size]\t:\tSet window size for trie indexing.\n");
#ifndef NODRAW
  fprintf(stdout, "-dumptree\t:\tDump tree in GRAPHVIZ format.\n");
#endif
  fprintf(stdout, "-saveindex\t:\tSave index in a file.\n");
  fprintf(stdout, "\nSEARCHING OPTIONS:\n\n");
  fprintf(stdout, "-loadindex [inf]:\tLoad index from [inf].\n");
  fprintf(stdout, "-qualfile [quff]:\tLoad quality values from [quf]\n");
  fprintf(stdout, "-threads [n_th]\t:\tUse multithread version with n_th number of threads.\n");
  fprintf(stdout, "-search [sfile]\t:\tSearch fosmids in file sfile.\n");
  fprintf(stdout, "-istart [istart]:\tUpdate loaded indexes by adding istart-1; use for contigs.\n");
  fprintf(stdout, "-seqname [sname]:\tUpdate sequence name with sname; use for contigs.\n");
  fprintf(stdout, "\nCOMMON OPTIONS:\n\n");
  fprintf(stdout, "-v\t\t:\tVerbose.\n\n\n");
}

char *readFasta(FILE *fastaFile, int *length){

  /* there is supposed to be only one long long long sequence */

  int cnt;
  char ch; 
  int i;
  int seqlen=0;
  char dummy[SEQ_LENGTH];
  char *sequence;

  fprintf(stdout, "Checking length.\n");
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      fgets(dummy, SEQ_LENGTH, fastaFile);
    }
    else if (!isspace(ch))
      seqlen++;
  }

  cnt = 0; i=0;

  fprintf(stdout, "Allocating memory for sequence with length %d.\n", seqlen);
  *length = seqlen;
  
  sequence = (char *) mymalloc((seqlen+1) * sizeof(char));
  
  sequence[0] = 0;

  fprintf(stdout, "Reading sequence.\n");

  rewind(fastaFile);

  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      fgets(dummy, SEQ_LENGTH, fastaFile);
    else if (isalpha(ch))
      sequence[i++] = ch;
  }

  sequence[i] = 0;
  fprintf(stdout, "\n[OK] sequence is read from fasta file.\n");

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

int getWindow(char *genome, int glen, char **window, int win_start, int *skipN){
  int i;
  int win_end;
  win_end = win_start+WINDOW_SIZE;
  if (win_end>=glen)
    win_end = glen;
  for (i=win_start;i<win_end;i++){
    if (genome[i] == 'N' || genome[i] == 'O'){
      *skipN = i;
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

char reverseCindex(int code){
  switch(code){
  case 0:
    return 'A';
    break;
  case 1:
    return 'C';
    break;
  case 2:
    return 'G';
    break;
  case 3:
    return 'T';
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
  (*t)->location = NULL;
#ifndef NODRAW
  (*t)->id = identifier++;
#endif
  (*t)->kids[0] = NULL;
  (*t)->kids[1] = NULL;
  (*t)->kids[2] = NULL;
  (*t)->kids[3] = NULL;
}

void insert(struct trie *root, char *window, int start, char *seqname){
  struct trie *current = root;
  int i;
#ifndef NODRAW
  int pid;
#endif
  int loc;

  for (i=0;i<strlen(window);i++){
    loc = cindex(window[i]);
#ifndef NODRAW
    pid = current->id;
#endif
    if (current->kids[loc] == NULL){
      if (VERBOSE)
	printf("alloc for %c - %d\n", window[i], loc);
      alloc_trie(&(current->kids[loc]));
    }
    current = current->kids[loc];
#ifndef NODRAW
    current->pid = pid;
#endif
  }
  // we're at the leaf now; fix this for multiple hits.
  insert_sloc(&(current->location), start, seqname);
  //strcpy(current->seq, seqname);
  if (VERBOSE)
    printf("inserted %d\n", start);
  //current->location = start;
}


//int search(struct trie *root, char *window, int *location, char **seqname){
int search(struct trie *root, char *window, struct sloc **location, int windowID){
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
 

  /*
  *location = current->location;
  strcpy((*seqname), current->seq);
  */
  //(*location) = copyLocation(current->location, windowID);
  concatLocations(location, current->location, windowID);
  //(*location) = current->location;
  return 1;
  
}

void drawTrie(FILE *treefile, struct trie *root, char ch){
#ifndef NODRAW
  struct trie *current=root;
  if (current != NULL){
    fprintf(treefile, "\t%d [shape=circle, label=\"\", style=filled, height=0.1, width=0.1, color=black, fontsize=6];\n", current->id );  
    if (ch != '0')
      fprintf(treefile, "%d->%d [ label= \"%c\"];\n", current->pid, current->id, ch);
    drawTrie(treefile, current->kids[0], 'A');
    drawTrie(treefile, current->kids[1], 'C');
    drawTrie(treefile, current->kids[2], 'G');
    drawTrie(treefile, current->kids[3], 'T');
  }
#endif
}

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

void insert_sloc(struct sloc **location, int start, char *seqname){

  /* 
     NOTE: since this insertion is implemented as stack (ah, beauty of O(1))
     and start locations are 1 through length(chromosome)
     these numbers are always kept reverse sorted (descending)
     in ABSOLUTE VALUE (reverse-complements dude)
  */

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


int do_file(char *fname, struct trie *root){

  FILE *fasta;
  //char *genome;
  //int glen;
  int i;
  char *window;
  char *rcwindow;
  int willInsert;
  char dummy[SEQ_LENGTH];

  willInsert = 1;

  
  fprintf(stdout, "\n\nInserting sequence from file %s\n", fname);
  fasta = myfopen(fname, "r");
  fgets(dummy, SEQ_LENGTH, fasta);

  //genome = readFasta(fasta, &glen);  //for only one 400k right now
  
  window = (char *) mymalloc(sizeof(char) * WINDOW_SIZE+1);
  rcwindow = (char *) mymalloc(sizeof(char) * WINDOW_SIZE+1);
  
  i = 0;
  while (getIndexWindow(fasta, &window, &willInsert) > 0){
    //fprintf(stderr, "\rGetting windows from %s\t: %d%%", fname, ((int)(100.0*(float)(i+strlen(window))/(float)strlen(genome))));
    if (VERBOSE)
      printf("%s\n", window);
    /* insert the window to the trie here */
    if (willInsert){
      // start location is i+1
      // because i starts from 0 here; but 
      // we want the sequence index starting from 1
      // STARTING POSITION FOR FORWARD
      insert(root, window, (i+1), fname);      
      // reverse complement of this window is indexed by its negative
      // value ENDING POSITION FOR REVERSSE COMPLEMENT
      // #0: WE DONT NEED TO INDEX REVERSE COMPLEMENTS WHEN CREATING THE INDEX
      //rcomp(window, rcwindow);
      //insert(root, rcwindow, (-1*(i+1)), fname);      
    }
    i+=WINDOW_SIZE;
  }
  fprintf(stdout, "\n");
  
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
  
  fprintf(stdout, "Saving Trie...\n");

  indexFile = myfopen(fname, "w");

  fprintf(indexFile, "WS %d\tSS %d %s %d\n", WINDOW_SIZE, SLIDE_SIZE, fprefix, genomelen);
  // ONUR FIXME - write the header in binary?
  
  window[0] = 0;
  saveIndex(root, indexFile, window);

  fclose(indexFile);
  
}

void saveIndex(struct trie *root, FILE *fp, char *window){
  struct trie *current;
  struct sloc *location;
  unsigned char newWindow[WINDOW_SIZE];
  unsigned int location_count;
  current = root;
  if (current == NULL)
    return;
  if (current->location != NULL){ // leaf node
    // 1. convert window to char's
    // 2. store chars

    location = current->location;
    location_count = 0;
    while (location != NULL){
      if (location->start > 0)
	location_count++;
	// TODO #0. REMOVE NEGATIVES FROM WRITE
                // DON'T FORGET TO FIX READING THIS
      
      location = location->next;
    }

    if (location_count == 0)
      return;

    if (!TEXTFORMAT) {
      // -------- ONUR: Begin - Encode and write to file
      int i=0;
      unsigned char encoded_window = 0;
      int shift_amount = 0;
      
      while (window[i] != '\0') {
	shift_amount = (BASES_PER_ENCODING_UNIT - 1 - (i % BASES_PER_ENCODING_UNIT))*2;
	encoded_window |= ((unsigned char)cindex(window[i]) << shift_amount);
	if (VERBOSE)
	  fprintf(stdout, "i: %d  shamt: %d   %c \n", i, shift_amount, window[i]); 
	if (shift_amount == 0) {
	  if (VERBOSE)
	    fprintf(stdout, "i: %d  encoded_window: %x\n\n", i, encoded_window);
	  //fprintf(fp, "%x ", encoded_window);
	  //fputc(encoded_window, fp);
	  fwrite(&encoded_window, sizeof(encoded_window), 1, fp);
	  encoded_window = 0;
	}
	i++;
      }
      if (shift_amount != 0) {
	if (VERBOSE)
	  fprintf(stdout, "i: %d  encoded_window: %x\n\n", i, encoded_window);
	//fprintf(fp, "%x\t", encoded_window);
	//fputc(encoded_window, fp);
	fwrite(&encoded_window, sizeof(encoded_window), 1, fp);
      }
      // -------- ONUR: End - Encode and write to file
      
      dumpLocation(current, fp, location_count);
    } else {
      fprintf(fp, "%s\t|\t", window);
      dumpLocation(current, fp, location_count);
      fprintf(fp, "\t!\n");
    }
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

void dumpLocation(struct trie *root, FILE *fp, unsigned int location_count){
  struct sloc *location;
  char seq[SEQ_LENGTH];
  //unsigned int location_count = 0;
  unsigned char location_count_encoded;

  location = root->location;
  seq[0] = 0;

  if (!TEXTFORMAT) {
    // ONUR: Find location count
    /*
    while (location != NULL){
    if (location->start > 0)
    location_count++;
    // TODO #0. REMOVE NEGATIVES FROM WRITE
    // DON'T FORGET TO FIX READING THIS
    
    location = location->next;
      }*/

    // ONUR: Decided not printing location count until we understand its profile --- fwriting as int increases the file size by 30%!!! 
    // CAN: BIR INTEGER BU YA NASIL 30% ARTTIRIYOR?
    // ONUR: Print location count
    // fprintf(fp,"%d ", location_count);
    // ONUR: FIXME: Location count'u binary olarak yazmak her zaman iyi degil... Ozellikle location count genelde kucuk sayi ise!!! Bunu profile ederek bir sekilde halledebiliriz. 
    // Veya ayrica bir byte'ta location'in kac byte gerektirdigini store edip ona gore variable-length encode edebiliriz. Bunu iyice anlamak lazim!
    //fwrite(&location_count, sizeof(location_count), 1, fp);
    
    // Trick with location count -- need profiling info here!!! Maybe we should construct a coding table
    if (location_count <= 250) {
      assert(location_count != 0);
      location_count_encoded = location_count;
      fwrite(&location_count_encoded, sizeof(location_count_encoded), 1, fp);
    } else {
      location_count_encoded = 0;
      fwrite(&location_count_encoded, sizeof(location_count_encoded), 1, fp);
      fwrite(&location_count, sizeof(location_count), 1, fp);
    }
  }
  
  location = root->location;
  while (location != NULL){
    if (location->start > 0){
      // TODO #0. REMOVE NEGATIVES FROM WRITE
                // DON'T FORGET TO FIX READING THIS
      if (TEXTFORMAT)
	fprintf(fp,"%d\t", location->start);
      else {
	// ONUR: Print location in binary format
	fwrite(&location->start, sizeof(location->start), 1, fp);
      }
    }
    location = location->next;
  }
}

struct sloc *copyLocation(struct sloc *source, int windowID){
  struct sloc *new;
  struct sloc *current;
  struct sloc *ret;
  struct sloc *this;


  if (source == NULL)
    return NULL;

  /* 
     NOTE: locations are already reverse-sorted in descending order
  */

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



void concatLocations(struct sloc **s1, struct sloc *s2, int windowID){
  struct sloc *new;
  struct sloc *s1t, *s2t;
  struct sloc *s1p;

  /* inputs are sorted IN DESCENDING ORDER */

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

void free_cmember(struct cmember *cm){
  struct cmember *tmp;
  while (cm != NULL){
    tmp = cm->next;
    free(cm);
    cfrees++;
    cm = tmp;
  }
}


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




void loadTrie(char *fname, struct trie **root){
  FILE *fp;
  FILE *fp_sanity;
  int ws, ss;
  char dummy[SEQ_LENGTH];
  char dummy2[SEQ_LENGTH];
  char name[SEQ_LENGTH];
  char *window;
  char *rcwindow;
  char *encoded_window;
  int start;
  int i,j;
  int read_bytes;
  unsigned char location_count_encoded;
  unsigned int location_count;
  int encoded_window_bytes;
  
  fp = myfopen(fname, "r");
  fprintf(stdout, "Loading %s\n", fname);
  
  alloc_trie(root);
  
  // read the header
  fscanf(fp, "%s %d %s %d %s %d\n", dummy, &ws, dummy2, &ss, DBNAME, &genomelen);
  //fprintf(stdout, "%d %d %s %d\n", ws, ss, DBNAME, genomelen);

  // genome = (char *) mymalloc(sizeof(char) * genomelen+1);

  strcpy(DBNAME2, DBNAME);
  WINDOW_SIZE = ws;
  SLIDE_SIZE = ss;

  window = (char *) mymalloc(sizeof(char) * (ws+1));
  rcwindow = (char *) mymalloc(sizeof(char) * (ws+1));
  name[0] = 0;
  
  if (!TEXTFORMAT) {
    fp_sanity = myfopen("onur_sanity.read", "w");
    fprintf(fp_sanity, "WS %d\tSS %d %s %d\n", ws, ss, DBNAME, genomelen);

    encoded_window_bytes = (int)((ws+BASES_PER_ENCODING_UNIT)/BASES_PER_ENCODING_UNIT);
    encoded_window = (unsigned char *) mymalloc(sizeof(unsigned char) * encoded_window_bytes);
    
    // ONUR --- Begin - Read from the file. Encoded window first
    while (read_bytes = fread(encoded_window, sizeof(unsigned char) * encoded_window_bytes, 1, fp)) {
      //fprintf(stdout, "bytes to read: %d   read_bytes: %d\n", sizeof(char) * encoded_window_bytes, read_bytes);
      //assert(read_bytes == (sizeof(char) * encoded_window_bytes));
      i = 0;
      while (i < ws) {
	// Decode the bytes --- for now sanity check... we may want to keep it like this for ever
	int encoded_window_index = i/BASES_PER_ENCODING_UNIT;
	unsigned char encoded_byte = encoded_window[encoded_window_index]; 
	int lshift_amount = (i % BASES_PER_ENCODING_UNIT) * BITS_TO_ENCODE_BASE;
	int rshift_amount = (BASES_PER_ENCODING_UNIT - 1) * BITS_TO_ENCODE_BASE;
	unsigned char mask = (1 << BITS_TO_ENCODE_BASE) - 1;
	unsigned char masked_encoded_base = ((encoded_byte << lshift_amount) >> rshift_amount) & mask;
	window[i] = reverseCindex(masked_encoded_base);
	if (VERBOSE)
	  fprintf(stdout, "i: %d masked_base: %d char: %c (ls: %d, rs: %d)\n", i, masked_encoded_base, window[i], lshift_amount, rshift_amount);
	i++;
      }
      if (VERBOSE)
	fprintf(stdout, "\n");
      
      window[i] = '\0';
      fprintf(fp_sanity, "%s\t|\t", window);
      //fprintf(stdout, "%s\t", window);
      
      
      // read the encoded location count
      fread(&location_count_encoded, sizeof(location_count_encoded), 1, fp);
      if (location_count_encoded != 0) {
	location_count = location_count_encoded;
      } else {
	fread(&location_count, sizeof(location_count), 1, fp);
      }
      
      //fprintf(stdout, "locations: %d\t | ", location_count);
      
      // now read the locations (integer values)
      i = location_count;
      while (i) {
	fread(&start, sizeof(start), 1, fp);
	fprintf(fp_sanity, "%d\t", start);

	//if (start > 0)
	start += INDEXSTART;
	insert(*root, window, start, name); // #0: INSERT - strand

	rcomp(window, rcwindow);
	start = -1*start;
	start -= INDEXSTART;
	insert(*root, rcwindow, start, name); // #0: INSERT - strand
	
	//else
	start -= INDEXSTART;

	i--;
      }
      fprintf(fp_sanity, "\t!\n");
      //fprintf(stdout, "\n");
    }
    // ONUR --- End - Read from the file. Encoded window first
    fclose(fp_sanity);
    free(encoded_window);
    //exit(0); // for debugging reads
  } 
  else { // TEXTFORMAT
    // OLD version to read from the file
    
    while (fscanf(fp, "%s", window) > 0){
      // get the first |

      fscanf(fp, "%s", dummy);
      dummy[0] = 0;

      
      while (dummy[0] != '!'){
	fscanf(fp, "%s", dummy);
	if (dummy[0] != '!' && dummy[0]!='|'){ // new start
	  start = atoi(dummy);
	  start += INDEXSTART;
	  insert(*root, window, start, name); // #0: INSERT + strand
	  // #0:  INSERT REVCOMPLEMENT
	  rcomp(window, rcwindow);
	  start = -1*start;
	  start -= INDEXSTART;
	  insert(*root, rcwindow, start, name); // #0: INSERT - strand
	}
      }
    }
  }

  fclose(fp);
  free(window);
}
 
void do_search(struct trie *root, char *fullsfile, char *qfile){
  FILE *fp;
  FILE *qp;
  char fosmidName[SEQ_LENGTH];
  char ch;
  FILE *output;
  struct threadParam *params;
  int tid; int rc;
  pthread_t *threads;

  char *sfile=(char *) malloc(sizeof(char)*250);
  char *stokenizer=(char *) malloc(sizeof(char)*250);
  char *temp=(char *) malloc(sizeof(char)*250);

  if (OUTFOLDER[0] == 0)
    strcpy(OUTFOLDER, ".");

  threads = (pthread_t *) mymalloc(sizeof(pthread_t) * THREADS);


  sprintf(sfile,"%s",fullsfile);
  sprintf(stokenizer,"%s",fullsfile);

  temp=strtok(stokenizer,"\\/");

   while (temp=strtok(0,"\\/"))
                        sprintf(sfile,"%s",temp);




    sprintf(fosmidName, "%s/%s.%s.out", OUTFOLDER, sfile, DBNAME2); // reuse :)

   printf("Fosmid Name is %s \n",fosmidName); //eray
  output = myfopen(fosmidName, "w");
  fosmidName[0] = 0;

  /* put header for output */

  /* shut down the header
     fprintf(output, "Searching Fosmids in %s\n", sfile);
     fprintf(output, "WINDOW SIZE: %d\n", WINDOW_SIZE);
     fprintf(output, "---------------\n");
     fprintf(output, "%10s\t%10s\t%10s\%14s\t%s\t%s\n\n", "FNAME","START","END","WINDOWmatch","ORIENTATION", "SOURCE");
  */

  nfosmid = 0;
  fprintf(stdout, "\n\nStart Searching %s ...\n\n", fullsfile);
  
  fp = myfopen(fullsfile, "r");
  fscanf(fp, "%c", &ch);  // pass first >
  if (qfile[0] != '\0'){
    qp = myfopen(qfile, "r");
    fscanf(qp, "%c", &ch);  // pass first >
  }
  else
    qp = NULL;

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
  int qflen; // quality based flen
  int flen;
  int minwin;
  int i;
  char fosmidName[SEQ_LENGTH];
  int willSearch;
  int skipN;
  struct sloc *locations;  



  root = (struct trie *) (((struct threadParam *)parm)->trieRoot);
  fp = (FILE *) (((struct threadParam *)parm)->in);
  qp = (FILE *) (((struct threadParam *)parm)->qual);
  output = (FILE *) (((struct threadParam *)parm)->out);

  
  window = (char *)mymalloc(sizeof(char) * (WINDOW_SIZE+1));
  fosmidName[0] = 0;

  while (readFosmid(fp, fosmid, fosmidName, qp, fqual)){
    locations = NULL;
    
    if (VERBOSE)
      fprintf(stdout, "Searching %s\n", fosmidName);

    flen = strlen(fosmid);
#ifdef DEBUG
    printf("flen: %d\n", flen);
#endif
    qflen = flen;

    i=0;

    while (i<flen-WINDOW_SIZE+1){
      willSearch = getWindow(fosmid, flen, &window, i, &skipN);  

      if (willSearch){
	search(root, window, &locations, i);	
      }
      else{
	// some chars lost
	qflen=qflen-(skipN-i);
	i=skipN;
      }
      i++;
    }
    
    minwin = (int)ceil((float)(qflen/WINDOW_SIZE)) / 5;
#ifdef DEBUG
    printf("flen:%d qflen:%d minwin:%d\n", flen, qflen, minwin);
#endif
    
    
    if (minwin>7)
      minwin=7;
    

    if (locations!=NULL){
      findPositions(locations, fosmidName, qflen, output, minwin, fosmid);
      freeLocation(locations);
    }

  }

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

int readFosmid(FILE *fp, char *fosmid, char *name, FILE *qp, int *qual){
  int i;
  int qlen;
  char ch=0;
  char qname[100];

  pthread_mutex_lock(&readMutex);  	

  i = 0;

  if (!(fscanf(fp, "%s", name) > 0)){
    pthread_mutex_unlock(&readMutex);  	
    return 0;
  }
  
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
  
  qlen = readQual(qp, qual, qname);

  if (qp != NULL){

    if (strcmp(qname, name)){
      fprintf(stderr, "Fosmid and quality names do not match: %s\t%s !!!.\n", name, qname);
      exit(0);
    }
    
    if (qlen != i){
      fprintf(stderr, "Fosmid and quality lengths do not match: %s/%d\t%s/%d !!!.\n", name, i, qname, qlen);
      exit(0);
    }

  }

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
    fprintf(stdout, "Sloc usage: %lld\n", slocmem);
    fprintf(stdout, "Allocs: %d\nFrees: %d\n", allocs, frees);
    exit (0);
  }

  return ret;
}


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


/* cluster stuff */

void alloc_cmember(struct cmember **cm){
  (*cm) = (struct cmember *) mymalloc (sizeof(struct cmember));
  (*cm)->next = NULL;
  //(*cm)->start=0;
  (*cm)->windowID = -1;
  callocs++;
}


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







void alloc_cluster(struct cluster **cl){
  (*cl) = (struct cluster *) mymalloc (sizeof(struct cluster));
  (*cl)->members = NULL;
  (*cl)->next = NULL;
  (*cl)->nmember = 0;
  clallocs++;
}



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


  /* newloc is already sorted */

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
	*/
	if (((current->members->windowID - windows->windowID) >= WINDOW_SIZE-1)  &&  
	    ((current->start - loc->start) >= WINDOW_SIZE-1) 
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
	}*/
    }


    if (current->nmember >= minwin && 
	((float)(expectedwin-thiswin)/(float)thishitlen) < (1-IDENTITY) &&
	//isShortOK &&
	//thiswin >= (thishitlen/WINDOW_SIZE)*IDENTITY &&
	//thishitlen >= (thiswin*WINDOW_SIZE)*IDENTITY &&	
	thishitlen <= thisupratio && 
	thishitlen >= thisbottomratio){
	maxWin = current->nmember;
	maxEnd  = current->end;
	maxStart = current->start;
	maxWinEnd = current->startWin + WINDOW_SIZE;
	maxWinStart = current->members->windowID+1;
	//}	

	pthread_mutex_lock(&logMutex);  	

	if (maxStart > 0 && maxEnd > 0) {
	  fprintf(out, "%10s\t%10d\t%10d\t%6d\t%6d\t%d\t\tF\t%s\t", fosmidName, maxStart, maxEnd+WINDOW_SIZE-1, maxWinStart, maxWinEnd, maxWin, DBNAME);
	  /*printSeq(out, fosmid, maxWinStart, maxWinEnd);
	    fprintf(out, "\t");
	    printSeq(out, genome, maxStart-INDEXSTART, maxEnd+WINDOW_SIZE-1-INDEXSTART);*/
	  fprintf(out, "\n");
	}
	else if (maxStart < 0 && maxEnd < 0) {
	  fprintf(out, "%10s\t%10d\t%10d\t%6d\t%6d\t%d\t\tR\t%s\t", fosmidName, (-1*maxEnd), (-1*maxStart+WINDOW_SIZE-1), maxWinStart, maxWinEnd, maxWin, DBNAME);
	  /*    printSeq(out, fosmid, maxWinStart, maxWinEnd);
		fprintf(out, "\t");
		printSeq(out, genome, (-1*maxEnd-INDEXSTART), (-1*maxStart+WINDOW_SIZE-1-INDEXSTART));*/
	  fprintf(out, "\n");
	}
	
	pthread_mutex_unlock(&logMutex);  	
	
	
    }

    

    /*
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
    */



    /*
	pthread_mutex_lock(&logMutex);  	
	
	if (maxStart > 0 && maxEnd > 0) 
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tF\t%s\n", fosmidName, maxStart, maxEnd+WINDOW_SIZE-1, maxWinStart, maxWinEnd, maxWin, DBNAME);
	else if (maxStart < 0 && maxEnd < 0) 
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tR\t%s\n", fosmidName, (-1*maxEnd), (-1*maxStart+WINDOW_SIZE-1), maxWinStart, maxWinEnd, maxWin, DBNAME);
	
	pthread_mutex_unlock(&logMutex);  	
    */

    current = current->next;
  }

  /*
  printf("bee %d %d\n", maxStart, maxEnd);
  printf("maxwin %d\n", maxWin);
  */

  
  if (maxWin < minwin)
    return;

  current = (*cRoot);

  //while (current != NULL){

  /*
  pthread_mutex_lock(&logMutex);  	
  
  
  if (maxStart > 0 && maxEnd > 0) {
    fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tF\t%s\t", fosmidName, maxStart, maxEnd+WINDOW_SIZE-1, maxWinStart, maxWinEnd, maxWin, DBNAME);
    //printSeq(out, fosmid, maxWinStart, maxWinEnd);
    fprintf(out, "\t");
    printSeq(out, genome, maxStart-INDEXSTART, maxEnd+WINDOW_SIZE-1-INDEXSTART); 
    fprintf(out, "\n");
  }
  else if (maxStart < 0 && maxEnd < 0) {
    fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\tR\t%s\t", fosmidName, (-1*maxEnd), (-1*maxStart+WINDOW_SIZE-1), maxWinStart, maxWinEnd, maxWin, DBNAME);
    //    printSeq(out, fosmid, maxWinStart, maxWinEnd);
    fprintf(out, "\t");
    printSeq(out, genome, (-1*maxEnd-INDEXSTART), (-1*maxStart+WINDOW_SIZE-1-INDEXSTART));
    fprintf(out, "\n");
  }

  pthread_mutex_unlock(&logMutex);  	
  */

    // current = current->next;  
    // }
}


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
    E[i][0] = -1000000; /* -infinity */
    P[i][0] = 1;
  }
  for (j=1;j<=tSize;j++){
    V[0][j] = 0 - Wg - j*Ws;
    //V[0][j] = 0;
    F[0][j] = -1000000; /* -infinity */
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
  


  /* vectors ready, set, go! */

  

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

