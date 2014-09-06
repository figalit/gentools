#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <zlib.h>
#include <time.h>
#include <string.h>

#define MAXSEQ 275000000
//#define MAXSEQ 30000000
#define MAXLEN 5000

int GZ=0;

typedef struct spair{
  char *name;
  char *seq;
  char *qual;
  unsigned char len;
}_spair;

struct spair **seqs;

static int maxfunc(const void *p1, const void *p2);


void revcomp(char *seq, char *qual);


int main(int argc, char **argv){
  FILE *fp;
  int i,j;
  char fname[MAXLEN];
  int nseq;
  char sequence[MAXLEN]; char name[MAXLEN]; char revseq[MAXLEN];
  int uniq=0;
  char fqual[MAXLEN]; char rqual[MAXLEN];
  int realuniq=0;
  char prevseq[200];
  int seqlen;
  char line[MAXLEN];
  FILE *clean;
  FILE *dup;
  char fname2[MAXLEN];
  int crop=0;
  char *ch;
  int tilecnt=1;
  int tilechunk=0;
  int tilecheck=0;
  int offset = 0;

  int x; int y;
  int rc=0;

  fname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-u"))
      uniq=1; 
    else if (!strcmp(argv[i], "-uu"))
      realuniq=1; 
    else if (!strcmp(argv[i], "-gz"))
      GZ=1; 
    else if (!strcmp(argv[i], "-crop"))
      crop = atoi(argv[i+1]); 
    else if (!strcmp(argv[i], "-rc"))
      rc = 1;
    
  }
  if (fname[0]==0) return 0;

  if (GZ)
    fp = gzopen(fname, "r");
  else
    fp = fopen(fname, "r");



  //  fname[strlen(fname)-3] = 0;
  
  ch = NULL;
  ch = strrchr(fname, '.');
  if (ch != NULL)
    (*ch) = 0;

  if (GZ){
    sprintf(fname2, "%s.read1.gz", fname);
    clean = gzopen(fname2, "w");
    sprintf(fname2, "%s.read2.gz", fname);
    dup = gzopen(fname2, "w");
  }
  else{
    sprintf(fname2, "%s.read1.fq", fname);
    clean = fopen(fname2, "w");
    sprintf(fname2, "%s.read2.fq", fname);
    dup = fopen(fname2, "w");
  }

  i=0;

  
  

  seqs = (struct spair ** ) malloc (sizeof (struct spair *) * MAXSEQ);
  nseq=0;

  fprintf(stderr, "Loading ... ");
  srand(time(NULL));


  if (GZ){
    while (!gzeof (fp)){
      gzgets(fp, line, 1000);
      if (gzeof (fp)) break;
      line[strlen(line)-3] = 0;
      strcpy(name, line); // name/1
      
      gzgets(fp, line, 1000);
      line[strlen(line)-1] = 0;
      strcpy(sequence, line); // sequence1
      
      gzgets(fp, line, 1000); // + line
      
      
      gzgets(fp, line, 1000);
      line[strlen(line)-1] = 0;
      strcpy(fqual, line); // qual1
      
      gzgets(fp, line, 1000); // name/2
      
      
      gzgets(fp, line, 1000);
      line[strlen(line)-1] = 0;
      strcpy(revseq, line); // sequence2
      
      gzgets(fp, line, 1000);// + line
      
      gzgets(fp, line, 1000);
      line[strlen(line)-1] = 0;
      strcpy(rqual, line); // qual2
      
      if (crop != 0){
	sequence[crop]=0;	
	revseq[crop]=0;
	fqual[crop]=0;
	rqual[crop]=0;
      }
      
      seqlen = strlen(sequence);
      
      seqs[i] = (struct spair * ) malloc (sizeof (struct spair));
      seqs[i]->name = (char *) malloc (sizeof (char) * (strlen (name)+1));
      seqs[i]->seq = (char *) malloc (sizeof (char) * (2*seqlen+1));
      seqs[i]->qual = (char *) malloc (sizeof (char) * (2*seqlen+1));
      seqs[i]->len = seqlen;

      strcpy(seqs[i]->name, name);
      
      sprintf(seqs[i]->seq, "%s%s", sequence, revseq);
      sprintf(seqs[i]->qual, "%s%s", fqual, rqual);
      i++;
    }
    
    gzclose(fp);
  }
  
  else{
    while (!feof (fp)){
      fgets(line, 1000, fp);
      if (feof (fp)) break;
      line[strlen(line)-3] = 0;
      strcpy(name, line); // name/1
      
      fgets(line, 1000, fp);
      line[strlen(line)-1] = 0;
      strcpy(sequence, line); // sequence1
      
      fgets(line, 1000, fp); // + line
      
      
      fgets(line, 1000, fp);
      line[strlen(line)-1] = 0;
      strcpy(fqual, line); // qual1
      
      fgets(line, 1000, fp); // name/2
      
      
      fgets(line, 1000, fp);
      line[strlen(line)-1] = 0;
      strcpy(revseq, line); // sequence2
      
      fgets(line, 1000, fp);// + line

      fgets(line, 1000, fp);
      line[strlen(line)-1] = 0;
      strcpy(rqual, line); // qual2

      if (crop != 0){
	sequence[crop]=0;	
	revseq[crop]=0;
	fqual[crop]=0;
	rqual[crop]=0;
      }
      
      
      
      seqlen = strlen(sequence);
      
      seqs[i] = (struct spair * ) malloc (sizeof (struct spair));
      seqs[i]->name = (char *) malloc (sizeof (char) * (strlen (name)+1));
      seqs[i]->seq = (char *) malloc (sizeof (char) * (2*seqlen+1));
      seqs[i]->qual = (char *) malloc (sizeof (char) * (2*seqlen+1));
      seqs[i]->len = seqlen;

      strcpy(seqs[i]->name, name);      

      sprintf(seqs[i]->seq, "%s%s", sequence, revseq);
      sprintf(seqs[i]->qual, "%s%s", fqual, rqual);
      i++;
    }
    
    fclose(fp);
    
  }


  nseq = i;

  fprintf(stderr, "  done. %d pairs loaded.\n", nseq);

  /*  fprintf(stderr, "Sorting ..");
  qsort(seqs, nseq, sizeof(struct spair *), maxfunc);
  */

  tilechunk = nseq / 119;

  fprintf(stderr, " Dumping ..");
  prevseq[0]=0;

  for (i=0;i<nseq;i++){

    seqlen = seqs[i]->len;

    for (j=0;j<seqlen;j++)
      sequence[j]=seqs[i]->seq[j];
    sequence[j]=0;
    
    for (j=seqlen;j<2*seqlen;j++)
      revseq[j-seqlen]=seqs[i]->seq[j];
    revseq[seqlen]=0;
    
    for (j=0;j<seqlen;j++)
      fqual[j]=seqs[i]->qual[j];
    fqual[j]=0;
    
    for (j=seqlen;j<2*seqlen;j++)
      rqual[j-seqlen]=seqs[i]->qual[j];
    rqual[seqlen]=0;
    
    
    if  (tilecheck >= tilechunk){
      tilecnt++;
      fprintf(stderr, "nseq: %d\ttilechunk: %d\ttilecheck: %d\ttilecnt is now %d\n", nseq, tilechunk, tilecheck, tilecnt);
      tilecheck=0;
    }

    tilecheck++;

    if (strcmp(seqs[i]->name, prevseq)){
      offset += 5;
    }
    else
      offset = 0;
    

    x = abs((rand()*rand())) % 4097;
    y = abs((rand()*rand())) % 4097;

    /*
    if (GZ)
      gzprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name,  sequence, fqual, seqs[i]->name,  revseq, rqual);
    else
      fprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name,  sequence, fqual, seqs[i]->name, revseq, rqual);
    */

    if (rc) revcomp(revseq, rqual);

    if (GZ){
      gzprintf(clean, "%s/1\n%s\n+\n%s\n", seqs[i]->name,  sequence, fqual);
      gzprintf(dup, "%s/2\n%s\n+\n%s\n", seqs[i]->name, revseq, rqual);
    }
    else{
      fprintf(clean, "%s/1\n%s\n+\n%s\n", seqs[i]->name,  sequence, fqual);
      fprintf(dup, "%s/2\n%s\n+\n%s\n", seqs[i]->name, revseq, rqual);
    }
    //fprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name,  sequence, fqual, seqs[i]->name, revseq, rqual);
  
    strcpy(prevseq, seqs[i]->name);


  }
  fprintf(stderr, "  done.\n");
  if (GZ){
  gzclose(dup); gzclose(clean);
  }
  else{
  fclose(dup); fclose(clean);
  }
  //  nseq = readSingleFasta(fp);

}


static int maxfunc(const void *p1, const void *p2){
  struct spair *a, *b;

  int ret;
  a = *((struct spair **)p1);
  b = *((struct spair **)p2);


  ret = memcmp(a->name, b->name, strlen(a->name));

  return ret;


}



void revcomp(char *seq, char *qual) {
  int len;
  char *seq2,  *qual2;
  int i;
  
  len = strlen(seq);
  
  seq2 = malloc(sizeof(char *) * (len+1));
  qual2 = malloc(sizeof(char *) * (len+1));
  
  for (i=0; i<len; i++){

    qual2[i] = qual[len-1-i];
    
    switch(seq[len-1-i]){
    case 'A':
      seq2[i]='T'; break;
    case 'C':
      seq2[i]='G'; break;
    case 'G':
      seq2[i]='C'; break;
    case 'T':
      seq2[i]='A'; break;
    default:
      seq2[i]='N'; break;
    }

  }
  
  qual2[len]=0; seq2[len]=0;
  
  strcpy(seq, seq2);
  strcpy(qual, qual2);
  free(seq2); free(qual2);

}

