#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MEMUSE 2147483648
#define MEMSCALE 1.5
#define STRLEN 256

typedef struct read{
  char *qname;
  char *seq;
  char *qual;
  char empty;
}_read;

void  fastq_match(char *, char *, int, int);
int load_reads(FILE *, struct read **, int);
static int fastq_qname_comp(const void *, const void *);
void alloc_reads(struct read ***, int);
void realloc_reads(struct read ***, int, int);
void free_reads(struct read ***, int);

void set_str( char** target, char* source)
{
  if( *target != NULL)
    {
      free( ( *target));
    }
  
  ( *target) = ( char*) malloc( sizeof( char) * ( strlen( source) + 1));
  strncpy( ( *target), source, ( strlen( source) + 1));
}
 
int main(int argc, char **argv){
  int num_seq;
  int read_length;


  if (argc != 5) return;
  num_seq = atoi(argv[3]);
  read_length = atoi(argv[4]);

  fastq_match(argv[1], argv[2], num_seq, read_length);
}


void  fastq_match(char *filename1, char *filename2, int num_seq, int read_length){
  struct read **reads1, **reads2;
  struct read **res;
  
  int num_batch;
  int old_batch;
  int num_matched;
  int i;
  int j;

  FILE *f1, *f2;
  FILE *of1, *of2;

  int loaded;

  char *ofilename1, *ofilename2;

  ofilename1 = (char *) malloc (sizeof(char) * (strlen(filename1)+10));
  ofilename2 = (char *) malloc (sizeof(char) * (strlen(filename2)+10));
  
  sprintf(ofilename1, "%s.sorted", filename1);
  sprintf(ofilename2, "%s.sorted", filename2);
  
  of1 = fopen(ofilename1, "w");
  of2 = fopen(ofilename2, "w");

  num_batch = MEMUSE / (read_length * 3 * 2); // 3 comes from qname,seq,qual. 2 comes from pairs

  if (num_batch > num_seq)
    num_batch = num_seq;

  fprintf(stderr, "numseq: %d\tnumbatch: %d\n", num_seq, num_batch);


  alloc_reads(&reads1, num_batch);
  alloc_reads(&reads2, num_batch);

  num_matched = 0;

  f1 = fopen(filename1, "r"); //replace with gfOpen
  f2 = fopen(filename2, "r");

  while (num_matched < num_seq){

    /* load the /1 reads */
    loaded = load_reads(f1, reads1, num_batch);
    if (loaded == 0){
      /* 
	 no new reads are loaded -- the array is full and it can't find more matches
	 extend memory by MEMSCALE so you can load more
      */
      old_batch = num_batch;
      num_batch = num_batch * MEMSCALE;
      if (num_batch > num_seq)
	num_batch = num_seq;
      realloc_reads(&reads1, old_batch, num_batch);
      realloc_reads(&reads2, old_batch, num_batch);
      loaded = load_reads(f1, reads1, num_batch);
    }    
  
    qsort(reads1, num_batch, sizeof(struct read *), fastq_qname_comp); 

    /* load the /2 reads */
    loaded = load_reads(f2, reads2, num_batch);
    
    /* search for the /2 reads within the sorted array of /1 reads */
    for (i=0;i<num_batch;i++){
      if (reads2[i]->qname != NULL && reads2[i]->empty == 0){
	res =  (struct read **) bsearch(&(reads2[i]), reads1, num_batch, sizeof(struct read *), fastq_qname_comp);
	if (res != NULL){
	  fprintf(of1, "@%s/1\n%s\n+\n%s\n", (*res)->qname, (*res)->seq, (*res)->qual);
	  fprintf(of2, "@%s/2\n%s\n+\n%s\n", reads2[i]->qname, reads2[i]->seq, reads2[i]->qual);
	  (*res)->empty = 1;
	  reads2[i]->empty = 1;
	  num_matched++;
	}
      }
    }
    fprintf(stderr, "matched: %d of %d\n", num_matched, num_seq);
  }
  
  fclose(of1);  fclose(of2);
  fclose(f1);  fclose(f2);

  /* 
     add code here to REMOVE the old files (filename1, filename2)
     and rename the ofilename1 with filename1 and ofilename2 with filename2 
     so everything seems to be in-place & temp files are discarded
  */


  free (ofilename1); free(ofilename2);
  
  free_reads(&reads1, num_batch);
  free_reads(&reads2, num_batch);

}


static int fastq_qname_comp(const void *p1, const void *p2){
  struct read *a, *b;
  int i;
  int l1, l2;

  a = * ((struct read **) p1);
  b = * ((struct read **) p2);
  
  return strcmp(a->qname, b->qname);
}

int load_reads(FILE *f1, struct read **reads, int num_batch){
  int i;
  char qname[STRLEN], seq[STRLEN], qual[STRLEN];
  int cnt=0;

  for (i=0; i<num_batch; i++){
    if (reads[i]->empty == 1){
      if (!feof(f1)){
	fscanf(f1, "@%s\n%s\n+\n%s\n", qname, seq, qual);
	qname[strlen(qname)-2] = 0; // get rid of /1 /2
	set_str(&(reads[i]->qname), qname);
	set_str(&(reads[i]->seq), seq);
	set_str(&(reads[i]->qual), qual);
	reads[i]->empty = 0;
	cnt++;
      }
    }
  }
  
  return cnt;
}


void alloc_reads(struct read ***reads, int num_batch){
  /* allocate memory for the read data structure */

  int i;

  *reads = (struct read **) malloc(sizeof(struct read *) * num_batch);

  for (i=0; i<num_batch; i++){
    (*reads)[i] = (struct read *) malloc(sizeof(struct read));
    (*reads)[i]->qname = NULL;
    (*reads)[i]->seq = NULL;
    (*reads)[i]->qual = NULL;
    (*reads)[i]->empty = 1;
  }
}

void realloc_reads(struct read ***reads, int old_batch, int num_batch){

  /* reallocate more memory for the read data structure when needed */

  int i;
  struct read **tmp_read;

  alloc_reads(&tmp_read, num_batch);

  for (i=0; i<old_batch; i++){
    set_str(&(tmp_read[i]->qname), (*reads)[i]->qname);
    set_str(&(tmp_read[i]->seq), (*reads)[i]->seq);
    set_str(&(tmp_read[i]->qual), (*reads)[i]->qual);
    tmp_read[i]->empty = (*reads)[i]->empty;
  }

  free_reads(reads, old_batch);
  *reads = tmp_read;

}

void free_reads(struct read ***reads, int num_batch){

  /* free memory from the read data structure */

  int i;
  for (i=0; i<num_batch; i++){
    free((*reads)[i]->qname);
    free((*reads)[i]->seq);
    free((*reads)[i]->qual);
    free((*reads)[i]);    
  }
  free((*reads));
}
