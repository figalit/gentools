#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

#define NODRAW
//#define RANDOMS

#define SEQ_LENGTH 200
#define FILENAME_LENGTH 20

typedef struct sloc{
  int start; // start index
  //int rcstart; // reverse complement start index
  /* if start index is negative, then that window is
     reverse-complemented */
#ifdef RANDOMS
  char seq[FILENAME_LENGTH]; // which chromosome/contig ?
#endif
  struct sloc *next;
}slocation;


typedef struct trie{
  /*
  char seq[SEQ_LENGTH/2]; 
  int location;
  */
  struct sloc *location; 
#ifndef NODRAW
  int pid;
  int id;
#endif
  struct trie *kids[4]; //children nodes
}strie;


int WINDOW_SIZE;
int SLIDE_SIZE;
int DUMPTREE;
int SAVEINDEX;
void help(char *);
int readFosmid(FILE *, char *, char *);
char *readFasta(FILE *, int *);
int getWindow(char *, int, char **, int);
int getIndexWindow(FILE *, char **, int *);
int cindex(char);
void alloc_trie(struct trie **);
void insert(struct trie *, char *, int, char *);
void drawTrie(FILE *, struct trie *, char);
void saveTrie(struct trie *, char *);
void saveIndex(struct trie *, FILE *,char *);
void loadTrie(char *, struct trie **);
void dumpLocation(struct trie *, FILE *);
void concatLocations(struct sloc **, struct sloc *);
void freeLocation(struct sloc **);
void do_search(struct trie *, char *);
FILE *myfopen(char *, char *);
void rcomp(char *, char *);
//int search(struct trie *, char *, int *, char **);
int search(struct trie *, char *, struct sloc **);
void insert_sloc(struct sloc **, int, char *);
int do_file(char *, struct trie *);
struct sloc *copyLocation(struct sloc *);


int identifier;
int VERBOSE;
long long triemem;
long long slocmem;


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
  
  if (searchFile[0] != 0)
    do_search(root, searchFile);
  
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
  
  sequence = (char *) malloc((seqlen+1) * sizeof(char));
  
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

int getWindow(char *genome, int glen, char **window, int win_start){
  int i;
  int win_end;
  win_end = win_start+WINDOW_SIZE;
  if (win_end>=glen)
    win_end = glen;
  for (i=win_start;i<win_end;i++){
    if (genome[i] == 'N' || genome[i] == 'O')
      return 0;
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
  (*t) = (struct trie *) malloc(sizeof(struct trie));
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
int search(struct trie *root, char *window, struct sloc **location){
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
  (*location) = copyLocation(current->location);
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
  (*loc) = (struct sloc *) malloc (sizeof(struct sloc));
  if (*loc == NULL){
    fprintf(stderr, "Insufficient memory\n");
    exit(0);
  }
  slocmem+=sizeof(struct sloc);
  (*loc)->next = NULL;
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
  
  window = (char *) malloc(sizeof(char) * WINDOW_SIZE+1);
  rcwindow = (char *) malloc(sizeof(char) * WINDOW_SIZE+1);
  
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
      insert(root, window, (i+1), fname);      
      insert(root, rcwindow, (-1*(i+1)), fname);      
    }
    i+=WINDOW_SIZE;
  }
  fprintf(stderr, "\n");
  
  //free(genome);
  fclose(fasta);
  free(window);
  
  return 1;
}


void saveTrie(struct trie *root, char *fprefix){
  char fname[SEQ_LENGTH];
  FILE *indexFile;
  char window[WINDOW_SIZE];
  
  sprintf(fname, "%s.index", fprefix);
  
  indexFile = myfopen(fname, "w");

  fprintf(indexFile, "WS %d\tSS %d\n", WINDOW_SIZE, SLIDE_SIZE);
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

struct sloc *copyLocation(struct sloc *source){
  struct sloc *new;
  struct sloc *current;
  struct sloc *ret;

  /* IMPORTANT :  make this sorted */
  /* 
     NOTE: locations are already reverse-sorted in ABSOLUTE VALUE (descending order)   
  */


  /* 
     insert positives first; negatives later
   */

  /*

  int hasPositives;
  
  hasPositives = 1;
  
  if (current == NULL)
  return NULL;

  current = source;
  ret = NULL;
  
  while (current != NULL && current->start<0)
  current = current->next;

  alloc_sloc(&ret);

  if (current != NULL){
    ret->start = current->start;
    //ret->rcstart = current->rcstart;
    current = current->next;
  }

  else{
    hasPositives = 0;
    ret->start = source->start;
    current = source->next;
  }


  if (hasPositives){
    while (current != NULL){
      if (current->start > 0){
	alloc_sloc(&new);
	new->start = current->start;
	//new->rcstart = current->rcstart;
	new->next = ret;
	ret = new;
      }
      else 
	printf("rc %d\n", current->start);
      current = current->next;
    }
    current = source;
  }

  // no positives; then first inserted was source


  while (current != NULL){
    if (current->start < 0){
      alloc_sloc(&new);
      new->start = current->start;
      //new->rcstart = current->rcstart;
      new->next = ret;
      ret = new;
    }
    current = current->next;
  }

  */


  current = source;
  while (current != NULL){
      alloc_sloc(&new);
      new->start = current->start;
      new->next = ret;
      ret = new;
      current = current->next;
  }

  return ret;
}

void concatLocations(struct sloc **dest, struct sloc *source){
  
  struct sloc *dest_cur;
  struct sloc *source_cur;

  source_cur = source;

  if ((*dest) == NULL){
    (*dest) = copyLocation(source);
    return;
  }
  
  dest_cur = *dest;

}
		     
void freeLocation(struct sloc **newloc){
  if ((*newloc)->next != NULL)
    freeLocation(&((*newloc)->next));
  free((*newloc));
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
  
  fscanf(fp, "%s %d %s %d", dummy, &ws, dummy2, &ss);
  WINDOW_SIZE = ws;
  SLIDE_SIZE = ss;
  
  window = (char *) malloc(sizeof(char) * (ws+1));
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
  char *fosmid;
  char fosmidName[SEQ_LENGTH];
  int i; char ch;
  int flen;
  struct sloc *locations;
  struct sloc *newloc;
  struct timeval start, end;
  struct timezone tz;
  int willSearch = 1;
  FILE *log  = fopen("mylog", "w");

  fprintf(stderr, "\n\nStart Searching %s ...\n\n", sfile);

  window = (char *)malloc(sizeof(char) * (WINDOW_SIZE+1));
  fosmid = (char *)malloc(sizeof(char) * SEQ_LENGTH*20);
  
  fp = myfopen(sfile, "r");
  fscanf(fp, "%c", &ch);  // pass first >

  locations = NULL;

  gettimeofday(&start, &tz);

  //while (fscanf(fp, "%s", fosmid) > 0){
  while (readFosmid(fp, fosmid, fosmidName)){
    //fprintf(stderr, "Searching %s\n", fosmidName);
    flen = strlen(fosmid);
    for (i=0;i<flen-WINDOW_SIZE;i++){
      willSearch = getWindow(fosmid, flen, &window, i);  
      if (willSearch){
	newloc = (struct sloc *) malloc(sizeof (struct sloc));
	newloc = NULL;
	
	printf( " window search %s\n", window);
	search(root, window, &newloc);
	printf( " window dump \n");


	while (newloc != NULL){
	  fprintf(log,"%d\t", newloc->start);
	  newloc = newloc->next;
	}

	exit(0);



	concatLocations(&locations, newloc);
	freeLocation(&newloc);
      }
    }
    free(locations);
  }

  gettimeofday(&end, &tz);
  fprintf(stderr, "search time %d microseconds\n", ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec)));

  free(window);
  free(fosmid);

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
      return 0;
  }
  fosmid[i] = 0;
  return 1;
}
