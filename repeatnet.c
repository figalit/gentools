/*
  here's an initial speed test:
  -------------------------------------
  read time:      2630471
  compact time:   111553
  init time:      259761
  insert time:    16854057
  winflag time:   562600457
  compare time:   370638977
  
  -------------------------------------
  winflags are problem.
  i'm testing to convert it to intarray, and use the index number to check the last inserted
  forward seq; so far a lot faster
  
*/




/*

TODO : also dump the degrees of nodes. extracting it from .viz file sucks

 */

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



typedef struct wpair{
  int id;
  struct wpair *next;
}_wpair;



void my_fgets(char *, int, FILE *);
int readSingleFasta(FILE *, int);
int hash(char *, int, int);
void end2clone(char *, char *);
void rcomp(char *, char *);
int isReverse(char *);
int encode_window(char *);
void decode_window(int, char *);
int getWindow(char *, int, char *, int);
void do_end(int, int);
void insert_wloc(struct wpair **, int, int, int);
void compare_wloc(struct wpair *, struct wpair *, int *, int *, int *);
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
void loadMatrix(char *, char *);
void decode_wins(char *);
void loadNames(char *);
void dumpclones(char *);

char **names;
char **forwards;
char **reverses;
int nseq;
int WS;
int *windowflag;
int wincnt;
struct wpair **winlocs;
int collisions;
int *windowcnt;

int EDGEHIST;

int main(int argc, char **argv){
  
  FILE *in;
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
  
  EDGEHIST = 0;

  collisions=0;
  
  WS = 12;
  decoded = (char *)malloc(WS+1);
  ccut = 1;
  acut = 0;

  //  i=encode_window("AAAAAAAAAAAA");
  //printf("AAAAAAAAAAAA: %d\n", i);

  /*
  i=encode_window2("AAAAAAAAAAAA");
  printf("AAAAAAAAAAAA 2: %d\n", i);
  decode_window(i, decoded);
  printf("decoded : %s\n", decoded);

  j=revcomp_encoded(i);
  printf("\trecvomp: %d\n", j);
  decode_window(j, decoded);
  printf("\tdecoded : %s\n", decoded);


  //i=encode_window("TTTTTTTTTTTT");
  //printf("TTTTTTTTTTTT: %d\n", i);
  i=encode_window2("TTTTTTTTTTTT");
  printf("TTTTTTTTTTTT 2: %d\n", i);
  decode_window(i, decoded);
  printf("decoded : %s\n", decoded);

  j=revcomp_encoded(i);
  printf("\trecvomp: %d\n", j);
  decode_window(j, decoded);
  printf("\tdecoded : %s\n", decoded);


  //i=encode_window("AAAAAAAAAAAT");
  //printf("AAAAAAAAAAAT: %d\n", i);
  i=encode_window2("AAAAAAAAAAAT");
  printf("AAAAAAAAAAAT 2: %d\n", i);
  decode_window(i, decoded);
  printf("decoded : %s\n", decoded);

  j=revcomp_encoded(i);
  printf("\trecvomp: %d\n", j);
  decode_window(j, decoded);
  printf("\tdecoded : %s\n", decoded);

  //i=encode_window("TAAAAAAAAAAA");
  // printf("TAAAAAAAAAAA: %d\n", i);
  i=encode_window2("TAAAAAAAAAAA");
  printf("TAAAAAAAAAAA 2: %d\n", i);
  decode_window(i, decoded);
  printf("decoded : %s\n", decoded);

  j=revcomp_encoded(i);
  printf("\trecvomp: %d\n", j);
  decode_window(j, decoded);
  printf("\tdecoded : %s\n", decoded);

  //i=encode_window("ATCGATCGTAGC");
  //printf("ATCGATCGTAGC: %d\n", i);
  i=encode_window2("ATCGATCGTAGC");
  printf("ATCGATCGTAGC 2: %d\n", i);
  decode_window(i, decoded);
  printf("decoded : %s\n", decoded);

  j=revcomp_encoded(i);
  printf("\trecvomp: %d\n", j);
  decode_window(j, decoded);
  printf("\tdecoded : %s\n", decoded);
  */


  if (argc < 2)
    return 0;

  fname[0]=0;
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
    else if (!strcmp(argv[i], "-t"))
      hashtest=atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      ecut=atof(argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      acut=atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-c"))
      ccut=atof(argv[i+1]);
    else if (!strcmp(argv[i], "-m"))
      MERGE = 1; 
    else if (!strcmp(argv[i], "-loadwin")){
      strcpy(fname, argv[i+1]);
      LOADWIN=1;
    }
    else if (!strcmp(argv[i], "-loadnames")){
      strcpy(namefile, argv[i+1]);
      LOADNAMES=1;
    }
    else if (!strcmp(argv[i], "-loadmatrix")){
      strcpy(fname, argv[i+1]);
      LOADMATRIX=1;
    }
    else if (!strcmp(argv[i], "-getn")){
      strcpy(fname2, argv[i+1]);
    }
    else if (!strcmp(argv[i], "-decode")){
      strcpy(fname, argv[i+1]);
      DECODE=1;
    }
    else if (!strcmp(argv[i], "-compare"))
      COMPARE = 1;
    else if (!strcmp(argv[i], "-edgehist"))
      EDGEHIST = 1;
    else if (!strcmp(argv[i], "-clones")){
      DUMPCLONES = 1;
      strcpy(fname2, argv[i+1]);
    }
  }
  
  
  if(fname[0]==0){
    fprintf(stderr, "input file?\n");
    return 0;
  }


  in = myfopen(fname,"r");


  if (LOADNAMES){
    loadNames(namefile);
  }

  if (DECODE){
    decode_wins(fname);
    return 1;
  }
  
  if (LOADMATRIX){
    loadMatrix(fname, fname2);
    return 1;
  }

  if (LOADWIN){
    do_loaded(in, fname);
    if (COMPARE)
      compare(fname, hashtype, ecut, ccut, acut, MERGE);
    if (DUMPCLONES)
      dumpclones(fname2);
    return 1;
  }

  if (hashtest){
    for (i=3;i<hashtest;i++){
      rewind(in);
      for (j=0;j<nseq;j++)
	free(names[j]);
      free(names), 
      gettimeofday(&start, &tz);  
      collisions = 0;
      nseq = hashTest(in, i);
      gettimeofday(&end, &tz);
      fprintf(stdout, "hash type %d read time %d microseconds.\tCollisions:%d\n", i, ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec)), collisions);
      if (((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec))<mintime){
	mintime = ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec));
	minhash=i;
      }
      if (collisions<mincollide){
	mincollide=collisions;
	mincolhash=i;
      }
    }
    fprintf(stdout, "\n\nmintime: %d microseconds\nbesthash:%d\n",mintime, minhash);
    fprintf(stdout, "\n\nmincollide: %d times\nbesthash:%d\n",mincollide, mincolhash);
    return 0;
  }

  gettimeofday(&start, &tz);  
  nseq = readSingleFasta(in, hashtype);
  gettimeofday(&end, &tz);
  readtime = ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec));
	
  npairs = 0;
  /* compact seqs */

  gettimeofday(&start, &tz);
  sprintf(fname2, "%s.h%d.names", fname, hashtype);
  namelog = fopen(fname2, "w");
  fprintf(namelog, "%d\n",nseq);
  for (i=0;i<nseq;i++){
    fprintf(stderr, "\rCompacting %d\tof\t%d",(i+1),nseq);
    if (names[i]!=NULL){
      if (forwards[i]==NULL){
	free(reverses[i]);
	free(names[i]);
	names[i] = NULL;
      }
      else if (reverses[i]==NULL){
	free(forwards[i]);
	free(names[i]);
	names[i] = NULL;
      }
      else{
	npairs++;
	fprintf(namelog, "%d\t%s\n", i, names[i]);
      }
    }
  }
  fclose(namelog);
  fprintf(stderr, "\n");
  gettimeofday(&end, &tz);
  compacttime = ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec));
  
  
  fprintf(stderr, "Remaining pairs: %d\n", npairs);
  wincnt = pow(4, WS);
  
  fprintf(stdout, "Number of windows:%ld\n", wincnt);

  winlocs = (struct wpair **)malloc(sizeof(struct wpair *)*wincnt);
  windowflag = (int *) malloc(sizeof(int)*wincnt);
  windowcnt = (int *) malloc(sizeof(int)*wincnt);
  
  gettimeofday(&start, &tz);  
  fprintf(stdout, "Initializing arrays...");
  for (i=0;i<wincnt;i++){
    winlocs[i]=NULL;
    windowflag[i] = -1;
    windowcnt[i] = 0;
  }
  fprintf(stdout, "\n");
  fprintf(stderr, "\n");
  gettimeofday(&end, &tz);
  inittime = ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec));

  k=0;
  for (i=0; i<nseq; i++){
    if (names[i] == NULL)
      continue;
    
    // else, both ends are here.
    k++;
    fprintf(stderr, "\rInserting %d\tof\t%d",k,npairs);
    
    gettimeofday(&start, &tz);  
    do_end(i, FORWARD);
    do_end(i, REVERSE);
    gettimeofday(&end, &tz);
    inserttime += ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec));
    
    /* i dont need to flush if I use intarray
    // flush flags
    gettimeofday(&start, &tz);  
    for (j=0;j<wincnt;j++)
      windowflag[j] = 0;
    gettimeofday(&end, &tz);
    winflagtime += ((int)(end.tv_sec*1000000+end.tv_usec)-(int)(start.tv_sec*1000000+start.tv_usec));
    */

    // remove index i, I don't need them anymore.
    // but keep the names for now. 
    free(forwards[i]);
    free(reverses[i]);
    forwards[i]=NULL;
    reverses[i]=NULL;

  }
  fprintf(stderr, "\n\n");
  
  sprintf(fname2, "%s.h%d.winlog", fname, hashtype);
  winlog = fopen(fname2, "w");
  

  fprintf(stderr, "Comparing...");
  fflush(stderr);

  fprintf(winlog,"%d\n",WS);

  FILE *dumphist = fopen("dumphist.txt", "w");
  printf("zero: %d\n", windowcnt[0]);

  for(i=0;i<wincnt;i++){
    if (winlocs[i]==NULL)
      continue;
    dumpwins(winlog, i);
    fprintf(dumphist, "%d\t%d\n", i, windowcnt[i]);
  }
  return 1;

  // THIS FOR-LOOP can be written with pthread  producer/consumer as well
  gettimeofday(&start, &tz);  

  compare(fname, hashtype, ecut, ccut, acut, MERGE);

  gettimeofday(&end, &tz);
  comparetime += ((int)(end.tv_sec)-(int)(start.tv_sec));
  
  fprintf(stderr, "\n");
  

  /*
    http://www.cs.nmsu.edu/~jcook/Tools/pthreads/pc.c
    it was something like this:
    prod=pthread_create

    for (j=0;j<MAXTHREADS;j++){
    threads[i]=pthread_create(j)
    for (j=0;j<MAXTHREADS;j++){
    pthread_join


    producer
    i = 0;
    while (i<wincnt){
    for (j=0;j<MAXTHREADS;j++){
    mutex
    if (threadopen[j])
    threadopen[j]=i++;
    endmutex
    
    } //endfor

    } //endwhile

    cnt=MAXTHREADS
    while (cnt!=0){
    for (j=0;j<MAXTHREADS;j++)
    if threadopen && threads[j]!=null
    pthread_kill(j); threads[j]=null
    cnt--;
    }


    consumer
    myid=j
    mutex
    myjob=threadopen[myid]
    endmutex
    if myjob==0
    pthread_wait
    else
    do myjob


   */


  //TEST OUTPUT 
  /*
  for (i=0;i<nseq;i++){
    if (names[i]!=NULL){
      printf("\n>%s_FORWARD\n", names[i]);
      for (j=0;j<strlen(forwards[i]);j++){
	if (j!=0 && j%80==0)
	  printf("\n");
	printf("%c", forwards[i][j]);
      }
      printf("\n>%s_REVERSE\n", names[i]);
      for (j=0;j<strlen(reverses[i]);j++){
	if (j!=0 && j%80==0)
	  printf("\n");
	printf("%c", reverses[i][j]);
      }
    }
    else
      fprintf(stderr, "can not find name for %i\n", i);
  }
  */
  


  fprintf(stdout,"\n-------------------------------------\n");
  fprintf(stdout,"\tread time:\t%ld usec\n", readtime);
  fprintf(stdout,"\tcompact time:\t%ld usec\n", compacttime);
  fprintf(stdout,"\tinit time:\t%ld usec\n", inittime);
  fprintf(stdout,"\tinsert time:\t%ld usec\n", inserttime);
  fprintf(stdout,"\twinflag time:\t%ld usec\n", winflagtime);
  fprintf(stdout,"\tcompare time:\t%ld sec\n", comparetime);
  fprintf(stdout,"-------------------------------------\n");

  
  return 1;

}


int hashTest(FILE *fastaFile, int hashtype){
  int cnt;
  char myname[SEQ_LENGTH];
  char myclone[SEQ_LENGTH];
  int index;
  char dummy[SEQ_LENGTH];
  int i;

  cnt = 0;
  fprintf(stderr, "Counting sequences.\n");

  while(fscanf(fastaFile, "%s", dummy)>0)
    cnt++;

  names = (char **) malloc((cnt) * sizeof(char *));
  rewind(fastaFile);
  i=0;
  while(fscanf(fastaFile, "%s", dummy)>0){
    end2clone(myclone, dummy);
    index = hash(myclone , cnt, hashtype); // hope it works    
    i++;
    fprintf(stderr, "\r%d\tof\t%d", i,cnt);
    if (names[index] == NULL)
      names[index] = (char *) malloc(sizeof(char)*(strlen(myclone)+1));
    strcpy(names[index], myclone);
  }

  fprintf(stderr, "\n");
}

int readSingleFasta(FILE *fastaFile, int hashtype){
  int cnt;
  char ch; 
  int i,j;
  int seqcnt=0, seqlen=0;
  int clonecnt;
  int maxnamelen;
  int maxlen;
  char dummy[SEQ_LENGTH];
  char str[SEQ_LENGTH];
  char myname[SEQ_LENGTH];
  char myclone[SEQ_LENGTH];
  int index;
  char *thisseq;
  char *thisrevseq;

  cnt = 0; i=0;
  fprintf(stderr, "Counting sequences.\n");
  maxlen=0; maxnamelen=0;
  rewind(fastaFile);
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
    //seqcnt/2+1; // divide fwds & reverses 
  if (seqlen>maxlen)
    maxlen=seqlen;

  //length = (int *) malloc((clonecnt) * sizeof(int));


  //int hash(char *this_name, int nseq, int mode)

  cnt = 0; i=0;

  fprintf(stderr, "Allocating memory for %d sequences with max length %d, maxnamelength %d.\n", clonecnt, maxlen, maxnamelen);
  
  
  forwards = (char **) malloc((clonecnt) * sizeof(char *));
  
  /*
  for (i=0; i<clonecnt; i++)
    forwards[i] = (char *) malloc(maxlen);
  */

  reverses = (char **) malloc((clonecnt) * sizeof(char *));
  
  /*
  for (i=0; i<clonecnt; i++)
    reverses[i] = (char *) malloc(maxlen);
  */

  names = (char **) malloc((clonecnt) * sizeof(char *));

  /*
  for (i=0; i<clonecnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);
  */
  

  thisseq = (char *) malloc(maxlen+1);
  thisrevseq = (char *) malloc(maxlen+1);
  
  

  for (i=0; i<clonecnt; i++){
    forwards[i] = NULL;
    reverses[i] = NULL;
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

      end2clone(myclone, myname);
      
      index = hash(myclone , clonecnt, hashtype); // hope it works
      /*
	fprintf(stderr, "\r%d\tof\t%d\tindex\t%d\tname\t%s\tclone\t%s", (cnt+1), clonecnt*2, index, myname, myclone);
      */
      fprintf(stderr, "\r%d\tof\t%d", (cnt+1),seqcnt);
      if (names[index] == NULL)
	names[index] = (char *) malloc(sizeof(char)*(strlen(myclone)+1));
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

    if (isReverse(myname)){
      rcomp(thisseq, thisrevseq);
      reverses[index] = (char *)malloc(sizeof(char)*(strlen(thisseq)+1));
      strcpy(reverses[index], thisrevseq);
    }
    else{
      forwards[index] = (char *)malloc(sizeof(char)*(strlen(thisseq)+1));
      strcpy(forwards[index], thisseq);
    }
    
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", (cnt+1), seqcnt );
      if (cnt != seqcnt){
	  my_fgets(myname, SEQ_LENGTH, fastaFile);
	  myname[strlen(myname)-1] = 0;
	  
	  end2clone(myclone, myname);
	  
	  index = hash(myclone , clonecnt, hashtype); // hope it works
	  if (names[index] == NULL)
	    names[index] = (char *) malloc(sizeof(char)*(strlen(myclone)+1));
	  strcpy(names[index], myclone);
	  
      }
    } // if
  } // while
  
  
  fprintf(stderr, "\n[OK] %d sequences read from fasta file.\n",clonecnt);

  free(thisseq); free(thisrevseq);

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
    if (index == nseq)
      index = 0;
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

  rloc = strstr(end, "REVERSE");
  floc = strstr(end, "FORWARD");
  
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
    while (end[i]!='.') i++;
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

int encode_window(char *window){

  return encode_window2(window);


  int len = strlen(window);
  int i;
  int ret = 0;
  int this;
  // OLD:: NOTE: this actually reverses the window
  // 000..00: AAA..AA
  // 111..11: TTT..TT
  // 000..11: TAAA...AA


  // FIXED AS: 
  // 000..00: AAA..AA
  // 111..11: TTT..TT
  // 000..11: AAA..AT
  // 110..00: TAA..AA
  // THE FIRST BIT IS 2*WS
  // ANY BITS BETWEEN 32-2WS-1 are 0

  // need to rerun g248 & OGA

  //A:0, C:1, G:2, T:3
  for (i=0;i<len;i++){
    switch(toupper(window[i])){
    case 'A':
      this = 0;
      break;
    case 'C':
      this = 1 * pow(4, (WS-i-1));
      break;
    case 'G':
      this = 2 * pow(4, (WS-i-1));
      break;
    case 'T':
      this = 3 * pow(4, (WS-i-1));
      break;
    }
    ret += this;
  }
  return ret;
}

int encode_window2(char *window){
  int len = strlen(window);
  unsigned int code;
  //int totalbits = sizeof(int) * 8;
  unsigned int this;
  int shiftleft;
  //int shiftright;
  int i;
  unsigned int left; //, right;

  code = 0;
  
  for (i=0;i<len;i++){
    this = cindex(window[i]);
    shiftleft = 2*(len-i-1);
    //shiftright = totalbits - 2; 
    left = this << shiftleft;
    code = code | left;
  }
  return code;
}

int revcomp_encoded(unsigned int code){
  unsigned int n, this;
  int totalbits = sizeof(int) * 8;
  int shiftright;
  int shiftleft;
  int i;
  char thiswin[20];
  char reverse[20];

  decode_window(code, thiswin);
  rcomp(thiswin, reverse);
  return encode_window2(reverse);


  n = 0;
  this = code;

  for (i=0;i<WS;i++){
    shiftright = 2*(WS-i-1);
    this = this >> shiftright;
    this = ~this;

    shiftleft = totalbits - 2;
    this = this << shiftleft;

    shiftright = totalbits - 2*(i+1);
    this = this >> shiftright;

    n = n | this;
  }

  /*
  n = code;
  
  n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
  n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
  n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
  n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
  
  shiftright = totalbits - 2*WS;
  //n = ~n;
  n = n >> shiftright;
  */

  return n;

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

int getWindow(char *genome, int glen, char *window, int win_start){
  int i;
  int win_end;
  
  win_end = win_start+WS;
  if (win_start >= glen)
    return 0;
  if (win_end>=glen)
    return 0;
  //    win_end = glen;
  for (i=win_start;i<win_end;i++){
    if (genome[i] == 'N' || genome[i] == 'O'){
      return 0;
    }
    window[i-win_start] = genome[i];
    
  }

  window[i-win_start] = 0;


  return 1;
}

void do_end(int index, int orient){
  char *window;
  int i;
  int willdo;
  int flen;
  int code;
  if (orient == FORWARD)
    flen = strlen(forwards[index]);
  else
    flen = strlen(reverses[index]);

  window = (char *) malloc(sizeof(char)*WS);
  
  i=0;
  if (orient == FORWARD){
    while (i<flen-WS+1){
      willdo = getWindow(forwards[index], flen, window, i);  
      
      if (willdo){
	code = encode_window(window);
	windowflag[code] = index;
      }
      i++;
    }
  }

  else{
    while (i<flen-WS+1){
      willdo = getWindow(reverses[index], flen, window, i);  
      if (willdo){
	code = encode_window(window);
	if (windowflag[code] == index){
	  // index is the sequence number 0...nseqs-1
	  // code is the window code AAA..AA=0, etc.
	  //printf("DOUBLE SIDED %s\n", window);
	  insert_wloc(&(winlocs[code]), index, code, CREATE);
	}
      }
      i++;
    }
  }

  free(window);
}

void insert_wloc(struct wpair **winloc, int index, int code, int mode){
  //printf("Code: %d\t index: %d\n", code, index);
  struct wpair *new;
  if ((*winloc) == NULL){
    (*winloc) = (struct wpair *) malloc(sizeof(struct wpair));
    (*winloc)->id = index;
    (*winloc)->next = NULL;
    if (mode == CREATE)
      windowcnt[code]++;
    //printf("insert %d\tcnt %d\t code %d\n", index, windowcnt[code], code);
    return;
  }
  
  if ((*winloc)->id == index) // no double insertions
    return;
  
  new = (struct wpair *) malloc(sizeof(struct wpair));
  new->id = index;
  new->next = (*winloc);
  (*winloc) = new;
  if (mode == CREATE)
    windowcnt[code]++;
  //printf("insert %d\tcnt %d\t code %d\n", index, windowcnt[code], code);

}


void compare_wloc(struct wpair *wloc1, struct wpair *wloc2, int *element1, int *element2, int *eboth){
  struct wpair *w1, *w2;
  int e1,e2,eb;
  e1 = 1; e2 = 1; eb = 0;

  w1 = wloc1;
  w2 = wloc2;

  // they are both sorted in DESCENDING ORDER

  // FOR LOADED WINDOWS THEY SHOULD BE ASCENDING ORDER
  // FIX THIS!!!!
  // ABOVE IS FIXED WORKS ONLY FOR LOADED NOW. MAKE THIS OPTIONAL

  // REMOVE e1, e2 NOT NECESSARY ANYMORE

  while (w1!=NULL && w2!=NULL){
    if (w1->id == w2->id){
      eb++;
      e1++;
      e2++;
      w1=w1->next;
      w2=w2->next;
    }
    
    else if (w1->id < w2->id){
      // we wont find w1.id in the rest of w2 anyway
      e1++;
      w1=w1->next;
    }
    else{
      // we wont find w2.id in the rest of w1 anyway
      e2++;
      w2=w2->next;
    }
    
  }

  (*element1) = e1;
  (*element2) = e2;
  (*eboth) = eb;
}

void dumpwins(FILE *winlog, int index){
  struct wpair *w1;
  int separator=-1;
  w1 = winlocs[index];

  fwrite(&index, sizeof(int), 1, winlog);
  fwrite(&(windowcnt[index]), sizeof(int), 1, winlog);
  while (w1!=NULL){
    fwrite(&(w1->id), sizeof(int), 1, winlog);
    w1=w1->next;
  }
  //fwrite(&separator, sizeof(int), 1, winlog);
}

void loadwins(FILE *winlog){
  int cnt;
  int index;
  int myid;
  char ch;
  int i;
  fscanf(winlog, "%d", &WS);
  // pass newline
  fscanf(winlog, "%c", &ch);
  printf("read WS %d\n", WS);
  while (!feof(winlog)){
    fread(&index, sizeof(int), 1, winlog);
    fread(&cnt, sizeof(int), 1, winlog);
    //printf("read %d\t%d\n", index,cnt);
    windowcnt[index] = cnt;
    //printf("read %d\t%d\n", index, windowcnt[index]);
    for (i=0;i<cnt;i++){
      fread(&myid, sizeof(int), 1, winlog);
      insert_wloc(&(winlocs[index]), myid, index, LOAD);      
    }   
  }
}


void do_loaded(FILE *in, char *fname){
  FILE *histogram;
  int i;
  char fname2[SEQ_LENGTH];

  wincnt = pow(4, WS);

  winlocs = (struct wpair **)malloc(sizeof(struct wpair *)*wincnt);
  windowflag = (int *) malloc(sizeof(int)*wincnt);
  windowcnt = (int *) malloc(sizeof(int)*wincnt);
  // CALKAN: Jun 13, 12:37pm.
  // something's wrong with counter
  
  fprintf(stdout, "Loading windows...");
  fflush(stdout);

  loadwins(in);
  
  sprintf(fname2, "%s.histogram", fname);
  histogram = fopen(fname2, "w");
  


  for (i=0;i<wincnt;i++){
    fprintf(histogram, "%d\n", windowcnt[i]);
  }
  fclose(histogram);
  fprintf(stdout, "\nWindows loaded.\n");
  
}

void decode_window(int code, char *window){
  int i;
  int shiftleft, shiftright;
  unsigned int left, right;
  int totalbits = sizeof(int)*8;
  for (i=0;i<WS;i++){
    shiftleft = totalbits - 2*WS + 2*i;
    shiftright = totalbits - 2; 
    left = code << shiftleft;
    right = left >> shiftright;
    window[i] = reverseindex(right);
  }
  window[WS]=0;
}



void compare(char *fname, int hashtype, float ecut, int ccut, int acut, int merge){
  int i,j ;
  char fname2[SEQ_LENGTH];
  FILE *graph, *matlog, *edgelog;
  int e1, e2, eboth;
  float eweight;
  float thiscut=ecut;

  if (merge)
    compactwins();
  
  if (acut!=0) thiscut=acut;
  
  fprintf(stdout, "acut: %d\nthiscut: %d\n", acut, (int)thiscut);

  if (!merge)
    sprintf(fname2, "%s.h%d.cut%d.e%d.viz", fname, hashtype, ccut, (int)(thiscut*100));
  else
    sprintf(fname2, "%s.h%d.cut%d.e%d.merged.viz", fname, hashtype, ccut, (int)(thiscut*100));
  graph = fopen(fname2, "w");
  if (!merge)
    sprintf(fname2, "%s.h%d.cut%d.e%d.matrix", fname, hashtype, ccut, (int)(thiscut*100));
  else
    sprintf(fname2, "%s.h%d.cut%d.e%d.merged.matrix", fname, hashtype, ccut, (int)(thiscut*100));
  matlog = fopen(fname2, "w");


  if (EDGEHIST){
    if (!merge)
      sprintf(fname2, "%s.h%d.cut%d.e%d.edges", fname, hashtype, ccut, (int)(thiscut*100));
    else
      sprintf(fname2, "%s.h%d.cut%d.e%d.merged.edges", fname, hashtype, ccut, (int)(thiscut*100));
    edgelog = fopen(fname2, "w");
  }


  fprintf(graph, "graph G{\n");
  //fprintf(graph, "\tnode[style=filled,label=\"\"];\n");
  fprintf(matlog,"%d\n",WS);


  fprintf(stdout, "Comparing...");
  fflush(stdout);

  for(i=0;i<wincnt;i++){
    /*
    if (winlocs[i]==NULL)
    continue; */
    if (windowcnt[i]<ccut)
      continue;
    //dumpwins(winlog, i);

    for (j=i+1;j<wincnt;j++){
      if (windowcnt[j]<ccut)
	continue;
      if (winlocs[j]==NULL)
	continue;
      compare_wloc(winlocs[i], winlocs[j], &e1, &e2, &eboth);

      e1 = windowcnt[i];
      e2 = windowcnt[j];

      eweight=2*(float)eboth/(float)(e1+e2);

      if (acut!=0) eweight=eboth;

      if (eweight>thiscut){
	// if ecut=0.0, then return any overlap (1)
	/* TODO:
	   
	dump the matrix representation in binary format
	done; it may be useful to dump all e1,e2,eb values as well
	didn't decide yet.
	..
	and, oh, yes, make this thing faster. even the linked list insertion
	is slow; a huge portion of the slowness is probably because of windowflags
	...
	try producer/consumer pthreads 
	..
	parameterization, WS, input, etc. in progress
	
	dump winloc array in binary format
	-> done, but not optimal :) 
	-- hash is in good condition now, types 4 to 14 all look good and have similar speeds
	-- binary outputs are not tested. there's output, we need a decoder :)
	 */


	fprintf(graph, "\t%d -- %d;\n", i, j);
	//fprintf(matlog, "%d\t%d\t%f\n",i,j,(2*(float)eboth/(float)(e1+e2)));
	//eweight=2*(float)eboth/(float)(e1+e2);
	fwrite(&i,sizeof(int),1,matlog);
	fwrite(&j,sizeof(int),1,matlog);
	//fwrite(&eweight,sizeof(float),1,matlog);
	fwrite(&eboth,sizeof(int),1,matlog);
	if(EDGEHIST)
	  fprintf(edgelog, "%d\n", eboth);
      }
    }    
  }

  fprintf(graph, "}\n");
  fclose(graph);
  fclose(matlog);
  fprintf(stdout, "\nCompared...\n");
  
}

void compactwins(void){
  // concatenate linked lists of kmers that are revcomp'ed
  // use concatlocations from pfast
  // do this for loaded wins for now.
  // the numbers are in ASCENDING ORDER

  int i;
  int rci;

  fprintf(stdout, "Merging...");

  for (i=0;i<wincnt;i++){
    if (windowcnt[i]==0)
      continue;
    rci = revcomp_encoded(i);
    if (windowcnt[rci]==0)
      continue;
    if (rci<=i)
      continue;
    merge(i, rci);
  }
  
  fprintf(stdout, "\nMerged.\n");
}

void merge(int i, int rci){
  struct wpair *w1, *w2, *new, *w1p;
  // winloc_f = winloc_f+winloc_r
  w1 = winlocs[i];
  w1p = w1;
  w2 = winlocs[rci];
  
  while (w2 != NULL){
    if (w2->id < w1->id){
      new = (struct wpair *) malloc(sizeof(struct wpair));
      new->id = w2->id;
      new->next = w1;
      if (w1p == winlocs[i]){
	winlocs[i] = new;
	w1p = winlocs[i];
      }
      else{
	w1p->next = new;
	w1 = new;
      }
      w2 = w2->next;
    } 
    else if (w2->id > w1->id){
      // no copy here, w2 stays the same, w1 moves
      w1p = w1;
      w1 = w1->next;
    }
    else{
      // they are equal; move along
      w1p = w1;
      w1 = w1->next;
      w2 = w2->next;
    }
    if (w1==NULL && w2!=NULL){
      w1p->next = copyWins(w2);
      break;
    }
  }
  
  freewins(winlocs[rci]);
  windowcnt[i] += windowcnt[rci];
  windowcnt[rci] = 0;

}

struct wpair *copyWins(struct wpair *w){
  struct wpair *new;
  struct wpair *ret;
  struct wpair *this;
  struct wpair *current;

  if (w==NULL)
    return NULL;
 
  new = (struct wpair *) malloc(sizeof(struct wpair));
  new->id = w->id;
  new->next = NULL;
  ret = new;
  this = ret;
  current = w->next;

  while(current != NULL){
    new = (struct wpair *) malloc(sizeof(struct wpair));
    new->id = current->id;
    new->next = NULL;
    this->next = new;
    this = this->next;
    current = current->next;
  }

  return ret;
}

void freewins(struct wpair *w){
  struct wpair *tmp;
  while (w!=NULL){
    tmp = w->next;
    free(w);
    w = tmp;
  }
}

void loadMatrix(char *fname, char *fname2){
  char ch;
  FILE *matlog;
  FILE *neighbor;
  FILE *nfile;
  char name[200];
  int i, j, eboth;
  int nsearch[200]; // for now
  int cnt=0;
  int k;
  FILE *degrees;
  int *matdegrees;

  fprintf(stdout, "Loading matrix...");
  fflush(stdout);

  matlog = myfopen(fname, "r");

  if (fname2[0]!=0){
    neighbor = myfopen(fname2, "r");
    while (fscanf(neighbor, "%d", &(nsearch[cnt++])) > 0)
      ;
    fclose(neighbor);
    sprintf(name, "%s.neighbors", fname2);
    nfile = myfopen(name, "w");
  }

  fscanf(matlog, "%d", &WS);
  fscanf(matlog, "%c", &ch); // skip newline
  wincnt = pow(4, WS);
  
  matdegrees = (int *)malloc(sizeof(int)*wincnt);
  for (i=0;i<wincnt;i++) matdegrees[i]=0;

  while (!feof(matlog)){
    fread(&i, sizeof(int), 1, matlog);
    fread(&j, sizeof(int), 1, matlog);
    matdegrees[i]++; matdegrees[j]++;
    fread(&eboth, sizeof(int), 1, matlog);    
    for (k=0;k<cnt;k++){ 
      if (i==nsearch[k]){
	fprintf(nfile, "%d\n", j);
	break;
      }
      else if (j==nsearch[k]){
	fprintf(nfile, "%d\n", i);
	break;
      }
    }
  }
  fclose(matlog);
  if (fname2[0]!=0) fclose(nfile);
  fprintf(stdout, "\nMatrix loaded.\n");

  sprintf(name, "%s.degrees", fname);
  degrees = fopen(name, "w");
  for (i=0;i<wincnt;i++) 
    if (matdegrees[i]!=0)
      fprintf(degrees, "%d\t%d\n", i, matdegrees[i]);

  free(matdegrees);

}

void mds(){
  // find maximum density subgraph
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

void decode_wins(char *fname){
  int i;
  char decoded[20];
  FILE *in;
  FILE *out;
  char name[200];
  sprintf(name, "%s.decoded", fname);
  in = myfopen(fname, "r");
  out = myfopen(name, "w");

  while (fscanf(in, "%d", &i)>0){
    decode_window(i, decoded);
    fprintf(out, ">%d\n%s\n", i, decoded);
  }
  fclose(in); fclose(out);
}

void dumpclones(char *fname){
  FILE *in;
  FILE *out;
  int i;
  char name[200];
  struct wpair *w;

  // for now, i'm assuming names[] are loaded already

  sprintf(name, "%s.clones", fname);
  
  in = myfopen(fname, "r");
  out = myfopen(name, "w");
  
  while(fscanf(in, "%d", &i) > 0) {
    if (windowcnt[i]!=0 && winlocs[i]!=NULL){ // over cautious
      w = winlocs[i];
      while (w!=NULL){
	fprintf(out, "%s\n", names[w->id]);
	w = w->next;
      }
    }
  }
  fclose(in); fclose(out);
}


void loadNames(char *fname){
  FILE *in; 
  char thisname[SEQ_LENGTH];
  int i;
  in = fopen(fname, "r");

  fscanf(in, "%d", &nseq);
  names = (char **) malloc((nseq) * sizeof(char *));
  fprintf(stdout, "Loading %d names...", nseq);
  fflush(stdout);

  while (fscanf(in, "%d %s", &i, thisname) > 0){
    names[i] = (char*) malloc((strlen(thisname)+1) * sizeof(char));
    strcpy(names[i], thisname);
  }
  
  fclose(in);

  fprintf(stdout, "\nNames loaded.\n");
}
