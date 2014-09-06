#include <stdio.h>

int main(int argc, char **argv){
  char infname[1000];
  char outdir[1000];
  int i;
  FILE *in;
  char outfname[2000];
  FILE *out1, *out2;

  char name1[1000], seq1[1000], plus1[1000], qual1[1000];
  char name2[1000], seq2[1000], plus2[1000], qual2[1000];
 
  strcpy(outdir, "./");

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i")) strcpy(infname, argv[i+1]);
    else if (!strcmp(argv[i], "-o")) strcpy(outdir, argv[i+1]);
  }

  in = fopen(infname, "r");
  
 
  sprintf(outfname, "%s/%s_1", outdir, infname);
  out1 = fopen(outfname, "w");
  
  sprintf(outfname, "%s/%s_2", outdir, infname);
  out2 = fopen(outfname, "w");
  
  while (fscanf(in, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", name1, seq1, plus1, qual1, name2, seq2, plus2, qual2) > 0){
    fprintf(out1, "%s\n%s\n+\n%s\n", name1, seq1, qual1);
    fprintf(out2, "%s\n%s\n+\n%s\n", name2, seq2, qual2);
  }

  fclose(in); fclose(out1); fclose(out2);

}
