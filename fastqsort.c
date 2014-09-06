#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <zlib.h>
#include <string.h>
#include <time.h>

#define MAXSEQ 275000000
//#define MAXSEQ 30000000
#define MAXLEN 100

int GZ=0;

typedef struct spair{
  char *name;
  char *seq;
  char *qual;
  unsigned char len;
}_spair;

struct spair **seqs;

static int maxfunc(const void *p1, const void *p2);

int count_colon(char *name);

int main(int argc, char **argv){
  FILE *fp;
  int i,j;
  char fname[100];
  int nseq;
  char sequence[100]; char name[100]; char revseq[100];
  int uniq=0;
  char fqual[100]; char rqual[100];
  int realuniq=0;
  char prevseq[200];
  int seqlen;
  char line[1000];
  FILE *clean;
  FILE *dup;
  char fname2[100];
  int crop=0;
  char *ch;
  int x; int y, tilecnt, lane;
  char *flow;
  int duped;
  int iter;
  
  
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
    sprintf(fname2, "%s.clean.gz", fname);
    clean = gzopen(fname2, "w");
    sprintf(fname2, "%s.dup.gz", fname);
    dup = gzopen(fname2, "w");
  }
  else{
    sprintf(fname2, "%s.clean.fq", fname);
    clean = fopen(fname2, "w");
    sprintf(fname2, "%s.dup.fq", fname);
    dup = fopen(fname2, "w");
  }
  
  i=0;
  
  
  srand(time(NULL));
  
  seqs = (struct spair ** ) malloc (sizeof (struct spair *) * MAXSEQ);
  nseq=0;
  
  fprintf(stderr, "Loading ... ");
  
  
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
  
  fprintf(stderr, "Sorting ..");
  iter=1;
  
  do{
    
    printf("iteration %d\n", iter);
    qsort(seqs, nseq, sizeof(struct spair *), maxfunc);
    
    prevseq[0]=0; duped = 0;
    
    for (i=0;i<nseq;i++){
      
      
      flow = strtok(seqs[i]->name, ":");
      lane = atoi(strtok(NULL, ":"));
      
      if (count_colon(seqs[i]->name) == 4)
	tilecnt = atoi(strtok(NULL, ":"));
      
      x = atoi(strtok(NULL, ":"));
      y = atoi(strtok(NULL, ":"));
      
      /*
	printf("y: %s\n", y);
	getchar();
      */
      
      /*
	x += abs(rand() % 107) + 1;
	if (x>4096) x = 0;
	y += abs(rand() % 43) + 1;
	if (y>4096) y = 0;
      */
      
      tilecnt = abs(rand() % 119) + 1;
      
      //duped = 1;
      
      sprintf(seqs[i]->name, "%s:%d:%d:%d:%d", flow, lane, tilecnt, x,y);
      //sprintf(seqs[i]->name, "@IPAR1-1:8:%d:%d:%d", tilecnt, x,y);
      
      //}
      
      if (!strcmp(prevseq, seqs[i]->name))
	duped - 1;
      
      strcpy(prevseq, seqs[i]->name);
    }
    
    iter++;
  } while (duped);
  
  fprintf(stderr, "  done.\nDumping ..");
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
    

    if (GZ)
      gzprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name, sequence, fqual, seqs[i]->name, revseq, rqual);
    else
      fprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name, sequence, fqual, seqs[i]->name, revseq, rqual);
    
    /*
    if (strcmp(prevseq, seqs[i]->name)){
      if (GZ)
	gzprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name, sequence, fqual, seqs[i]->name, revseq, rqual);
      else
	fprintf(clean, "%s/1\n%s\n+\n%s\n%s/2\n%s\n+\n%s\n", seqs[i]->name, sequence, fqual, seqs[i]->name, revseq, rqual);
    }
    else{
      
      flow = strtok(seqs[i]->name, ":");
      lane = atoi(strtok(NULL, ":"));
      tilecnt = atoi(strtok(NULL, ":"));
      x = atoi(strtok(NULL, ":"));
      y = atoi(strtok(NULL, ":"));
      
      /*
      printf("y: %s\n", y);
      getchar();
      
      
      x += abs(rand() % 107) + 1;
      if (x>4096) x = 0;
      y += abs(rand() % 43) + 1;
      if (y>4096) y = 0;
      


      if (GZ)
	gzprintf(clean, "@IPAR1-1:8:%d:%d:%d/1\n%s\n+\n%s\n@IPAR1-1:8:%d:%d:%d/2\n%s\n+\n%s\n", tilecnt, x,y,  sequence, fqual, tilecnt, x,y,  revseq, rqual);
      else
	fprintf(clean, "@IPAR1-1:8:%d:%d:%d/1\n%s\n+\n%s\n@IPAR1-1:8:%d:%d:%d/2\n%s\n+\n%s\n", tilecnt, x,y,  sequence, fqual, tilecnt, x,y,  revseq, rqual);

    

    }
    */



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


int count_colon(char *name){
  int cnt = 0; int i;
  for (i=0;i<strlen(name);i++) if (name[i]==':') cnt++;

  return cnt;

}
