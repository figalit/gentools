#include <stdio.h>
#include <string.h>
char * readseq(FILE *fp);

main(){
  FILE *gene;
  FILE *seq;
  char seqs[8][1005];
  char ch;
  int i,j;
  char *searchgene;
  char genename[100];
  char fname[150];
  seq=fopen("seq1","r");
  fscanf(seq, "%s", seqs[0]);
  fclose(seq);
  seq=fopen("seq2","r");
  fscanf(seq, "%s", seqs[1]);
  fclose(seq);
  seq=fopen("seq3","r");
  fscanf(seq, "%s", seqs[2]);
  fclose(seq);
  seq=fopen("seq4","r");
  fscanf(seq, "%s", seqs[3]);
  fclose(seq);
  seq=fopen("seq5","r");
  fscanf(seq, "%s", seqs[4]);
  fclose(seq);
  seq=fopen("seq6","r");
  fscanf(seq, "%s", seqs[5]);
  fclose(seq);
  seq=fopen("seq7","r");
  fscanf(seq, "%s", seqs[6]);
  fclose(seq);
  seq=fopen("seq8","r");
  fscanf(seq, "%s", seqs[7]);
  fclose(seq);
  //printf("read all \n\n%s\n\n%s\n\n", seqs[0], seqs[1]);
  while(scanf("%s", genename) > 0){
    sprintf(fname, "GENES/%s", genename);
    gene = fopen(fname,"r");
    searchgene = readseq(gene);
    fclose(gene);
    for (i=0;i<8;i++){
      if (strstr(searchgene, seqs[i]) || strstr(seqs[i], searchgene))
	printf("seq %d and gene %s\n", (i+1) , genename);
    }
    //fprintf(stderr, "%s tested\n", genename);
  }

}

char * readseq(FILE *fp){
  int i=0;
  char ch;
  static char *sgene;
  sgene = (char *)malloc(15000);
  while (fscanf(fp, "%c", &ch) > 0){
    if (isalpha(ch))
      sgene[i++] = ch;
  }
  sgene[i]=0;
  return sgene;
}
