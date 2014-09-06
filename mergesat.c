#include <stdio.h>

#define MAXLEN 250000000

char seq[MAXLEN];

int main(int argc, char **argv){
  char fname[1000];
  int i,j;
  
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
  int is, ie;
  int iss;
  int atcnt;
  char thischr[100];

  offset = 1;

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

  for (ss=1;ss<=22;ss++){
    sprintf(thischr, "chr%d", ss);
    fprintf(stderr, "%s\n", thischr);

    memset(seq, 0, sizeof(char)*MAXLEN);

    rewind(in);

    while (fscanf(in, "%s\t%d\t%d\n", seqname, &start, &end) > 0){
      if (strcmp(seqname, thischr)) continue;
      
      for (i=start; i<=end; i++) seq[i]=1;
    }

    fprintf(stderr, "%s loaded.\n", thischr);

    atcnt=0;
    cnt = 0;
    ie=-1; is = -1;
    iss=-1;
    /*
    for (ee=0;ee<=MAXLEN;ee++){
      cnt++;
      if (seq[ee]==1) atcnt++;
      if ((float)atcnt / (float)cnt > 0.8){
	if (is!=-1) is=ee;
	ie=ee+1000;
      }

      }*/

    

    for (ee=0;ee<=MAXLEN;ee+=1000){
      cnt = 0;
      for (j=ee;j<=ee+1000, j<MAXLEN;j++){
	if (seq[j]==1) cnt++;
      }

      if (cnt>900){
	if (is!=-1) is=ee;
	ie=ee+1000;
      }
      else{
	printf("%s\t%d\t%d\n", thischr, is, ie);
	is = -1; ie=-1;
      }

    }
    


  }

    
}
