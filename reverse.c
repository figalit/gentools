#include <stdio.h>
#include <string.h>

#define MAX 250000000

char seq[MAX];
main(int argc, char **argv){
  char outfile[40];
  char ch;

  char seqname[100];
  int cnt=0; int i;
  FILE *in;
  FILE *out;

  if (argc != 3){
    fprintf(stderr, "%s <infile> <outfile>\n");
    return -1;
  }

  in =fopen(argv[1],"r");

  
  if (in==NULL){
    fprintf(stderr, "Cannot open %s\n", argv[1]);
    return -1;
  }

  out = fopen(argv[2],"w");

  if (out==NULL){
    fprintf(stderr, "Cannot open %s\n", argv[2]);
    return -1;
  }
  

  while(fscanf(in,"%c",&ch) > 0){
    if (ch == '>')
      fgets(seqname, 100, in);
    else if (ch!='\n'){
      seq[cnt++]=ch;
      /*
      if(ch=='A')
	seq[cnt++]='T';
      else if(ch=='C')
	seq[cnt++]='G';
      else if(ch=='G')
	seq[cnt++]='C';
      else if(ch=='T')
	seq[cnt++]='A';
      */
    }
  }
  seq[cnt] = 0;
  seqname[strlen(seqname)-1]=0;
  fprintf(out, ">%s\n", seqname);
  for (i=cnt-1;i>=0;i--){
    fprintf(out,"%c",seq[i]);
    if (i%60==0)
      fprintf(out,"\n");
  }
    
}
