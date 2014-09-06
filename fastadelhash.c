
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#define SEQ_LENGTH 150

#define FORWARD 0
#define REVERSE 1

#define LOAD 0
#define CREATE 1

#define ASCENDING 1
#define DESCENDING 0

#define MYPRIME 72552593
//#define MYPRIME 40197601
//#define MYPRIME 953543


typedef struct imatrix{
  int i;
  int j;
  int eweight;
}_imatrix;

typedef struct wpair{
  int id;
  struct wpair *next;
}_wpair;


typedef struct Node{
  int v1,v2,w;
  struct Node *next;
}_Node;

typedef struct Component{
  struct Node *node;
  struct Component *next;
}_Component;


void *mymalloc(size_t size);

void my_fgets(char *, int, FILE *);
int readTwoFasta(FILE *, int);
int hash(char *, int, int);
void end2clone(char *, char *);
void rcomp(char *, char *);
int isReverse(char *);
int encode_window(char *);
void decode_window(int, char *);
int getWindow(char *, int, char *, int);
void do_end(int, int);
void insert_wloc(struct wpair **, int, int, int);
void compare_wloc(struct wpair *, struct wpair *, int *);
int hashTest(FILE *, int);
void dumpwins(FILE *, int);
void do_loaded(FILE *, char *);
void loadwins(FILE *);
char reverseindex(int);
void compare(char *, int, float, int, int, int);
unsigned int cindex(char);
int encode_window2(char *);
int revcomp_encoded(unsigned int);
void freewins(struct wpair *);
struct wpair *copyWins(struct wpair *);
void compactwins(void);
void merge(int, int);
FILE *myfopen(char *, char *);
int loadMatrix(char *, char *);
void decode_wins(char *);
void loadNames(char *);
void dumpclones(char *);
void trimwins(char [], int, char []);


/* component stuff */
void connectedComponent(int, int);
void alloc_node(struct Node **);
void alloc_component(struct Component **);
void insert_node_to_component(struct Component **, int, int, int);
void makecolors(void);
void connComp(int, int);


char **names;
char **forwards;
char *mark;

int nseq;
int WS;
int *windowflag;
int wincnt;
struct wpair **winlocs;
long long collisions;
int *windowcnt;

char **colors;

struct imatrix *interactions;

int EDGEHIST;
int WINORDER;


int main(int argc, char **argv){
  
  FILE *in, *rin, *out;
  int i,j,k;
  int e1, e2, eboth;
  FILE *graph;
  FILE *matlog;
  FILE *winlog;
  FILE *namelog;
  int npairs;
  struct timeval start, end;
  struct timezone tz;
  char fname[SEQ_LENGTH];
  char ofname[SEQ_LENGTH];
  char fname2[SEQ_LENGTH];
  char namefile[SEQ_LENGTH];
  int hashtype;
  int hashtest=0;
  int mintime;
  int minhash;
  float eweight;
  float ecut;
  int acut;

  long winflagtime;
  long inserttime;
  long compacttime;
  long readtime;
  long comparetime;
  long inittime;
  int mincollide=999999999;
  int mincolhash;
  int ccut; // compare cut; do not compare any kmer with cnt<ccut
  int LOADWIN = 0;
  int COMPARE = 0;
  int MERGE = 0;
  char *decoded;
  int LOADMATRIX = 0;
  int DECODE = 0;
  int DUMPCLONES = 0;
  int LOADNAMES = 0;
  int COMPONENTS = 0;
  int TRIMWIN = 0;
  char trimwinfile[SEQ_LENGTH];

  int matrixcnt;
  int doclean = 0;
  
  char removethis[500];
  int removeindex;
  int remcnt;

  EDGEHIST = 0;

  collisions=0;
  
  WS = 12;
  decoded = (char *)mymalloc(WS+1);
  ccut = 1;
  acut = 0;

  if (argc < 2)
    return 0;

  fname[0]=0;
  ofname[0]=0;
  fname2[0]=0;
  namefile[0]=0;
  hashtype=11;
  mintime=999999999;

  winflagtime=0;
  inserttime=0;
  compacttime=0;
  readtime=0;
  comparetime=0;
  inittime=0;
  ecut = 0.0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-h"))
      hashtype=atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-r"))
      strcpy(fname2, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(ofname, argv[i+1]);
    else if (!strcmp(argv[i], "-nc"))
      doclean = 0;
    else if (!strcmp(argv[i], "-clean"))
      doclean = 1;
    else if (!strcmp(argv[i], "-h"))
      strcpy(ofname, argv[i+1]);
    else if (!strcmp(argv[i], "-ht"))
      hashtest=1;
  }
  

  if(fname[0]==0){// || fname2[0]==0){
    fprintf(stderr, "input file?\n");
    return 0;
  }


  in = myfopen(fname,"r");



  if (in == NULL){// || rin == NULL){
    fprintf(stderr, "one file is missing\n");
    return 0;
  }




  if (hashtest){

    for (i=3;i<14;i++){
      collisions = 0;
      printf("hash %d\n",i);
      nseq = readTwoFasta(in, i);
      for (j=0;j<nseq;j++){
	free(names[j]);
	free(forwards[j]);
      }
      free(names);      free(forwards);
      free(mark);
    }

    return 1;
  }

  rin = myfopen(fname2,"r");

  nseq = readTwoFasta(in, hashtype);

  remcnt = 0;
  while(fscanf(rin, "%s", removethis) > 0){
    removeindex = hash(removethis , nseq, hashtype); // hope it works
    mark[removeindex] = 1;
    //printf ("%d : %s marked\n", i, names[i]);
    remcnt++;
  }

  fclose(rin);
  printf("%d entries marked.\n", remcnt);

  if (ofname[0]!=0)
    out = myfopen(ofname,"w");
  if (out == NULL)
    return;

  for (i=0;i<nseq;i++){
    if (!doclean && mark[i] == 1){
      fprintf(out, ">%s\n%s\n", names[i], forwards[i]);
    }
    else if (doclean && mark[i] == 0 && names[i]!=NULL){
      fprintf(out, ">%s\n%s\n", names[i], forwards[i]);
    }
  }
  


  return 1;

}


int readTwoFasta(FILE *fastaFile, int hashtype){
  int cnt;
  char ch; 
  int i,j;
  int seqcnt=0, seqlen=0;
  int rseqcnt = 0;
  int clonecnt;
  int maxnamelen;
  int maxlen;
  char dummy[SEQ_LENGTH];
  char str[SEQ_LENGTH];
  char myname[SEQ_LENGTH];
  char myclone[SEQ_LENGTH];
  int index;
  char *thisseq;


  cnt = 0; i=0;
  fprintf(stderr, "Counting sequences.\n");
  maxlen=0; maxnamelen=0;

  rewind(fastaFile);
  /*

  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      fscanf(fastaFile, "%s", dummy);
      if (strlen(dummy)>maxnamelen)
	maxnamelen=strlen(dummy);
      if (seqlen>maxlen)
	maxlen=seqlen;
      cnt++;
      seqlen=0;
      my_fgets(dummy, SEQ_LENGTH, fastaFile);
    }
    else if (!isspace(ch))
      seqlen++;
  }

  seqcnt = cnt;  
  clonecnt = seqcnt;
  */
    //seqcnt/2+1; // divide fwds & reverses 

  maxnamelen = 250;
  clonecnt = MYPRIME;

  if (seqlen>maxlen)
    maxlen=seqlen;

  maxlen = 100000;

  fprintf(stderr, "Allocating memory for %d sequences with max length %d, maxnamelength %d.\n", clonecnt, maxlen, maxnamelen);
  
  
  forwards = (char **) mymalloc((clonecnt) * sizeof(char *));
  
  /*
  for (i=0; i<clonecnt; i++)
    forwards[i] = (char *) mymalloc(maxlen);
  */


  names = (char **) mymalloc((clonecnt) * sizeof(char *));


  mark = (char *) mymalloc((clonecnt) * sizeof(char ));

  memset(mark, '\0', sizeof(char)*clonecnt);

  /*
  for (i=0; i<clonecnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);
  */
  

  thisseq = (char *) mymalloc(maxlen*2);
  
  

  for (i=0; i<clonecnt; i++){
    forwards[i] = NULL;
    names[i] = NULL;
    /*
    forwards[i][0] = 0;
    reverses[i][0] = 0;
    names[i][0] = 0; */
  }
   
  // FIX THE REST for FWD/REV

  fprintf(stderr, "Reading sequences.\n");
  rewind(fastaFile);
  cnt = -1; 

  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      my_fgets(myname, SEQ_LENGTH, fastaFile);
      myname[strlen(myname)-1] = 0;

      //end2clone(myclone, myname);
      
      strcpy(myclone, myname);

      index = hash(myclone , clonecnt, hashtype); // hope it works
      /*
	fprintf(stderr, "\r%d\tof\t%d\tindex\t%d\tname\t%s\tclone\t%s", (cnt+1), clonecnt*2, index, myname, myclone);
      */
      //fprintf(stderr, "\r%d\tof\t%d", (cnt+1),seqcnt);
      if (names[index] == NULL)
	names[index] = (char *) mymalloc(sizeof(char)*(strlen(myclone)+1));
      strcpy(names[index], myclone);

    }
    i = 0;
    if (cnt != 0){
      thisseq[i++] = ch;
      /*
      if (isReverse(myname))
	reverses[index][i++] = ch;
      else
	forwards[index][i++] = ch;
      */
    }
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n'){
	thisseq[i++] = ch;
	/*
	if (isReverse(myname))
	  reverses[index][i++] = ch;
	else
	  forwards[index][i++] = ch;
	*/
      }
    } while (ch != '>');
    
    thisseq[i++] = 0;

    forwards[index] = (char *)mymalloc(sizeof(char)*(strlen(thisseq)+1));
    strcpy(forwards[index], thisseq);
    
    if (ch == '>'){
      cnt++;
      //fprintf(stderr, "\r%d\tof\t%d", (cnt+1), seqcnt );
      if (cnt != seqcnt){
	  my_fgets(myname, SEQ_LENGTH, fastaFile);
	  myname[strlen(myname)-1] = 0;
	  
	  //end2clone(myclone, myname);
	  strcpy(myclone, myname);

	  index = hash(myclone , clonecnt, hashtype); // hope it works
	  if (names[index] == NULL)
	    names[index] = (char *) mymalloc(sizeof(char)*(strlen(myclone)+1));
	  strcpy(names[index], myclone);
	  
      }
    } // if
  } // while

  
  
  fprintf(stderr, "\n[OK] %d sequences read from fasta file.\n",clonecnt);
  fprintf(stderr, "# of collisions: %lld\n", collisions);

  //  free(thisseq); 

  return clonecnt;

}

void my_fgets(char *str, int length, FILE *in){
  char ch;
  int i=0;
  while (i<length && fscanf(in, "%c", &ch)){
    if ((ch==' '  || ch=='\t')  && i!=0){
      if (str[i-1] == ' ' || str[i] == '\t')
	;
      else
	str[i++] = ch;
    }
    else
      str[i++] = ch;
    if (ch == '\n' || ch=='\r')
      break;
  }
  str[i] = 0;
}


int hash(char *this_name, int nseq, int type){
  int i;
  unsigned long sum;
  //int prime=3160979;
  int index;
  unsigned int a,b;
  int doescollide=0;
  int infloop=0;

  sum = 0;
  a=378551;
  b=63689;
  
  if (type == 0){
    for (i=0;i<strlen(this_name);i++){
      if (this_name[i]!=' ')
	sum += i*this_name[i]+i;
    }
  }
  else if (type == 1){
    for (i=0;i<strlen(this_name);i++){
      if (this_name[i]!=' ')
	sum += this_name[i];
    }  
  }
  else if (type == 2){
    for (i=0;i<strlen(this_name);i++){
      if (this_name[i]!=' ')
	sum += i*this_name[i];
    }
  }
  else if (type == 3){
    for (i=0;i<strlen(this_name);i++){
      if (this_name[i]!=' ')
	sum += this_name[i] * pow(7,i); // rabin-karp style
    }
  }
  else if (type == 4){
    for (i=0;i<strlen(this_name);i++){
      if (this_name[i]!=' ')
	sum += this_name[i] * pow(11,i); // rabin-karp style, part 2
    }
  }
  else if (type == 5){
   for (i=0;i<strlen(this_name);i++){
     sum += sum*a + this_name[i];
     a = a*b; //RShash
   }
  }
  else if (type == 6){
    sum=1315423911;
   for (i=0;i<strlen(this_name);i++){
     sum ^= ((sum<<5) + this_name[i] + (sum>>2)); // JShash
   }
  }
  else if (type == 7){
   for (i=0;i<strlen(this_name);i++){
     sum = (sum << 4) + this_name[i];
     if ((a = sum & 0XFF0000000L) != 0)
       sum ^= a>>24;
     sum &= ~a; // ELFhash
   }  
  }
  else if (type == 8){
    a=131;
    for (i=0;i<strlen(this_name);i++){
      sum=(sum*a)+this_name[i]; //bkdrhash
    }
  }
  else if (type == 9){
    for (i=0;i<strlen(this_name);i++){
      sum = this_name[i] + (sum<<6) + (sum<<16) - sum; //sdbmhash
    }
  }
  else if (type == 10){
    for (i=0;i<strlen(this_name);i++){
      sum = ((sum<<5) + sum) + this_name[i]; // djb
    }
    
  }
  else if (type == 11){
    for (i=0;i<strlen(this_name);i++){
      sum=((sum<<5)^(sum>>27))^this_name[i]; //DEK
    }
  }
  else if (type == 12){
    for (i=0;i<strlen(this_name);i++){
      sum = sum<<7 ^ this_name[i];
    }
  }
  else if (type == 13){
    for (i=0;i<strlen(this_name);i++){
      a=0x811C9DC5;
      sum*=a;
      sum^=this_name[i]; // fnv
    }
  }
  else if (type == 14){
    for (i=0;i<strlen(this_name);i++){
      sum ^= ((i&1) == 0)?((sum<<7)^this_name[i]^(sum>>3)):
	(~((sum<<11)^this_name[i]^(sum>>5))); //ap
    }
  }    

  
  index = sum % nseq;

  
  while (names[index] != NULL && strcmp(names[index], this_name)){
    //  fprintf(stderr, "looking %s\n", this_name);
    doescollide=1;
    collisions++;
    index++;
    if (index == nseq){
      index = 0;
      if (infloop){
	printf("infinite loop!!!!!!!!!!!!!\n");
	exit(0);
      }
      infloop=1;
    }
  }
  
  //if (doescollide) collisions++;

  return index;

}

void end2clone(char *clone, char *end){
  int i, len, j;
  int flag = 0;
  char *rloc, *floc;
  char *pass;
  
  len = strlen(end);

  rloc = strstr(end, "R3");
  floc = strstr(end, "F3");
  

  strcpy(clone, end);
  clone[strlen(clone)-3] = 0;

  return;

  if (floc!=NULL || rloc!=NULL){
    
    if (rloc != NULL)
      pass = rloc;
    else
      pass = floc;
    
    i = 0; j=0;
    while (end+i+1 != pass){
      clone[j] = end[i];
      i++; j++;
    }
    // we're at the '.' before REVERSE/FORWARD
    // pass that
    i++;
    // find the '.' after REVERSE/FORWARD
    while (end[i]!='_') i++;
    // don't pass that 
    // copy the rest
    while(i<strlen(end))
      clone[j++] = end[i++];
    // terminate
    clone[j] = 0;
    return;
  }


  j=0;
  for (i=0; i<strlen(end); i++){
    if ((end[i] != 'F' && end[i] != 'R') || flag){
      clone[j++] = end[i];  
    }
    else if (end[i] != 'F' || end[i] != 'R'){
      if (flag)
	clone[j++] = end[i];  
      else{
	flag = 1;
      }
    } 
    
  }
  clone[j] = 0;

  //fprintf(stderr, "name:\t%s\tclone:\t%s\n", end, clone);
  
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
    default: // may be N
      rcomp[len-i-1] = window[i];
      break;
    }    
  rcomp[len] = 0;
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



unsigned int cindex(char ch){
  unsigned int this;
  switch(toupper(ch)){
  case 'A':
    this = 0;
    break;
  case 'C':
    this = 1;
    break;
  case 'G':
    this = 2;
    break;
  case 'T':
    this = 3;
    break;
  default:
    printf("alooooooooooooooooo %c\n", ch);
    assert(0);
    break;
  }
  return this;
}

char reverseindex(int code){
  char ch;
  switch(code){
  case 0:
    ch = 'A';
    break;
  case 1:
    ch = 'C';
    break;
  case 2:
    ch = 'G';
    break;
  case 3:
    ch = 'T';
    break;
  default:
    printf("alooooooooooooooooo %d\n", code);
    assert(0);
    break;
  }
  return ch;
}



FILE *myfopen(char *fname, char *mode){
  FILE *ret;
  ret = fopen(fname, mode);
  if (ret == 0){
    fprintf(stderr, "File %s not found.\n", fname);
    exit(0);
  }
  return ret;
}


void *mymalloc(size_t size){
  void *ret;
  ret = malloc(size);

  if (ret == NULL){
    fprintf(stderr, "Insufficient memory.\n");
    exit (0);
  }

  return ret;
}

