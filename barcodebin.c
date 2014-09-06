#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct spair{
  char *name;
  char *seq;
  int count;
}_spair;


//char **seqs;
//char **names;
//char *count;

int binSearch(char *seq, int s, int e);
void revcomp(char *s, char *r);


static int maxfunc(const void *p1, const void *p2);

struct spair **seqs;
int seqlen;

int main(int argc, char **argv){
  FILE *fp;
  int i;
  char fname[100];
  int nseq;
  char sequence[100]; char name[100];
  char sname[100];
  char revseq[100];
  int retCode=100000;
  int s, e; int olds;
  int found;
  int crop;
  int len;
  int longest_barcode=0;
  int fastq=0;
  char line[1000];
  int loop_done=0;
  int crop_lim;
  int min_barcode = 17;
  
  fname[0]=0; nseq = 0; sname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      strcpy(sname, argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      nseq = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-m"))
      min_barcode = atoi(argv[i+1]);
  }

  if (fname[0]==0 || nseq==0 || sname[0]==0) return 0;

  fp = fopen(fname, "r");

  i=0;

  /*
  seqs = (char ** ) malloc (sizeof (char *) * nseq);
  names = (char ** ) malloc (sizeof (char *) * nseq);
  count = (int ) malloc (sizeof (int) * nseq);
  */

  seqs = (struct spair ** ) malloc (sizeof (struct spair *) * nseq);

  fprintf(stderr, "Loading  %d sequences ..", nseq);
  while (fscanf(fp, ">%s\n%s\n", name, sequence) > 0 ){
    seqlen=strlen(sequence);
    if (seqlen > longest_barcode) longest_barcode = seqlen;
    if (seqlen>=15){
      seqs[i] = (struct spair * ) malloc (sizeof (struct spair));
      seqs[i]->name = (char *) malloc (sizeof (char) * (strlen (name)+1));
      seqs[i]->seq = (char *) malloc (sizeof (char) * (2*seqlen+1));
      //seqs[i]->qual = (char *) malloc (sizeof (char) * (2*seqlen+1));
      seqs[i]->count = 0;

      //seqs[i] = (char * ) malloc (sizeof (char) * (strlen(sequence)+1));
      //names[i] = (char * ) malloc (sizeof (char) * (strlen(name)+1));
      strcpy(seqs[i]->seq, sequence);
      strcpy(seqs[i]->name, name);
      //count[i]=0;
      i++;
    }
  }

  nseq = i;

  //  printf("before: %s\t%s\n", seqs[0]->name, seqs[0]->seq);

  qsort(seqs, nseq, sizeof(struct spair *), maxfunc);

  //  printf("after : %s\t%s\n", seqs[0]->name, seqs[0]->seq);


  fprintf(stderr, "  done. %d left. Longest barcode: %d bp\n", nseq, longest_barcode);
  
  //printf("%s\n", seqs[2110]);

  fclose(fp);

  fp = fopen(sname, "r");

  /*
  fgets(line, 1000, fp);

  if (line[0] == '@') { fastq=1;  fprintf(stderr, "FASTQ mode.\n"); }
  else { fastq=0;  fprintf(stderr, "FASTA mode.\nSearching .."); }

  rewind(fp);
  */

  //  while (!loop_done){
  while (fscanf(fp, "%s\n%s\n", name, sequence) > 0){

    /*
    if (!fastq){
      if ( ! (fscanf(fp, ">%s\n%s\n", name, sequence) > 0) ){
	loop_done = 1;
	break;
      } 
    }
    else{
      if ( ! (fscanf(fp, "@%s\n%s\n%s", name, sequence) > 0) ){
	loop_done = 1;
	break;
      } 
      fgets(line, 1000, fp); // + line 
      fgets(line, 1000, fp); // quality line
    }

    */

    if (name[0]=='@'){
      fgets(line, 1000, fp); // + line 
      fgets(line, 1000, fp); // quality line
    }

    /*
    if (strlen(sequence) > strlen(seqs[0])){
      sequence[strlen(seqs[0])] = 0;
    }
    else if (strlen(sequence) < strlen(seqs[0]))
      continue;
    */

    sequence[longest_barcode]=0;


    //revcomp(sequence, revseq);

    strcpy(revseq, sequence);
    s = 0, e = nseq; retCode = 10000; olds=-10;
    found=0;

/*
    while (s<e && retCode!=0){
      retCode=binSearch(sequence, s, e);
      if (retCode < 0){e = (s+e)/2;}
      else if (retCode > 0){olds=s; s = (s+e)/2;}
     
      else if (refotCode == 0){
	//fprintf (stderr,  "found\n"); found=1;
	fprintf (stdout,  "found %s at %s\n", name, names[(s+e)/2]);
	break;
      }
      if (s>=e || s==olds){
	//fprintf (stderr,  "missed\n"); 
	break;
	//fprintf (stdout,  "missed %s\n", name); break;
      }
      

    }
*/

    //s = 0, e = nseq; retCode = 10000; olds=-10;

    crop = 0;
    len = strlen(revseq);

    crop_lim = longest_barcode - min_barcode + 1; // ad hoc

    while (crop<crop_lim){
      revseq[len-crop]=0;

      while (s<e && retCode!=0){
	retCode=binSearch(revseq, s, e);
	if (retCode < 0){e = (s+e)/2;}
	else if (retCode > 0){olds=s; s = (s+e)/2;}
	
	else if (retCode == 0){
	  //fprintf (stderr,  "found\n"); found=1;
	  //fprintf (stdout,  "found %s in %s\n", name, seqs[(s+e)/2]->name);
	  seqs[(s+e)/2]->count++;
	  crop = crop_lim+1;
	  break;
	}
	
	if (s>=e || s==olds){
	  //fprintf (stderr,  "missed\n");
	  crop++;
	  //fprintf(stderr, "crop %s at %d\n", name, crop);
	  s = 0, e = nseq; retCode = 10000; olds=-10;
	  break;
	  //	fprintf (stdout,  "missed %s\n", name); break;
	}
	
      }
    }

    //if (!found) fprintf (stdout,  "missed %s\n", name);
  }
  
  fprintf(stderr, "  done.\n");
  fprintf(stdout, "Barcode_Name\tBarcode_Length\tFrequency\n");
  for (i=0;i<nseq;i++)
    fprintf(stdout, "%s\t%d\t%d\n", seqs[i]->name, strlen(seqs[i]->seq), seqs[i]->count);
}

int binSearch(char *seq, int s, int e){
  int med = (s+e)/2;
  
  int ret;
  
  //ret = memcmp(seq, seqs[med], strlen(seq));
  ret = strcmp(seq, seqs[med]->seq);
  // strcmp = memcmp(seq, seqs[med]);
  //  printf("%s\t%s\t%d\n", seq, seqs[med], ret);

  //printf("%d\t%d\t%d\t%d\n", s, e, med, ret);

  
  return ret;
}

void revcomp(char *s, char *r){
  int len = strlen(s);
  int  i;

  for (i=0;i<len;i++){
    switch(s[i]){
    case 'A': r[len-i-1] = 'T'; break;
    case 'T': r[len-i-1] = 'A'; break;
    case 'C': r[len-i-1] = 'G'; break;
    case 'G': r[len-i-1] = 'C'; break;
    default:  r[len-i-1] = s[i]; break;
    }
  }
  r[len]=0;
}


static int maxfunc(const void *p1, const void *p2){
  struct spair *a, *b;

  int ret;
  a = *((struct spair **)p1);
  b = *((struct spair **)p2);


  //ret = memcmp(a->seq, b->seq, strlen(a->seq));
  ret = strcmp(a->seq, b->seq);

  return ret;


}




