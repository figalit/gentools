#include <stdio.h>
#define MAX 40000

typedef struct gene{
  int s, e;
  char genename[100];
  char transcript[100];
  char chr[10];
}_gene;

int main(int argc, char **argv){
  FILE *fasta;
  FILE *name;
  int i;
  char fname[100], nname[100];
  char chr[100]; int s, e;
  char gene[100], trans[100];
  int count;
  char ch;
  char rest[1000];
  char from[100];
  char to[100];

  struct gene table[MAX];

  fname[0]=0; nname[0]=0;
  for (i=1; i<argc;i++){
    if (!strcmp(argv[i], "-f")) strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-n")) strcpy(nname, argv[i+1]);
  }

  fasta = fopen(fname, "r");
  name = fopen(nname, "r");
  
  if (fasta==NULL || name==NULL) return 0;

  i=0;
  
  
  
  while (fscanf(name, "%s\t%d\t%d\t%s\t%s\n", chr, &s, &e, trans, gene) > 0){
    strcpy(table[i].chr, chr);    
    strcpy(table[i].transcript, trans);
    strcpy(table[i].genename, gene);
    table[i].s=s; table[i].e=e;
    i++;
  }
  count=i;
  fprintf(stderr, "%d genes loaded\n", count);

  fclose(name);


  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch == '>'){
      fscanf(fasta, "%s %s %d %s %d %s\n", chr, from, &s, to, &e, rest);
      for (i=0; i<count; i++){
	if (!strcmp(chr, table[i].chr) && s==table[i].s && e==table[i].e){
	  printf(">%s-%s\n", table[i].transcript, table[i].genename);
	}
      }
    }
    else printf("%c", ch);
  }
  
}
