#include <stdio.h>
#include <assert.h>


#define MAX 250000000

int snpcnt[MAX];

int main(int argc, char **argv){

  int i;

  char bname[200];
  FILE *border;
  char fname[200];
  FILE *snpfile;

  char chr[20];
  char chr2[20];
  int pos;
  int total;
  char snpchar[10];
  char refchar[10];
  int a, c, g, t;
  char type;
  char dummy[1000];

  int s,e;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-b")){
      strcpy(bname, argv[i+1]);
    }
    else if (!strcmp(argv[i], "-i")){
      strcpy(fname, argv[i+1]);
    }
  }

  snpfile = fopen(fname, "r");

  fprintf(stderr, "initializing...\n");
  memset(snpcnt, 0, MAX*sizeof(int));

  fprintf(stderr, "reading SNP table...\n");

  while (fscanf(snpfile, "%s\t%d\t%s\t%s", chr, &pos, &total, snpchar, refchar) > 0){//, &a, &c, &g, &t, &type) > 0){
    fgets(dummy, 1000, snpfile);
    snpcnt[pos-1]++; 
    
    /*
    switch (snpchar[0]){

    case 'A':
      total -= a;
      break;

    case 'C':
      total -= c;
      break;

    case 'G':
      total -= g;
      break;

    case 'T':
      total -= t;
      break;

    default:
      fprintf(stderr, "SNP: %d\t%s\n", pos, snpchar);
      //assert(0);
      break;
    }
    
    snpcnt[pos-1] += total;
    */

  }

  fclose(snpfile);

  border = fopen(bname, "r");

  fprintf(stderr, "calculating SNP count/density...\n");

  while (fscanf(border, "%s\t%d\t%d\n", chr2, &s, &e) > 0){
    total = 0;

    if (!strcmp(chr, chr2)){
      for (i=s; i<=e; i++)
	total += snpcnt[i];
      printf("%s\t%d\t%d\t%d\n", chr2, s, e, total);
    }
  }

}
