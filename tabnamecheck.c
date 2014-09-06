#include <stdio.h>
#include <string.h>

main(int argc, char **argv){
  char str[1000];
  FILE *tab;
  FILE *fasta;
  FILE *log;
  char exseq[1000];
  char line[10000];
  int i;
  int count;

  if (argc != 3){
    fprintf(stderr, "Eliminates duplicate sequences given a SORTED fasta-tab file\n");
    fprintf(stderr, "%s [tabfile] [fastafile]\n", argv[0]);
    exit(0);
  }
  
  tab = fopen(argv[1], "r");
  fasta = fopen(argv[2], "w");
  sprintf(str, "%s.log", argv[2]);
  log = fopen(str, "w");
  exseq[0] = 0;
  count = 1;

  while (fscanf(tab, "%s", str) > 0){
    if (str[0]=='>'){
      line[0]=0;
      do{
	strcat(line, str);
	strcat(line, " ");
	//fprintf(fasta, "%s " str);
	fscanf(tab, "%s", str);
      } while (strcmp(str, "--==--"));
      // str is now --==-- get the sequence
      fscanf(tab, "%s", str);
      if (strcmp(line, exseq)){
	if (count>0){
	  fprintf(log, "Sequence %s repeated %d times.\n", exseq, count);
	  count=1;
	}
	fprintf(fasta, "%s\n", line);
	for (i=0;i<strlen(str);i++){
	  fprintf(fasta, "%c", str[i]);
	  if (i!=0 && i%60==0)
	    fprintf(fasta, "\n");
	}
	fprintf(fasta, "\n");
	strcpy(exseq, line);
      }
      else
	count++;
    }
  }
  
}
