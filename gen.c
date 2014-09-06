#include <stdio.h>
#include <string.h>
#include <time.h>
main(){
  char fname[20];
  FILE *in;
  char outfname[30];
  FILE *out;
  int ntax;
  int i,k;
  int exit=0;
  int taxno=1;
  char ch;
  int seed;
  while (scanf("%s",fname) > 0){
    exit=0;
    in = fopen(fname,"r");
    sprintf(outfname,"%s.paup",fname);
    out = fopen(outfname,"w");
    fprintf(out,"#NEXUS\n");
    fprintf(out,"begin data;\n");
    ntax=count(in);
    printf("cnt: %d\n",ntax);
    rewind(in);
    fprintf(out,"\tdimensions ntax=%d nchar=180;\n",ntax);
    fprintf(out,"\tformat datatype=DNA;\n");
    fprintf(out,"\tmatrix\n");
    i=0;
    while(fscanf(in,"%c",&ch) > 0){
      printf("hebele: %c\n",ch);
      while (ch != '\n' && ch!='\r'){
	fscanf(in, "%c", &ch);
      }
      fprintf(out,"\t  taxon_%d ",taxno);
      taxno++;
      while (ch != '>'){
	if (!(fscanf(in, "%c", &ch) > 0)){
	  exit=1;
	  break;
	}
	if (ch!='>' && ch!='\n' && ch!='\r'){
	  fprintf(out,"%c",ch);
	  i++;
	}
      }
      if (i<179)
	for (k=i;k<180;k++)
	  fprintf(out,"N");
      if (taxno==ntax)
	fprintf(out,";\n");
      else
	fprintf(out,"\n");
      i=0;
      if (exit)
	break;
    }
    fprintf(out,"end;\n");
    fprintf(out,"\nbootstrap treefile=%s.bootstrap;\n",fname);
    printf("%s done\n",outfname);
    fclose(out);
    fclose(in);
  }
}

int count(FILE *in){
  int cnt=0;
  char ch;
  while (fscanf(in,"%c",&ch) > 0)
    if (ch=='>')
      cnt++;
  return cnt;
}
