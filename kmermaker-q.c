#include <stdio.h>
#include <stdlib.h>

#define MAXKMER 100
#define MAXLEN  10000

void  breakthis(char *name, char *seq, char *, int klen);
int slide=1;
char seq[MAXLEN];
char qual[MAXLEN];

int pass_first = 0;

int main(int argc, char **argv){
  FILE *fasta;
  char kmer[MAXKMER];
  int klen=0;
  int i,j;
  char name[MAXKMER];
 
  char fname[100];
  char line[100];
  char ch;
  

  fname[0]=0;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-k"))
      klen = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      slide = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-pass"))
      pass_first = 1;
  }

  if (!strcmp(fname, "stdin"))
    fasta  = stdin;
  else
    fasta = fopen(fname, "r");
  
  j=0; i=0; seq[0]=0; name[0]=0; qual[0]=0;
  while (fscanf(fasta, "@%s\n%s\n", name, seq) > 0){
    fgets(line, 100, fasta);
    fscanf(fasta, "%s\n", qual);
    
    /*
      if (ch == '>'){
      if (seq[0]!=0){
      seq[j]=0;
      //fprintf(stderr, "breaking %s\n", name);
      breakthis(name, seq, klen);
      }
    */
    
    breakthis(name, seq, qual, klen);
    /*
    fscanf(fasta, "%s", name);
    i=0; j=0; seq[0]=0;
    fgets(line, 100, fasta);*/
    //fprintf(stderr, "read %s\n", name);
  }
    
  /*
    else if (!isspace(ch))
      seq[j++] = ch;

      }*/
  
  /*  
  if (seq[0]!=0)
  breakthis(name, seq, klen);*/
  
}

void  breakthis(char *name, char *seq, char *qual, int klen){
  int i;
  int len = strlen(seq);
  char kmer[MAXKMER];
  char qmer[MAXKMER];
  char nmer[MAXKMER];
  for (i=0;i<klen/3;i++)
    nmer[i]='N';
  nmer[klen/3]=0;
  //fprintf(stderr, "breaking %s\n", name);
  for (i=0; i<len-klen;i+=slide){
    memcpy(kmer, seq+i, klen);
    memcpy(qmer, qual+i, klen);
    kmer[klen]=0; qmer[klen]=0;

    if (i==0 && pass_first) continue;

    if (!strstr(kmer, nmer))
      printf("@%s\n%s\n+\n%s\n", name, kmer, qmer);
    //printf("@%s.%d\n%s\n+\n%s\n", name, i, kmer, qmer);
  }

}
