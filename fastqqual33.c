#include <stdio.h>

int main(int argc, char **argv){


  char name[500], seq[500], qname[500], qual[500];
  char run[500];
  int i;
  int lane, tile, x, y;

  while (fscanf(stdin, "%s\n%s\n%s\n%s\n", name, seq, qname, qual) > 0){
    fprintf(stdout, "%s\n%s\n%s\n", name, seq, qname);
    for (i=0; i<strlen(qual); i++){
      fprintf(stdout, "%c", (qual[i] - 31));
    }
    fprintf(stdout, "\n");
  }
  
  
}
