
/* 
   BLAST output "hit" listing 
   Update: Oct 13, 2005
*/ 
#include <stdio.h>

int main(int argc, char **argv){
  FILE *in;
  FILE *out;
  char tmp[200];
  char name[200];
  char score[200];
  char evalue[200];
  if (argc != 2){
    fprintf(stderr, "BLAST output listing program.\n%s [bo file]\n", argv[0]);
    return 0;
  }
  
  in = fopen(argv[1], "r");
  
  if (in == NULL){
    fprintf(stderr, "Can not open %s.\n", argv[1]);
    return 0;
  }

  sprintf(tmp, "%s.list", argv[1]);
  out = fopen(tmp, "w");
  tmp[0]=0;

  while(strcmp(tmp, "Value"))
    fscanf(in, "%s", tmp);
  // Value found now comes the list

  name[0]=0;
  
  while(name[0] != '>'){
    fscanf(in, "%s%s", name, score);
    while (!isdigit(score[0]))
      fscanf(in, "%s", score);
    fscanf(in, "%s", evalue);
    if (name[0]!='>')
      fprintf(out, "%s\n", name);
  }
  
  fclose(in);
  fclose(out);
  return 1;
}
