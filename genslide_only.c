#include <stdio.h>
#include "fastatools.h"

/* slides windows in a chromosome */

int main(int argc, char **argv){
  int i;
  int ws;
  int ss;
  char fname[100];
  FILE *fp;
  int nseq;
  int slen;
  char *window;

  ws=0; ss=0;
  fname[0]=0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-ws"))
      ws = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-ss"))
      ss = atoi(argv[i+1]);
  }
  
  fp = fopen(fname, "r");
  if (fp==NULL || ws == 0 || ss == 0)
    return 0;
  
  nseq = readSingleFasta(fp);
  if (nseq!=0)
    return 0;

  window = (char *) malloc(sizeof(char) * (ws+1));
  slen = strlen(seqs[0]);

  for (i=0;i<slen-ws;i+=ss){
    memcpy(window, seqs[0], ws*sizeof(char));
    window[ws] = 0;
    printf("%s\t%s\t%d\n", window, names[0], i);
  }
  
  return 1;
}
