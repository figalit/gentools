#include <stdio.h>
main(){
	int numcol;
	int numexon;
	int i;
	char chr[50];
	char gene[50];
	char acc[80];
	int s, e;	
	char line[100];
	int start[10000]; int end[10000];

	while (fscanf(stdin, "%d\t", &numexon) > 0){
	  fscanf(stdin, "%s\t%s\t%s", chr, gene, acc);
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
