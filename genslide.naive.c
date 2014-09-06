#include <stdio.h>
#include "fastatools.h"

/* slides windows in a chromosome */

//char *iswritten;

int mystrcmp(char *s1, char *s2);

int main(int argc, char **argv){
  int i,j;
  int ws;
  int ss;
  char fname[100];
  FILE *fp;
  int nseq;
  int slen;
  char *window;
  char *window2;
  int upper, lower;
  int iswritten;
  int ret;
  int n;

  lower = 0; upper = 0;
  ws=0; ss=0;
  fname[0]=0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-ws"))
      ws = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-ss"))
      ss = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-u"))
      upper = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      lower = atoi(argv[i+1]);
  }
  
  fp = fopen(fname, "r");
  if (fp==NULL || ws == 0 || ss == 0 || upper == 0 || lower == 0)
    return 0;
  
  nseq = readSingleFasta(fp);
  if (nseq!=1)
    return 0;


  window = (char *) malloc(sizeof(char) * (ws+1));
  window2 = (char *) malloc(sizeof(char) * (ws+1));
  slen = strlen(seqs[0]);
  /*
  iswritten = (char *) malloc(sizeof(char) * slen);
  memset(iswritten, '\0', slen*sizeof(char));
  */

  for (i=0;i<slen-ws;i+=ss){
    memcpy(window, seqs[0]+i, ws*sizeof(char));
    window[ws] = 0;

    n=0;

    for (j=0;j<ws;j++){
      if (window[j]=='N')
	n++;
      if (n>10)
	break;

    }
    
    if (n<=10){


      iswritten = 0;
      fprintf(stderr, "\r%d", (i+1));
      j=i+ws+lower;
      
      while ( j<i+ws+upper && j<slen-ws){
	//    for(j=i+ws+lower;j<i+ws+upper,j<slen-ws;j+=ss){
	memcpy(window2, seqs[0]+j, ws*sizeof(char));      
	window2[ws]=0;
	ret = mystrcmp(window, window2);
	//fprintf(stderr, "%d:%s\t%d:%s\t%d\n", i, window, j, window2, ret);
	if (!ret){
	  if (!iswritten){
	    printf("%s\t%d\t", window, (i+1));
	    iswritten = 1;
	  }
	  printf("%d\t", (j+1));
	  j+=ss;
	}
	else
	  j+=ret;
      }
      if (iswritten)
	printf("\n");
    }
  }
  
  return 1;
}


int mystrcmp(char *s1, char *s2){
  int cnt=0; 
  while (*s1!=0 && *s2!=0L){
    if (*s1 != *s2)
      return (cnt+1);
    s1++; s2++; cnt++;
  }
  return 0;
}
