#include <stdio.h>
main(){
	int numcol;
	int numexon;
	int i;
	char chr[50];
	char gene[50];
	char acc[80];
	int s, e, gs, ge, txs, txe;	
	char line[100];
	int start[10000]; int end[10000];
	char strand;
	while (fscanf(stdin, "%s\t", gene) > 0){
	  fscanf(stdin, "%s\t%s\t%c\t%d\t%d\t%d\t%d\t%d\t", acc, chr, &strand, &gs, &ge, &txs, &txe, &numexon);
	  //	  fprintf(stderr, "%d\texons\tgene\t%s\tacc\t%s\tchr\t%s\n", numexon, gene, acc, chr);
		for (i=0; i < numexon; i++){
		  fscanf(stdin, "\t%d", &s);
		  start[i]=s;
		}
		fscanf(stdin, "\t");
		for (i=0; i < numexon; i++){
		  fscanf(stdin, "\t%d", &e);
		  end[i]=e;
		}

		fgets(line, 100, stdin);
		for (i=0; i < numexon; i++){
		  s = start[i]; e = end[i];
		  printf("%s\t%d\t%d\t%s\t%s\n", chr, s, e, gene, acc);
		}
		

	}
}
