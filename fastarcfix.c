
/*
  given a fasta file, renames sequences that are the same until a whitespace
  is found
  last update: Oct 20, 2005
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
  int reverse;
  
  seqcnt=0;


  if (argc != 2){
    fprintf(stderr, "%s [fastafile]\n", argv[0]);
    fprintf(stderr, "Given a fasta file, renames sequences that are the same until a whitespace is found.\n");
    fprintf(stderr, "Last update: Oct 20, 2005\n");
    return 0;
  }
  
  in = fopen(argv[1], "r");
  
  
  
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
  
  while (fscanf(in, "%c", &ch) > 0){
    if (ch =='>'){
      fgets(name, 1000, in);
      printf(">%s", name);
      
      //if (strcmp(name_f, exname_f)){ // completely new
      if (strstr(name, "Reverse"))
	reverse=1;
      else
	reverse=0;
    }
    else{
      if (reverse){
	if (toupper(ch) == 'A')
	  printf("T");
	else if (toupper(ch) == 'T')
	  printf("A");
	else if (toupper(ch) == 'G')
	  printf("C");
	else if (toupper(ch) == 'C')
	  printf("G");
	else
	  printf("%c", ch);
      }
      else 
	printf("%c", ch);
    }

  }
	 
  return 1;
}


