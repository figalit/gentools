#include <stdio.h>

char cover[3000000];

int main(int argc, char **argv){
  int i;
  int j;
  int zero, one;
  char fname[100];
  FILE *fp;
  char line[1000];

  char gene[100];
  int gs, ge, gl;
  
  char hit[100];
  int hs, he, hl;
  int numfrag=0;

  char prevhit[100];

  fname[0] = 0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
  }

  fp = fopen (fname, "r");

  fgets(line, 1000, fp); // pass header

  memset(cover, 0, sizeof(char)*3000000);
  prevhit[0]=0;

  while(fscanf(fp, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t", gene, &gs, &ge, &gl, hit, &hs, &he, &hl) > 0){
    fgets(line, 1000, fp); // pass the rest
    for (i=gs; i<=ge; i++){
      if (cover[i] == 0){
	if (strcmp(prevhit, hit)){

	  zero=0; one=0;

	  for (j=gs;j<=ge;j++)
	    one+=cover[j];
	  zero = ge-gs+1-one;

	  //if (zero >= 2*one){
	  if (one==0){
	    numfrag++;
	    strcpy(prevhit, hit);
	    //printf("Frac hit:\t%s\treason:%d-%d\tzero:%d\tone:\t%d\n", hit, gs, ge, zero, one);
	  }
	}
      }
      cover[i] = 1;
    }

  }

 
  printf("%s\t%d\n", fname, numfrag);
  
}
