#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define SEQ_LENGTH 150
#define MISMATCH 1000


typedef struct cmember{
  int seqno; // sequence number from names and seqs
  struct cmember *next;
}scmember;

typedef struct cluster{
  int nmember; // no of members
  struct cmember *members;
  struct cluster *next;
}scluster;

char **names;
char **seqs;


int readSingleFasta(FILE *);
int my_strcmp(char *, char *, int);
void alloc_cmember(struct cmember **);
void alloc_cluster(struct cluster**);
void insert_cmember(struct cluster **, int);
struct cluster * find_cluster(struct cluster *, int);



int main(int argc, char **argv){
  char str[1000];
  FILE *fasta;
  FILE *log;
  int i,j;
  int count;
  int nseq;
  int mismatch;
  int score;
  struct cluster *sets;
  struct cluster *prevcluster;
  struct cluster *current;
  struct cmember *cm;
  int iflag;
  int isInserted;
  

  if (argc != 3){
    fprintf(stderr, "Clusters duplicate sequences with some mismatch threshold.\n");
    fprintf(stderr, "Last update: Nov 14, 2005.\n");
    fprintf(stderr, "%s [fasta file] [mismatch threshold]\n", argv[0]);
    exit(0);
  }
  
  fasta = fopen(argv[1], "r");
  sprintf(str, "%s-m%s.clusters.log", argv[1],argv[2]);
  log = fopen(str, "w");
  mismatch = atoi(argv[2]);


  //alloc_cluster(&sets);

  sets = NULL;
  nseq = readSingleFasta(fasta);
  fprintf(stderr, "%d monomers are read into memory.\n", nseq);

  i = 0;

  //insert_cmember(&sets, 1); // first guy in


  for (i=0;i<nseq;i++){

    if (sets == NULL){ 
      alloc_cluster(&sets);
      sets->nmember++;
      alloc_cmember(&(sets->members));
      sets->members->seqno=i;
      continue;
    }
    
    isInserted = 0;
    current = sets;
    while (current != NULL){
      iflag = 0;
      cm = current->members;
      while (cm != NULL){
	score = my_strcmp(seqs[cm->seqno], seqs[i], mismatch);
	if (!score)
	  iflag = 1;
	else{
	  iflag = 0;
	  break; 
	}
	cm = cm->next;
      }
      if (iflag){ // insertable
	insert_cmember(&current, i);
	isInserted = 1;
	break;
      }
      prevcluster = current;
      current = current->next;
    }
    // otherwise, current is now NULL, i is not inserted to any
    // existing place, so create a new one
    
    if (!isInserted){
      alloc_cluster(&current);
      current->nmember++;
      alloc_cmember(&(current->members));
      current->members->seqno=i; 
      prevcluster->next = current;
    }
    printf("\rDoing something %d%%", (int)((float)(i+1)/(float)(nseq)*100));
  }


  /*
  
  for (i=0;i<nseq;i++){
    for (j=i+1;j<nseq;j++){
      //score = my_strcmp(seqs[i], seqs[j], mismatch);
      score = strcmp(seqs[i], seqs[j]);
      if (score == mismatch){
	// if I can live with this
	// let's try to do this assuming mismatch = 0 
	// the other way will be a "little" messier.
	current = find_cluster(sets, i);
	insert_cmember(&current, j);
      }
    }
    printf("\rDoing something %d%%", (int)((float)(i+1)/(float)(nseq)*100));
  }

  */

  printf("\n");
  current = sets;
  while (current != NULL){
    if (current->nmember > 100){
      fprintf(log, "\nCluster size: %d\n------------\n", current->nmember);
      cm = current->members;
      while (cm != NULL){
	fprintf(log, "Member %5d -=- %s\n", cm->seqno, names[cm->seqno]);
	cm = cm->next;
      }
    }
    current = current->next;
  }



  /*
  while (i<nseq){
    count = 1;
    if (!strcmp(seqs[i], current)){
      if (count == 2)
	fprintf(log, "\nCLUSTER: %d\n----------\n", count);
      
      fprintf(log, "%s\n", names[i]);
      count++;
    }
    printf("\rDoing something %d%%", (int)((float)(i+1)/(float)(nhor)*100));
    fflush(stdout);
  }
  */

  printf("\n");
  
  return 1;
}



int readSingleFasta(FILE *fastaFile){
  int cnt;
  char ch; 
  int i;
  int seqcnt=0, seqlen=0;

  cnt = 0; i=0;

  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }

  seqcnt = cnt;
  seqlen = 200;

  cnt = 0; i=0;

  rewind(fastaFile);
  fprintf(stderr, "seqcnt: %d seqlen: %d\n", seqcnt, seqlen);

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen);
  

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
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


  return seqcnt;

} 


int my_strcmp(char *s1, char *s2, int mismatch){
	      // return 0 if match is ok
	      // 1 if not ok enough
	      // backward compability thingy
  
  int mcnt=0;
  int i, len;

  if (mismatch == 0)
    return strcmp(s1,s2);

  len = strlen(s1);
  if (strlen(s2) != len)
    return 1;

  for (i=0;i<len;i++){
    if (s1[i] != s2[i]) mcnt++;
    if (mcnt > mismatch) return 1;
  }

  return 0;

}




void alloc_cmember(struct cmember **cm){
  (*cm) = (struct cmember *) malloc (sizeof(struct cmember));
  (*cm)->next = NULL;
  (*cm)->seqno=-1;
}

void alloc_cluster(struct cluster **cl){
  (*cl) = (struct cluster *) malloc (sizeof(struct cluster));
  (*cl)->members = NULL;
  (*cl)->next = NULL;
  (*cl)->nmember = 0;
}

void insert_cmember(struct cluster **cl, int seqno){
  struct cmember *cm2;
  struct cmember *new;

  if ((*cl)->members == NULL){
    alloc_cmember(&((*cl)->members));
    (*cl)->members->seqno = seqno;
    ((*cl)->nmember)++;
    return;
  }
  cm2 = (*cl)->members;

  while(cm2->next != NULL)   
    cm2 = cm2->next;
  
  alloc_cmember(&new);
  new->seqno = seqno;
  cm2->next = new;

  ((*cl)->nmember)++;
  //printf("inserting %d to cluster, nmem: %d\n", seqno, (*cl)->nmember);
}

struct cluster * find_cluster(struct cluster *sets, int seqno){
  struct cluster *current;
  struct cluster *prev;
  struct cmember *cm;
  current = sets;

  /*
  if (current->nmember == 0)
    return current; // then this is the first guy

  else{ 
  */

  while (current != NULL){
    if (current->nmember != 0){
      cm = current->members;
      while (cm!=NULL){
	if (cm->seqno == seqno){
	  //printf("FOUND SEQNO %d IN CLUSTER NMEMBER %d\n", seqno, current->nmember);
	  return current;
	}
	cm = cm->next;
      }
    }
    prev = current;
    current = current->next;
  }

  // not found, and current is NULL and prev keeps the last one
  // create new cluster, link to prev and return it
  alloc_cluster(&current);
  current->nmember++;
  alloc_cmember(&(current->members));
  current->members->seqno=seqno;
  prev->next = current;
  
  //  }


  return current;

}

