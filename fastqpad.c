#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
  

  char name[500], seq[500], qual[500], plus[500];
  int len=atoi(argv[1]);
  int i;

  if (len <= 0) return 1;
  
  while (1){
    if (feof(stdin)) break;
    fgets(name, 500, stdin);
    if (feof(stdin)) break;
    fgets(seq, 500, stdin);
    fgets(plus, 500, stdin);
    fgets(qual, 500, stdin);

    if (strlen(seq) < len+1){
      for (i = strlen(seq)-1; i<len; i++){
	seq[i]='N';
	qual[i]='!';
      }
      seq[i]='\n'; qual[i]='\n';
    }

    if (strlen(seq) > len+1){
	seq[len]='\n';
	qual[len]='\n';
	seq[len+1]=0;
	qual[len+1]=0;
    }

    fprintf(stdout, "%s%s+\n%s", name, seq, qual);
    
  }
  return 0;
}
