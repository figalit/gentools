#include <stdio.h>
#include <ctype.h>

#define MAX 250000000

char seq[MAX];
int match(int, char *, char *);
int match_r(int, char *, char *);

int main(int argc, char **argv){
  FILE *in;
  char fname[100];
  char *kmer;
  int i,j;
  char seqname[50];
  char ch;
  char dummy[100];
  int seqlen;
  int klen;

  fname[0] = 0;
  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-s")){
      kmer = (char *) malloc(sizeof(char) * strlen(argv[i+1]));
      strcpy(kmer, argv[i+1]);
      for (j=0;j<strlen(kmer);j++)
	kmer[j] = toupper(kmer[j]);
    }
  }

  if (fname[0] == 0 || kmer[0] ==0)
    return 0;
  
  in = fopen(fname, "r");

  fscanf(in, "%c", &ch);
  fscanf(in, "%s", seqname);
  fgets(dummy, 100, in);
  
  i=0;
  
  while(fscanf(in, "%c", &ch) > 0){
    if (!isspace(ch))
      seq[i++]=toupper(ch);
  }

  seq[i]=0;
  seqlen=i;
  klen = strlen(kmer);
  

  for(i=0;i<seqlen-klen+1;i++){
    if (match(i, seq, kmer))
      printf("%s\t%d\t%d\tF\n", seqname, (i+1), (i+1+klen));
    else if (match_r(i, seq, kmer))
      printf("%s\t%d\t%d\tR\n",seqname, (i+1), (i+1+klen));
	   
  }

  return 1;
}

int match(int s, char *seq, char *kmer){
  int klen = strlen(kmer);
  int i;
  int j;
  
  i=s;
  for (j=0;j<klen;j++){
    if (kmer[j]=='X' || kmer[j]=='N')
      i++;
    else if (kmer[j]!=seq[i])
      return 0;
    else
      i++;
  }

  return 1;
}

int match_r(int s, char *seq, char *kmer){
  int klen = strlen(kmer);
  int i;
  int j;
  
  if (s<klen)
    return 0;

  i=s-1;
  for (j=0;j<klen;j++){
    if (kmer[j]=='X' || kmer[j]=='N')
      i--;
    else{
      switch (kmer[j]){
      case 'A':
	if (seq[i--]!='T')
	  return 0;
	break;
      case 'C':
	if (seq[i--]!='G')
	  return 0;
	break;
      case 'G':
	if (seq[i--]!='C')
	  return 0;
	break;
      case 'T':
	if (seq[i--]!='A')
	  return 0;
	break;
      }
    }
  }

  return 1;
}
