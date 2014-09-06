
/*
  given a fasta file, renames sequences that are the same until a whitespace
  is found
  last update: Oct 20, 2005
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inthere(char *);
char **names;
int seqcnt;

int main(int argc, char **argv){
  
  FILE *in;
  char ch;
  char line[1000];
  char *name;
  char exname[1000];
  int i;
  int cnt;
  char name_f[1000];
  char exname_f[1000];
  int place=0;
  int lim;

  seqcnt=0;


  if (argc != 2){
    fprintf(stderr, "%s [fastafile]\n", argv[0]);
    fprintf(stderr, "Given a fasta file, renames sequences that are the same until a whitespace is found.\n");
    fprintf(stderr, "If two sequences with similar names THAT HAS TO BE RENAMED are not next to each other, it won't work.\n");
    fprintf(stderr, "In that case, use fastasort.sh first.\n");
    fprintf(stderr, "Last update: March 8, 2006\n");
    return 0;
  }

  in = fopen(argv[1], "r");
  
  if (in == NULL){
    fprintf(stderr, "Unable to open file %s\n", argv[1]);
    return 0;
  }
  

  name = (char *)malloc(1000*sizeof(char));
  name[0]=0;
  name_f[0]=0;
  exname_f[0]=0;
  exname[0]=0;
  cnt = 1;

  while (fscanf(in, "%c", &ch)>0)
    if(ch=='>') seqcnt++;
  
  rewind(in);

  names = (char **)malloc((seqcnt+1)*sizeof(char *));
  for (i=0;i<=seqcnt;i++)
    names[i] = (char *) malloc(100*sizeof(char));

  for (i=0;i<=seqcnt;i++)
    names[i][0] = 0;

  while (fgets(line, 1000, in) > 0){ 
    if (line[0] == '>'){ // new sequence
      strcpy(name, line);
      for (i=0;i<strlen(name);i++){
	if (name[i] == ' ' || i==28 || isspace(name[i]))
	  break;
	else
	  name_f[i] = name[i];
      }
      name_f[i] = 0;
      if (strcmp(name_f, exname_f)){ // completely new
	//if (!inthere(name_f)){ // completely new
	printf("%s.0  %s\n", name_f, (name+strlen(name_f)+1));
	//printf("%s", line);
	cnt = 1;
	strcpy(exname_f, name_f);
	strcpy(names[place++], name_f);
      }
      else{
	printf("%s.%d  %s\n", name_f, cnt, (name+strlen(name_f)+1));
	cnt++;
      }
    }
    else
      printf("%s", line);
  }
  

  return 1;
}


int inthere(char *s){
  int i;
  for (i=0;i<=seqcnt;i++)
    if (!strcmp(s, names[i]))
      return 1;
  return 0;
}
