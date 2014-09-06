#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv){
  char *str;
  FILE *tab;
  FILE *log;
  char exseq[1000];
  char line[1000];
  int i,j;
  int count;
  char **exnames;
  int cluster;
  int FREQCUTOFF;

  if (argc != 3){
    fprintf(stderr, "Cluster duplicate sequences given a SORTED fasta-tab file\n");
    fprintf(stderr, "Last update: Nov 10, 2005.\n");
    fprintf(stderr, "%s [tabfile] [freqcutoff]\n", argv[0]);
    exit(0);
  }
  
  tab = fopen(argv[1], "r");
  str = (char *) malloc(sizeof(char)*1000);
  sprintf(str, "%s.%s.clusters", argv[1], argv[2]);
  log = fopen(str, "w");
  exseq[0] = 0;
  count = 1;
  cluster = 0;
  FREQCUTOFF = atoi(argv[2]);

  if (FREQCUTOFF == 0){
    fprintf(stderr, "I didnt like that freqcutoff.\n");
    return 0;
  }

  exnames = (char **) malloc (sizeof(char *) * (FREQCUTOFF+1));

  for (i=0; i<=FREQCUTOFF;i++){
    exnames[i] = (char *) malloc(sizeof(char) * 100); 
    exnames[i][0] = 0;
  }

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
      // sequence name is in line; without '>'
      fscanf(tab, "%s", str);
      if (!strcmp(str, exseq)){
	count++;
	if (count==FREQCUTOFF){
	  fprintf(log, "\nCLUSTER: %d\n----------\n%", ++cluster);
	  for (j=0;j<FREQCUTOFF-1;j++)
	    fprintf(log, "%s\n", exnames[j]);
	  fprintf(log, "%s\n", line);	  
	}
	else if (count > FREQCUTOFF)
	  fprintf(log, "%s\n", line);	  
	else{ 
	  strcpy(exnames[count-1], line);
	}
      }
      else{
	count = 1;
	strcpy(exseq, str);
	strcpy(exnames[0], line);
      }
    }
  }

  return 1;
}
