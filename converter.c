#include <stdio.h>
#include <string.h>
main(int argc, char **argv){
  char ch;
  char outfname[30];
  FILE *in=fopen(argv[1],"r");
  FILE *out;
  sprintf(outfname,"%s.my",argv[1]);
  out=fopen(outfname,"w");
  while(fscanf(in,"%c",&ch) > 0){
    if (ch=='>'){
      fprintf(out,"\n");
      while (ch!='\n')
	fscanf(in,"%c",&ch);
    }
    else
      if (ch != '\n')
	fprintf(out,"%c",ch);
  }
}
