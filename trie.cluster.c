#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define NODRAW
//#define RANDOMS

#define SEQ_LENGTH 200
#define FILENAME_LENGTH 20
#define FOSMID_LENGTH 5500
#define IDENTITY 0.975


typedef struct cmember{
  //int start; // location start
  int windowID;
  struct cmember *next;
}scmember;


typedef struct sloc{
  int start; // start index
  /* if start index is negative, then that window is
     reverse-complemented */
#ifdef RANDOMS
  char seq[FILENAME_LENGTH]; // which chromosome/contig ?
#endif
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
  struct cmember *members;
  struct cluster *next;
}scluster;


int WINDOW_SIZE;
int SLIDE_SIZE;
int DUMPTREE;
int SAVEINDEX;
int MINWIN;

void help(char *);
int readFosmid(FILE *, char *, char *);
char *readFasta(FILE *, int *);
int getWindow(char *, int, char **, int, int *);
int getIndexWindow(FILE *, char **, int *);
int cindex(char);
void alloc_trie(struct trie **);
void insert(struct trie *, char *, int, char *);
void drawTrie(FILE *, struct trie *, char);
void saveTrie(struct trie *, char *);
void saveIndex(struct trie *, FILE *,char *);
void loadTrie(char *, struct trie **);
void dumpLocation(struct trie *, FILE *);
void concatLocations(struct sloc **, struct sloc *, int);
void freeLocation(struct sloc *);
void sortLocations(struct sloc *loc);
void findPositions(struct sloc *, char *, int, FILE *);
void do_search(struct trie *, char *);
FILE *myfopen(char *, char *);
void rcomp(char *, char *);
//int search(struct trie *, char *, int *, char **);
int search(struct trie *, char *, struct sloc **, int);
void insert_sloc(struct sloc **, int, char *);
int do_file(char *, struct trie *);
struct sloc *copyLocation(struct sloc *, int);
void *mymalloc(size_t);


struct sloc *mergeLocations(struct sloc **, int);


/* cluster stuff from pimp.c */

void alloc_cmember(struct cmember **);
void insert_cmember(struct sloc **, int);
void free_cmember(struct cmember *);
void alloc_cluster(struct cluster **cl);
void free_cluster(struct cluster *cl);
void insert_cmember_to_cluster(struct cluster **, int, int);
void insertCluster(struct cluster **, struct sloc *, struct sloc *, char *, FILE *);


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



int main(int argc, char **argv){

  int i;
  //FILE *fasta;
  FILE *treefile;
  //char *window;
  //char *genome;
  struct trie *root;
  char fname[SEQ_LENGTH*2];
  //char tfname[SEQ_LENGTH];
  //  char seqname[SEQ_LENGTH];
  //int glen; // length of the text
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

  /* directory stuff */
  DIR *dp;
  struct dirent *ep;

  printf("trie: %d\nsloc: %d\n", (int)sizeof(struct trie), (int)sizeof(struct sloc));
  //return 1;
  triemem = 0;
  slocmem = 0;
  identifier = 0;

  DUMPTREE = 0;
  VERBOSE = 0;
  SAVEINDEX = 0;
  LOADED = 0;
  MINWIN = 1;

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
      loadTrie(argv[i+1], &root);
    }
    else if (!strcmp(argv[i], "-start"))
      build_start = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-end"))
      build_end = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-search"))
      strcpy(searchFile, argv[i+1]);
    else if (!strcmp(argv[i], "-minwin")){
      MINWIN = atoi(argv[i+1]);
      fprintf(stderr, "\tminwin:\t%d\n", MINWIN);
    }
  }

  if (MINWIN <= 0){
    fprintf(stderr, "Strange MINWIN=%d\n", MINWIN);
    return 0;
  }

  if (WINDOW_SIZE == 0){ // deprecated: || SLIDE_SIZE == 0 || SLIDE_SIZE>WINDOW_SIZE){
    fprintf(stderr, "Strange ws and ss.\n");
    return 0;
  }

  if ((build_start == 0) ^ (build_end == 0)){
    fprintf(stderr, "Strange build_start and build_end.\n");
    return 0;
  }
    
  if (!LOADED){
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
    if (DUMPTREE)
      fprintf(stderr,"\tdumptrie:\t\ton\n");
    if (regexpr[0] != 0)
      fprintf(stderr,"\tregexp:\t\t%s\n",regexpr);
    if (build_start != 0)
      fprintf(stderr,"\tbuild_start:\t%d\n\tbuild_end:\t%d\n",build_start, build_end);
    if (searchFile[0] != 0)
      fprintf(stderr,"\tsearch:\t\t%s\n",searchFile);

    if (regcomp(&preg, regexpr, 0) != 0) {
      printf("cant compile regular expression\n");
      return 0;      /* Report error. */
    }

    /*
      status = regexec(&preg, argv[2], (size_t) 0, NULL, 0);
      printf("%d\n", status);
    */

  
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
    else if (fname[0]!=0){
      filestat = do_file(fname, root);
      if (filestat == 0)
	return 0;
    }

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

  fprintf(stderr, "Total memory for the trie: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", triemem, (float)triemem/1024.0, (float)triemem/1024.0/1024.0);
  fprintf(stderr, "Total memory for the locations: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
  
  slocmem = 0;

  allocs = 0;
  frees = 0;
  callocs = 0;
  cfrees = 0;
  clallocs = 0;
  clfrees = 0;

  if (searchFile[0] != 0)
    do_search(root, searchFile);
  
  fprintf(stderr, "Total memory for the locations afterwards: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
  fprintf(stderr, "Allocs %d\nFrees %d\n", allocs, frees);
  fprintf(stderr, "cAllocs %d\ncFrees %d\n", callocs, cfrees);
  fprintf(stderr, "clAllocs %d\nclFrees %d\n", clallocs, clfrees);

  return 1;
}



void help(char *pname){
  //fprintf(stderr, "%s <-f [fastafile] OR -dir [startdir]> -ws [window_size] -ss [slide_size] <-dumptree> <-v>\n", pname);
  fprintf(stderr, "%s <-f [fastafile] OR -dir [startdir]> -ws [window_size] <-dumptree> <-v> <-saveindex> <-loadindex>\n", pname);
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
      sequence[i++] = ch;
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
      if (isalpha(ch))
	(*window)[i++] = ch;
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
    if (current->kids[loc] == NULL)
      return 0; // not found
    current = current->kids[loc];
  }
  if (current == NULL)
    return 0;
  
  /*
  *location = current->location;
  strcpy((*seqname), current->seq);
  */
  //(*location) = copyLocation(current->location);
  concatLocations(location, current->location, windowID);
  //(*location) = current->location;
  return 1;
  
}

void drawTrie(FILE *treefile, struct trie *root, char ch){
#ifndef NODRAW
  struct trie *current=root;
  if (current != NULL){
    fprintf(treefile, "\t%d [shape=circle, label=\"\", style=filled, height=0.1, width=0.1, color=black, fontsize=6];\n", current->id, ch );  
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
#ifdef RANDOMS
  (*loc)->seq[0]=0;
#endif
}

void insert_sloc(struct sloc **location, int start, char *seqname){
  /* some memory saving idea :
     linked list of linked lists
     struct sloc has seqname and linked list of integers
     insert_sloc creates new integer linked list with new seqname
     insert_index inserts new integer to already existing seqname
     saves some memory from using lots of identical seqnames
     no need to the things above any more.
  */


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
      
#ifdef RANDOMS
    if (strchr(seqname, '/') == NULL)
      strcpy((*location)->seq, seqname);
    else 
      strcpy((*location)->seq, (strrchr(seqname, '/')+1));
#endif
    return;
  }

  alloc_sloc(&new);
  new->start = start; 

#ifdef RANDOMS
  if (strchr(seqname, '/') == NULL)
    strcpy(new->seq, seqname);
  else 
    strcpy(new->seq, (strrchr(seqname, '/')+1));
#endif


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

  
  fprintf(stderr, "\n\nInserting sequence from file %s\n", fname);
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
      rcomp(window, rcwindow);
      // start location is i+1
      // because i starts from 0 here; but 
      // we want the sequence index starting from 1
      // STARTING POSITION FOR FORWARD
      insert(root, window, (i+1), fname);      
      // reverse complement of this window is indexed by its negative
      // value ENDING POSITION FOR REVERSSE COMPLEMENT
      insert(root, rcwindow, (-1*(i+1)), fname);      
    }
    i+=WINDOW_SIZE;
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

  fprintf(indexFile, "WS %d\tSS %d %s\n", WINDOW_SIZE, SLIDE_SIZE, fprefix);
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
  if (current->location != NULL){ // leaf node
    fprintf(fp, "%s\t|\t", window);
    dumpLocation(current, fp);
    fprintf(fp, "\t!\n");
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

void dumpLocation(struct trie *root, FILE *fp){
  struct sloc *location;
  char seq[SEQ_LENGTH];
  location = root->location;
  seq[0] = 0;
  while (location != NULL){
#ifdef RANDOMS
    if (strcmp(seq, location->seq)){
      strcpy(seq, location->seq);
      fprintf(fp,"\t%s\t", seq);
    }
#endif
    //if (location->start != 0)
    fprintf(fp,"%d\t", location->start);
    //if (location->rcstart != 0)
    //fprintf(fp,"%d\t", location->rcstart);
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
  //new->fromWindow = windowID;
  ret = new;
  this = ret;
  current = current->next;

  while (current != NULL){
      alloc_sloc(&new);
      new->start = current->start;
      insert_cmember(&new, windowID);
      //new->fromWindow = windowID;
      this->next = new;
      this = this->next;
      current = current->next;
  }

  return ret;
}




struct sloc *mergeLocations(struct sloc **s1, int nloc){
  struct sloc *new;
  struct sloc *ret;
  struct sloc *s1t;
  int stop=0;
  int i;
  int max;
  int lmax;
  struct sloc **allloc;

  /* too slow and buggy  */

  allloc = (struct sloc **) mymalloc(sizeof(struct sloc *) * nloc);

  for (i=0;i<nloc;i++)
    allloc[i] = s1[i];

  stop = 0;

  ret = NULL;

  while (!stop){
    max=-500000000; // minus 500million
    lmax=-1;
    s1t = ret;
    stop = 1;
    
    for (i=0;i<nloc;i++){
      if (allloc[i] != NULL) stop=0;
      else continue;
      
      if (allloc[i]->start > max){
	max = allloc[i]->start;
	lmax = i;
      }
    }
    
    if (stop) break;
    if (lmax==-1) break;

    if (ret == NULL){
      alloc_sloc(&ret);
      ret->start = max;
      insert_cmember(&ret, lmax);
      s1t = ret;
    }
    else{
      if (s1t->start == max)
	insert_cmember(&s1t, lmax);
      else{
	alloc_sloc(&new);
	new->start = max;
	insert_cmember(&new, lmax);
	s1t->next = new;
	s1t = s1t->next;
      }
    }

    allloc[lmax] = allloc[lmax]->next;

  }

  free(allloc);

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



void sortLocations(struct sloc *loc){
  struct sloc *fwd, *bwd;
  struct sloc *bwdprev;
  int swap;


  fwd = loc;
  while (fwd != NULL){
    bwdprev = fwd;
    bwd = fwd->next;

    while (bwd != NULL){
      if (bwd->start > fwd->start){	
	swap = bwd->start;
	bwd->start = fwd->start;
	fwd->start = bwd->start;

	bwdprev = bwd;
	bwd = bwd->next;
      }
      else if (bwd->start == fwd->start){
	bwdprev->next = bwd->next;
	free(bwd);
	bwd = bwdprev->next;
      }
    }

    fwd = fwd->next;
      
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
  int ws, ss;
  char dummy[SEQ_LENGTH];
  char dummy2[SEQ_LENGTH];
  char name[SEQ_LENGTH];
  char *window;
  int start;

  fp = myfopen(fname, "r");
  
  alloc_trie(root);
  
  fscanf(fp, "%s %d %s %d %s", dummy, &ws, dummy2, &ss, DBNAME);
  WINDOW_SIZE = ws;
  SLIDE_SIZE = ss;
  
  window = (char *) mymalloc(sizeof(char) * (ws+1));
  name[0] = 0;

  while (fscanf(fp, "%s", window) > 0){
    // get the first |
    fscanf(fp, "%s", dummy);
#ifdef RANDOMS
    // get name
    fscanf(fp, "%s", name);
#endif
    dummy[0] = 0;

    while (dummy[0] != '!'){
      fscanf(fp, "%s", dummy);
#ifdef RANDOMS
      if (dummy[0] == '|') // new name
	fscanf(fp, "%s", name);
#endif
      if (dummy[0] != '!' && dummy[0]!='|'){ // new start
	start = atoi(dummy);
	insert(*root, window, start, name);
      }
    }
  }
  

  fclose(fp);
  free(window);
}
 
void do_search(struct trie *root, char *sfile){
  char *window;
  FILE *fp;
  //char *fosmid;
  char fosmid[FOSMID_LENGTH];
  char fosmidName[SEQ_LENGTH];
  int i; char ch;
  int flen;
  struct sloc *locations;
  //struct sloc **lloc;
  struct sloc *newloc;
  // struct sloc *tmploc;
  struct timeval start, end;
  // struct timeval sstart, send;
  struct timezone tz;
  int willSearch = 1;
  FILE *output;
  int skipN;
  //struct cluster *cRoot;
  
  sprintf(fosmidName, "%s.out", sfile); // reuse :)
  output = myfopen(fosmidName, "w");
  fosmidName[0] = 0;

  /* put header for output */
  
  fprintf(output, "Searching Fosmids in %s\n", sfile);
  fprintf(output, "WINDOW SIZE: %d\n", WINDOW_SIZE);
  fprintf(output, "---------------\n");
  fprintf(output, "%10s\t%10s\t%10s\%14s\t%s\t%s\n\n", "FNAME","START","END","WINDOWmatch","ORIENTATION", "SOURCE");

  //int newlocs;
  //  FILE *log  = fopen("mylog", "w");
  //FILE *nlog  = fopen("newloc", "w");

  nfosmid = 0;
  //newlocs = 0;
  fprintf(stderr, "\n\nStart Searching %s ...\n\n", sfile);
    
  window = (char *)mymalloc(sizeof(char) * (WINDOW_SIZE+1));
  //fosmid = (char *)mymalloc(sizeof(char) * SEQ_LENGTH*20);
  
  fp = myfopen(sfile, "r");
  fscanf(fp, "%c", &ch);  // pass first >

  
  gettimeofday(&start, &tz);

  //while (fscanf(fp, "%s", fosmid) > 0){
  while (readFosmid(fp, fosmid, fosmidName)){
    locations = NULL;
    
    if (VERBOSE)
      fprintf(stderr, "Searching %s\n", fosmidName);
    //getchar();
    flen = strlen(fosmid);
    nfosmid++;
    if  (nfosmid%1000==1)
      fprintf(stdout, "nfosmid: %d\n", nfosmid);
    //lloc = (struct sloc **) mymalloc(sizeof(struct sloc *) * flen);
    
    i=0;

    //    for (i=0;i<flen-WINDOW_SIZE+1;i++){
    //gettimeofday(&sstart, &tz);
    
    while (i<flen-WINDOW_SIZE+1){
      willSearch = getWindow(fosmid, flen, &window, i, &skipN);  
      if (willSearch){
	newloc = NULL;
	//if (VERBOSE)
	//printf("window: %s\n", window);
	//search(root, window, &newloc);
	

	search(root, window, &locations, i);
	
	//search(root, window, &(lloc[i]), i);

  
/*
	fprintf(stderr, "Total memory for the locations afterwards: %lld bytes = %6.2f Kbytes = %4.2f Mbytes\n", slocmem, (float)slocmem/1024.0, (float)slocmem/1024.0/1024.0);
	exit(0);

*/

	
	//tmploc = newloc;
	//while (tmploc != NULL){
	//fprintf(nlog,"%d\n", tmploc->start);
	// tmploc = tmploc->next;
	//}
	
	//      	fprintf(nlog, "\n--\n");
	

	
	//if (newloc!=NULL){
	  //concatLocations(&locations, newloc);
	  //freeLocation(newloc);
	//}
      }
      else
	i+=skipN;
      i++;
    }
    

    //locations = mergeLocations(lloc, flen);
    //gettimeofday(&send, &tz);
    //fprintf(stderr, "windowsearch time %d microseconds\n", ((int)(send.tv_sec*1000000+send.tv_usec)-(int)(sstart.tv_sec*1000000+sstart.tv_usec)));

    /*

    if (locations!=NULL){
      insertCluster(&cRoot, locations, NULL, fosmidName, output);
      
      free_cluster(cRoot);
    }
    */

    //else
    // printf("locations null\n");
    
    //fprintf(log,"\n\n\n--------\n\n");


    
    
    //    tmploc = locations;
    //while (tmploc != NULL){
    // fprintf(log,"%d\n", tmploc->start);
    // tmploc = tmploc->next;
    // }

    //    fprintf(log,"\n");
    
    
    //sortLocations(locations);
    
    //printf("search done for nfosmid %d\n", nfosmid);
    if (locations!=NULL){
      findPositions(locations, fosmidName, flen, output);
      freeLocation(locations);
    }

    //    free(lloc);

  }

  gettimeofday(&end, &tz);
  fprintf(stderr, "search time %ld microseconds\n", ((long)(end.tv_sec*1000000+end.tv_usec)-(long)(start.tv_sec*1000000+start.tv_usec)));
  //fprintf(stderr, "search time %ld seconds\n", ((long)(end.tv_sec)-(long)(start.tv_sec)));

  //fprintf(stderr, "\n\nnewlocs: %d\n\n", newlocs);
  free(window);
  //free(fosmid);

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

int readFosmid(FILE *fp, char *fosmid, char *name){
  int i;
  char ch=0;
  
  i = 0;

  if (!(fscanf(fp, "%s", name) > 0))
    return 0;
  
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


void findPositions(struct sloc *locations, char *fosmidName, int flen, FILE *out){
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
  struct cluster *cRoot;

  maxStart = 0;
  maxEnd = 0;
  maxCount = 0;

  threshold = (int)ceil((float)(flen+WINDOW_SIZE)/IDENTITY);
  cnthreshold = (int)ceil((float)(flen/WINDOW_SIZE));
  //cnthreshold = 2;

  MINWIN = (int)ceil((float)(flen/WINDOW_SIZE)) / 2;

  //printf("threshold: %d\ncthr:%d\n", threshold, cnthreshold);
  cnthreshold = cnthreshold*IDENTITY;
  //printf("threshold: %d\ncthr:%d\n", threshold, cnthreshold);

  thisStart = locations->start;
  this = locations;
  last = locations;
  lastStart = thisStart;


  current = locations->next;

  count = 0;
  while (current!=NULL){
    //printf("ts:%d\tcs:%d\tls:%d\n", thisStart, current->start, lastStart);
    if (!((thisStart > 0) ^ (current->start > 0))){
      if (abs(thisStart + WINDOW_SIZE - 1 - current->start) < threshold){
	//if (abs(lastStart - current->start) == WINDOW_SIZE){
	//printf("count: %d\n", count);
	count++;
      }

      /*
      else if (count > cnthreshold){
	if (thisStart > 0 && lastStart > 0) 
	  fprintf(out, "%10s\t\%10d\t%10d\t%d/%d\t\tF\n", fosmidName, lastStart, thisStart+WINDOW_SIZE-1, count, cnthreshold);
	else if (thisStart < 0 && lastStart < 0) // not sure about RC yet
	  //fprintf(out, "c-%s\t\%d\t%d\t%d/%d\tR\n", fosmidName, (-1*(lastStart-WINDOW_SIZE+1)), (-1*thisStart), count, threshold);
	  fprintf(out, "%10s\t\%10d\t%10d\t%d/%d\t\tR\n", fosmidName, (-1*thisStart), (-1*(lastStart-WINDOW_SIZE+1)), count, cnthreshold);
	if (current->next != NULL)
	  thisStart = current->next->start;
	count = 0;
      }
      */
      else { // if (current->next != NULL){
	//thisStart = current->next->start;

	if (count > maxCount){
	  maxCount = count;
	  maxStart = thisStart;
	  maxEnd = lastStart;
	}

	if (count >= MINWIN && last!=NULL){
	  //printf("insert cluster here\n");
	  //printf("t: %d\tl: %d\n", this->start, last->start);
	  insertCluster(&cRoot, this, last->next, fosmidName, out);
	  //insertCluster(&cRoot, this, NULL, fosmidName, out);
	  free_cluster(cRoot);
	}
	else if (count >= MINWIN && last==NULL){
	  insertCluster(&cRoot, this, NULL, fosmidName, out);
	  free_cluster(cRoot);
	}
	
	this = current;
	thisStart = current->start;
	count=0;
      }
    }  
    else{

      if (count > maxCount){
	maxCount = count;
	maxStart = thisStart;
	maxEnd = lastStart;
      }

      if (count >= MINWIN && last!=NULL){
	insertCluster(&cRoot, this, last->next, fosmidName, out);
	free_cluster(cRoot);
      }
      else if (count >= MINWIN && last==NULL){
	insertCluster(&cRoot, this, NULL, fosmidName, out);
	free_cluster(cRoot);
      }

	/*

      if (count > cnthreshold){
	if (thisStart > 0 && lastStart > 0) 
	  fprintf(out, "%10s\t\%10d\t%10d\t%d/%d\t\tF\n", fosmidName, lastStart, thisStart+WINDOW_SIZE-1, count, cnthreshold);
	else if (thisStart < 0 && lastStart < 0)  // not sure about RC yet
	  fprintf(out, "%10s\t\%10d\t%10d\t%d/%d\t\tR\n", fosmidName, (-1*thisStart), (-1*(lastStart-WINDOW_SIZE+1)), count, cnthreshold);
	  }*/

      //if (current->next != NULL)
      //thisStart = current->next->start;
      this = current;
      thisStart = current->start;
      count = 0;      
    }
    last = current;
    lastStart = current->start;
    current = current->next;
  }

  /*
  if (maxCount == 0)
    return;

  if (maxStart > 0 && maxEnd > 0) 
    fprintf(out, "%10s\t\%10d\t%10d\t%d\t\tF\n", fosmidName, maxEnd, maxStart+WINDOW_SIZE-1, maxCount);
  else if (thisStart < 0 && lastStart < 0) // not sure about RC yet
    fprintf(out, "%10s\t\%10d\t%10d\t%d\t\tR\n", fosmidName, (-1*maxStart), (-1*(maxEnd-WINDOW_SIZE+1)), maxCount);
  */
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
    return;
  }
  

  alloc_cmember(&new);
  new->windowID = windowID;
  new->next = (*cl)->members;
  (*cl)->members = new;
  (*cl)->start = start;

  ((*cl)->nmember)++;

}

void insertCluster(struct cluster **cRoot, struct sloc *newloc, struct sloc *endloc, char *fosmidName, FILE *out){
  struct cluster *current;
  struct cluster *prev;
  struct cluster *new;
  struct sloc *loc;
  struct sloc *ploc;
  struct sloc *nloc;
  struct cmember *windows;
  int maxWin; 
  int maxStart;
  int maxEnd;
  int willInsert;

  loc = newloc;
  ploc = loc;
  maxWin = 0;


  /* newloc is already sorted */

  /*
  while(loc->next != endloc && abs(loc->start - loc->next->start)!=WINDOW_SIZE){
    ploc = loc;
    loc = loc->next;
  }
  */

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
  windows = windows->next;
  current = (*cRoot);
  while (windows != NULL){
    alloc_cluster(&new);
    new->end = loc->start;
    insert_cmember_to_cluster(&new, loc->start, windows->windowID);
    current->next = new;
    current = current->next;
    windows = windows->next;
  }

  ploc = loc;
  loc = loc->next;

  // *cRoot is not NULL


  while(loc != endloc){
    willInsert = 1;
    nloc = loc->next;

    /*
    if (nloc != endloc){ // look forward
      if (abs(loc->start - nloc->start) == WINDOW_SIZE)
	willInsert = 1;
    }
    
    if (abs(loc->start - ploc->start) == WINDOW_SIZE)
      willInsert = 1;

    //printf("for %d wi: %d\n", loc->start, willInsert);
    */

    if (willInsert){
      windows = loc->windows;
      while (windows != NULL){
	current = (*cRoot);
	while (current != NULL){
	  if ((abs(windows->windowID - current->members->windowID) >= WINDOW_SIZE ) &&  (abs(loc->start - current->start) >= WINDOW_SIZE)){
	    insert_cmember_to_cluster(&current, loc->start, windows->windowID);
	    break;
	  }
	  prev = current;
	  current = current->next;
	}
	if (current == NULL){
	  //printf("create new for %d/%d\n", nfosmid, loc->start);
	  alloc_cluster(&new);
	  new->end = loc->start;
	  insert_cmember_to_cluster(&new, loc->start, windows->windowID);
	  prev->next = new;
	}
	windows = windows->next;
      }
    }
    ploc = loc;
    loc = loc->next;
  }


  maxWin = 0;
  current = (*cRoot);
  while (current != NULL){
    if (current->nmember >= maxWin){
      maxWin = current->nmember;
      maxEnd  = current->end;
      maxStart = current->start;
    }
    current = current->next;
  }

  /*
  printf("bee %d %d\n", maxStart, maxEnd);
  printf("maxwin %d\n", maxWin);
  */
  if (maxWin < MINWIN)
    return;

  if (maxStart > 0 && maxEnd > 0) 
    fprintf(out, "%10s\t\%10d\t%10d\t%d\t\tF\t%s\n", fosmidName, maxStart, maxEnd+WINDOW_SIZE-1, maxWin, DBNAME);
  else if (maxStart < 0 && maxEnd < 0) // not sure about RC yet
    fprintf(out, "%10s\t\%10d\t%10d\t%d\t\tR\t%s\n", fosmidName, (-1*maxEnd), (-1*maxStart+WINDOW_SIZE-1), maxWin, DBNAME);


}
