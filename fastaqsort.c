#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SEQLEN 200

typedef struct spair{
  char *name;
  char *seq;
}_spair;

struct spair **seqs;

static int maxfunc(const void *p1, const void *p2);

int main(int argc, char **argv){
  FILE *fp;
  int i;
  char fname[SEQLEN];
  int nseq;
  char sequence[SEQLEN]; char name[SEQLEN];
  int uniq=0;
  int realuniq=0;
  char prevseq[SEQLEN];

  fname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-u"))
      uniq=1; 
    else if (!strcmp(argv[i], "-uu"))
      realuniq=1; 
    
  }
  if (fname[0]==0) return 0;

  fp = fopen(fname, "r");

  i=0;
  fprintf(stderr, "counting sequences..");

  while (fscanf(fp, ">%s\n%s\n", name, sequence) > 0 ) i++;

  nseq = i;
  fprintf(stderr, "   %d seqs\nLoading ..", i);
  rewind (fp);
  i=0;

  seqs = (struct spair ** ) malloc (sizeof (struct spair *) * nseq);

  while (fscanf(fp, ">%s\n%s\n", name, sequence) > 0 ){
    seqs[i] = (struct spair * ) malloc (sizeof (struct spair));
    seqs[i]->name = (char *) malloc (sizeof (char) * (strlen (name)+1));
    seqs[i]->seq = (char *) malloc (sizeof (char) * (strlen (sequence)+1));
    strcpy(seqs[i]->name, name);
    strcpy(seqs[i]->seq, sequence);
    i++;
  }
  fprintf(stderr, "  done. \n");

  if (uniq) fprintf(stderr, "first\n");
  if (realuniq) fprintf(stderr, "unique\n");

  fprintf(stderr, "Sorting ..");
  qsort(seqs, nseq, sizeof(struct spair *), maxfunc);
  fprintf(stderr, "  done.\nDumping ..");
  prevseq[0]=0;
  for (i=0;i<nseq;i++){
    //if (strstr(seqs[i]->seq, "N"))
    //continue;
    if ((uniq || realuniq) && !strcmp(seqs[i]->seq, prevseq))
      continue;
    else if (realuniq && i!= nseq-1 && !strcmp(seqs[i]->seq, seqs[i+1]->seq))
      continue;
    else
      printf(">%s\n%s\n", seqs[i]->name, seqs[i]->seq);
    strcpy(prevseq, seqs[i]->seq);
  }
  fprintf(stderr, "  done.\n");
  
  //  nseq = readSingleFasta(fp);

}


static int maxfunc(const void *p1, const void *p2){
  struct spair *a, *b;

  int ret;
  a = *((struct spair **)p1);
  b = *((struct spair **)p2);


  ret = memcmp(a->seq, b->seq, strlen(a->seq));

  return ret;


}



