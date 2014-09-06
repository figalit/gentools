#include "fastatools.h"

int main(int argc, char **argv){
  int nseq;
  FILE *fp;
  FILE *out;
  int len;
  char fname[SEQ_LENGTH];
  int i, j, k, l, m, cnt;
  FILE *log;
  int open;

  if (argc != 2)
    return 0;

  fp = fopen(argv[1], "r");

  log = fopen("index.log", "a"); 
  open = 0;

  nseq = readSingleFasta(fp);
  cnt = 1;
  for (i=0;i<nseq;i++){
    j = 0; k = 1;
    len = strlen(seqs[i]);
    sprintf(fname, "%s_%d", names[i], cnt);
    out = fopen(fname, "w");
    open = 1;
    fprintf(log, "%s\t%d\t", fname, (j+1));
    fprintf(out, ">%s_%d\n", names[i], cnt++);
    fflush(out);
    while (j<len){
      if (seqs[i][j] != 'N'){
	fprintf(out, "%c", seqs[i][j]);
	k++; j++;
	if (k % 60 == 0)
	  fprintf(out, "\n");
      }
      else{
	l = j;
	while (l<len && seqs[i][l] == 'N')
	  l++;
	if (l-j>=100){
	  fprintf(log, "%d\n", j);
	  fprintf(out, "\n");
	  fclose(out);
	  sprintf(fname, "%s_%d", names[i], cnt);
	  out = fopen(fname, "w");
	  fprintf(out, ">%s_%d\n", names[i], cnt++);	  
	  fprintf(log, "%s\t%d\t", fname, (l+1));
	}
	else{
	  for (m=j;m<l;m++)
	    fprintf(out, "%c", seqs[i][m]);
	}	
	j = l;
      }
      //      fprintf(out, "\n");
      // fclose(out);
    }
  }
  
  if (open){
    fprintf(log, "%d\n", (j+1));
    fclose(out);
  }
  
}
