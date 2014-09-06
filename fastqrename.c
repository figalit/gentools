#include <stdio.h>

int main(int argc, char **argv){
  FILE *fastq = fopen(argv[1], "r");
  FILE *names = fopen(argv[2], "r");

  char suff = argv[3][0];

  char name[100], seq[100], qname[100], qual[100];
  char run[100];
  int lane, tile, x, y;

  while (fscanf(fastq, "%s\n%s\n%s\n%s\n", name, seq, qname, qual) > 0){
    fscanf(names, "%s\t%d\t%d\t%d\t%d\n", run, &lane, &tile, &x, &y);
    fprintf(stdout, "@%s:%d:%d:%d:%d/%c\n%s\n+\n%s\n", run, lane, tile, x, y, suff, seq, qual);
  }
  
  
}
