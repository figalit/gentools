#include <stdio.h>
#include <string.h>

main(){
  FILE *in=fopen("EcoGene17.lib", "r");
  int i=1;
  char fname[100];
  FILE *out=NULL;
  char line[1000];
  char ch;
  while (fscanf(in, "%c", &ch) > 0){
    if (ch == '>'){
      if (out != NULL)
	fclose(out);
      fgets(line, 1000, in);
      sprintf(fname, "gene%d.fa", i);
      i++;
      out = fopen(fname, "w");
    }
    else
      fprintf(out, "%c", ch);
  }
}
