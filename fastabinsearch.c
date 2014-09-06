#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



char **seqs;
char **names;

int binSearch(char *seq, int s, int e);
void revcomp(char *s, char *r);

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

  fname[0]=0; nseq = 0; sname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      strcpy(sname, argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      nseq = atoi(argv[i+1]);
  }
  if (fname[0]==0 || nseq==0 || sname[0]==0) return 0;

  fp = fopen(fname, "r");

  i=0;

  seqs = (char ** ) malloc (sizeof (char *) * nseq);
  names = (char ** ) malloc (sizeof (char *) * nseq);

  fprintf(stderr, "Loading  %d sequences ..", nseq);
  while (fscanf(fp, ">%s\n%s\n", name, sequence) > 0 ){
    seqs[i] = (char * ) malloc (sizeof (char) * (strlen(sequence)+1));
    names[i] = (char * ) malloc (sizeof (char) * (strlen(name)+1));
    strcpy(seqs[i], sequence);
    strcpy(names[i], name);
    i++;
  }
  fprintf(stderr, "  done.\nSearching ..");
  
  //printf("%s\n", seqs[2110]);

  fclose(fp);

  fp = fopen(sname, "r");

  while (fscanf(fp, ">%s\n%s\n", name, sequence) > 0 ){
    if (strlen(sequence) > strlen(seqs[0])){
      sequence[strlen(seqs[0])] = 0;
    }
    else if (strlen(sequence) < strlen(seqs[0]))
      continue;
    revcomp(sequence, revseq);
    s = 0, e = nseq; retCode = 10000; olds=-10;
    found=0;
    while (s<e && retCode!=0){
      retCode=binSearch(sequence, s, e);
      if (retCode < 0){e = (s+e)/2;}
      else if (retCode > 0){olds=s; s = (s+e)/2;}
     
      else if (retCode == 0){
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

    s = 0, e = nseq; retCode = 10000; olds=-10;

    while (s<e && retCode!=0){
      retCode=binSearch(revseq, s, e);
      if (retCode < 0){e = (s+e)/2;}
      else if (retCode > 0){olds=s; s = (s+e)/2;}
      
      else if (retCode == 0){
	//fprintf (stderr,  "found\n"); found=1;
	fprintf (stdout,  "rev_found %s at %s\n", name, names[(s+e)/2]);
	break;
      }
      
      if (s>=e || s==olds){
	//fprintf (stderr,  "missed\n");
	break;
	//	fprintf (stdout,  "missed %s\n", name); break;
      }
         
    }
    //if (!found) fprintf (stdout,  "missed %s\n", name);
  }
  
  fprintf(stderr, "  done.\n");

}

int binSearch(char *seq, int s, int e){
  int med = (s+e)/2;
  
  int ret;
  
  ret = memcmp(seq, seqs[med], strlen(seq));
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
