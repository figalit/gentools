#include <stdio.h>
#include <ctype.h>

#define MAX 250000000
#define SIZE 15000000
#define STDEV 2000000


char seq[MAX];

int main(int argc, char **argv){
  FILE *in;
  char *seqname;

  char newseqname[35];
  char ch;
  int len;
  int part;
  int i,j;
  char logname[40];
  FILE *out, *log;
  char fname[100];
  int offset;
  char origname[100];
  
  seqname = (char *) malloc(sizeof(char)*30);

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      offset = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      strcpy(origname, argv[i+1]);
  
  }
  in = fopen(fname, "r");
  len = 0;

  fscanf(in, "%s\n", seqname);
  if (seqname[0] == '>')
    seqname++;

  while(fscanf(in, "%c", &ch) > 0){
    if (!isspace(ch))
      seq[len++] = ch;
  }
  
  fclose(in);

  if (len <= SIZE+STDEV)
    return 1;

  sprintf(logname, "%s.log", seqname);
  log = fopen(logname, "w");
  
  part = 1;

  sprintf(newseqname, "%s_%d", seqname, part);
  out = fopen(newseqname, "w");
  
  fprintf(log, "%s\t%s\t%d\n", newseqname, origname, 0+offset);
  printf("partitioning %d\n", part);
  fprintf(out, ">%s\n", newseqname);

  for (i=0;i<len;i++){
    if (i / SIZE >= part){
      if (len - i <= STDEV){
	printf("dumping the rest\n");
	for (j=i;j<len;j++){
	  fprintf(out, "%c", seq[j]);
	  if (j%60==0)
	    fprintf(out, "\n");
	}
	fclose(out); return 1;
      }
      fclose(out);
      part = i / SIZE + 1;
      printf("partitioning %d\n", part);
      sprintf(newseqname, "%s_%d", seqname, part);
      out = fopen(newseqname, "w");  
      fprintf(out, ">%s\n", newseqname);    
      fprintf(log, "%s\t%s\t%d\n", newseqname, origname, i+offset);
    }
    fprintf(out, "%c", seq[i]);
    if (i%60==0)
      fprintf(out, "\n");    
  }


  return 1;
}
