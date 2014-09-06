#include <stdio.h>

#define MAXLEN 250000000

char seq[MAXLEN];

int main(int argc, char **argv){
  char fname[1000];
  int i;
  
  char seqname[100];
  int start;
  int end;
  int depth;

  FILE *in;
  char ch;
  int len;
  int cnt;
  int offset=0;
  int ss, ee;
  int atcnt;
  
  fname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);    
    else if (!strcmp(argv[i], "-o"))
      offset = atoi(argv[i+1]);
  }
  
  if (fname[0]==0)
    return 0;


  in = fopen(fname, "r");
  if (in==NULL)
    return 0;


  fprintf(stderr, "Reading %s\n", fname);
  i=0;
  while(fscanf(in, "%c", &ch)  > 0){
    if (ch=='>') fgets(seqname, 100, in);
    else if (!isspace(ch))
      seq[i++]=ch;
  }
  seq[i]=0;
  len=i;
  
  fprintf(stderr, "%s read with %d chars\n", fname, len);

  //  while(scanf("%s%d%d%d", seqname, &start, &end, &depth) > 0){
  while(scanf("%s%d%d", seqname, &start, &end) > 0){
    cnt=0; atcnt=0;
    ss=start-offset;
    ee=end-offset;
    for (i=ss;i<=ee;i++){
      if (seq[i]=='N') cnt++;
      if (seq[i]=='A' || seq[i]=='T' || seq[i]=='G' || seq[i]=='C') atcnt++;
    }
    //printf("%s\t%d\t%d\t%d\t%f\n", seqname, start, end, depth, (float)cnt/(float)(atcnt+cnt));
    printf("%s\t%d\t%d\t%lf\n", seqname, start, end, (double)cnt/(double)(atcnt+cnt));
  }
}
