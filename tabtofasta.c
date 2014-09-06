#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LEN 1000

int main(int argc, char **argv){
  char str[MAX_LEN];
  FILE *tab;
  FILE *fasta;
  //FILE *log;
  char line[100000];
  int i;
  int cnt;
  char ch;

  if (argc != 3){
    fprintf(stderr, "Given a SORTED (ideally) tab file, converts it back to Fasta\n");
    fprintf(stderr, "%s [tabfile] [fastafile]\n", argv[0]);
    exit(0);
  }
  
  tab = fopen(argv[1], "r");
  fasta = fopen(argv[2], "w");

  /*
  sprintf(str, "%s.log", argv[2]);
  log = fopen(str, "w");
  */

  while (fscanf(tab, "%s", str) > 0){
    if (str[0]=='>'){
      line[0]=0;
      do{
	strcat(line, str);
	strcat(line, " ");
	fscanf(tab, "%s", str);
      } while (strcmp(str, "--==--"));
      // str is now --==-- get the sequence
      //fscanf(tab, "%s", str);
      /*
      fgets(str, MAX_LEN, tab);
      str[strlen(str)-1] = 0;

      cnt=0;
      for (i=0;i<strlen(str);i++){
	if (str[i]!='\t')
	  fprintf(fasta, "%c", str[i]);
	if (str[i]==0)
	  cnt++;
	if (cnt!=0 && cnt%30==0)
	  fprintf(fasta, "\n");
	  }
      */
      fprintf(fasta, "%s\n", line);
      cnt = 0;
      while (fscanf(tab, "%c", &ch) > 0){
	if (ch == '\r' || ch == '\n')
	  break;
	fprintf(fasta, "%c", ch);
	if (ch==' ') cnt++;
	if (cnt!=0 && cnt%50==0)
	  fprintf(fasta, "\n");
      }
      fprintf(fasta, "\n");
      
    }
  }
  return 1;
}
