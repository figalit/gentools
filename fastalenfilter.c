#include <stdio.h>
#define MAX 10000000

char seq[MAX];
int main(int argc, char **argv){
  int len=0;
  int len2=-1;
  char fname[1000];
  FILE *fp;
  int i,j;
  char something[100000];
  char name[100000];
  int seqlen;
  int seqlen2;
  int non;

  char ch;

  //  char *seq;

  fname[0]=0;
  non = 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-l2"))
      len2 = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      non = 1;
  }

  fp = fopen(fname, "r");
  fprintf(stderr, "%s\t%d\n", fname, len);

  name[0] = 0; i=0;
  while (fscanf(fp, "%c", &ch) > 0){
    if (ch=='>'){
      fgets(name, 100000, fp);
      if (name[0] != 0){
	seqlen = i;
	seq[i] = 0;
	if (!non)
	  seqlen2 = seqlen; 
	else{
	  seqlen2 = 0;
	  for (j=0; j<seqlen-1; j++)
	    if (toupper(seq[j])!='N') seqlen2++;
	}

	if (non && len2!=-1 && seqlen<len2)
	  ;
	else if (seqlen2>=len)
	  fprintf(stdout, "\n>%s%s", name, seq);	
      }
      i=0;
    }
    else if ( !isspace(ch))
      seq[i++]=ch;
    
  }
  
  
  seqlen = i;
  seq[i] = 0;
  if (!non)
    seqlen2 = seqlen; 
  else{
    seqlen2 = 0;
    for (j=0; j<seqlen-1; j++)
      if (toupper(seq[j])!='N') seqlen2++;
  }
  
  if (non && len2!=-1 && seqlen<len2)
    ;
  else if (seqlen2>=len)
    fprintf(stdout, "\n>%s%s", name,  seq);	
	
  /*

  while(fscanf(fp, "%s %d", name, &seqlen) >0 ){
    fgets(something, 100000, fp);
    //fprintf(stderr, "%d\n", seqlen);
    //    seq = (char *) malloc(sizeof(char) * (seqlen+2));
    fgets (seq, MAX, fp);
    seqlen = strlen(seq)-1;
    if (!non)
      seqlen2 = seqlen; 
    else{
      seqlen2 = 0;
      for (j=0; j<seqlen-1; j++)
	if (toupper(seq[j])!='N') seqlen2++;
    }
    
    if (seqlen2>=len)
      fprintf(stdout, "%s %d\n%s", name, seqlen, seq);
    //free(seq);
  }
  */
}
