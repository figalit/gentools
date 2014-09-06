#include <stdio.h>
#include <math.h>

#include "fastatools.h"

/* slides windows in a chromosome */

//char *iswritten;

int mystrcmp(char *s1, char *s2);
char encode_window(char *window);
int cindex(char ch);
static int maxfunc(const void *, const void *);

char *genome;
int *windows;

int ws;

int main(int argc, char **argv){
  int i,j;

  int ss;
  char fname[100];
  FILE *fp;
  int nseq;
  int slen;
  char window[301];
  char window2[301];
  int upper, lower;
  int iswritten;
  int ret;
  int n;
  char wind[5];
  int last;
  
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
  
  slen = strlen(seqs[0]);
  
  
  
  //genome = (char *) malloc(sizeof(char)*(slen+1));
  
  last=0;

  /*

  for (i=0;i<slen-4;i+=4){
    memcpy(wind, seqs[0]+i, 4);
    wind[4]=0;
    genome[i/4] = encode_window(wind);
    last = i;
  }
  
  if (floor(slen/4)!=ceil(slen/4)){
    memcpy(wind, seqs[0]+i, (slen-i));
    genome[i/4] = encode_window(wind);
  }
  */
      
  windows =   (int *) malloc(sizeof(int)*(slen-ws));

  for (i=0;i<slen-ws+1;i++)
    windows[i]=i;

  window2[0] = 0; last = -1; iswritten=0;
  fprintf(stderr, "Sorting ...");
  qsort(windows, (slen-ws), sizeof(int), maxfunc);
  fprintf(stderr, "\nDumping\n");


  /*
  for (i=0;i<slen-ws;i++){
    memcpy(window, seqs[0]+windows[i], ws);
    window[ws]=0;
    if (!strchr(window, 'N')){
      if (!strcmp(window, window2)){
	if (!iswritten){
	  //printf("----------\n%d\t%s\n", windows[last], window2);
	  printf("\n----------\n%s\t%d\t", window2, windows[last]);
	  iswritten = 1;
	}
	//printf("%d\t%s\n", windows[i], window);
	printf("%d\t", windows[i]);
      }
      else{
	iswritten = 0; last = i;
	strcpy(window2, window);
      }
    }
    }*/

  for (i=0;i<slen-ws+1;i++){
    memcpy(window, seqs[0]+windows[i], ws);
    window[ws]=0;
    if (strchr(window, 'N'))
      continue;
    j=i+1;
    if (j<slen-ws+1 && windows[j]-windows[i]>=lower && windows[j]-windows[i]<=upper){
      iswritten=0;
      memcpy(window2, seqs[0]+windows[j], ws);
      window2[ws]=0;
      while (j<slen-ws+1 && windows[j]-windows[i]>=lower && windows[j]-windows[i]<=upper && !strcmp(window, window2)){
	if (!iswritten){
	  printf("\n----------\n%s\t%d\t", window, windows[i]);      
	  iswritten = 1;
	}
	printf("%d\t", windows[j]);
	j++;
	memcpy(window2, seqs[0]+windows[j], ws);
	window2[ws]=0;
      }
    }
  }


  return 1;
}


char encode_window(char *window){
  int len = strlen(window);
  unsigned char code;
  unsigned char this;
  int shiftleft;
  int i;
  unsigned int left; //, right;                                                                                                                                                                                                              
  
  code = 0;

  for (i=0;i<len;i++){
    this = cindex(window[i]);
    shiftleft = 2*(len-i-1);
    left = this << shiftleft;
    code = code | left;
  }
  return code;
}


int cindex(char ch){
  switch(toupper(ch)){
  case 'A':
    return 0;
    break;
  case 'C':
    return 1;
    break;
  case 'G':
    return 2;
    break;
  case 'T':
    return 3;
    break;
  }
  return 3;
}


static int maxfunc(const void *p1, const void *p2){
  int a, b;
  int ret;
  a = (int)(*((int *)p1));
  b = (int)(*((int *)p2));

  ret = memcmp(seqs[0]+a, seqs[0]+b, ws);
  if (ret==0)
    return a-b;
  
  
  return ret;

  
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
