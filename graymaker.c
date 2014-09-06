#include <stdio.h>

#define MAX 250000000
#define EXTRACT 1

char seq[MAX];


void dochr(char *thischr, FILE *one, float gray, float red);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *one;
  int i, j;
  float gray;
  float red;

  char thischr[100];



  one = fopen(argv[1], "r");
  gray = atof(argv[2]);
  red = atof(argv[3]);

  if (one==NULL)
    return;

  
  dump = fopen("gray.tab", "w");

  memset(totvenn, 0, sizeof(int)*8);

  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(one); 
    dochr(thischr, one, gray, red);
  }


  /*
  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(one); 
  dochr(thischr, one);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(one); 
  dochr(thischr, one);
  */

  /*

  fprintf(stdout, "\n\nTOTAL\n\n");
  fprintf(stdout, "\tone: %d", totvenn[1]+totvenn[3]+totvenn[5]+totvenn[7]);
  fprintf(stdout, "\ttwo: %d", totvenn[2]+totvenn[3]+totvenn[6]+totvenn[7]);
  if (third)
    fprintf(stdout, "\tthree: %d", totvenn[4]+totvenn[5]+totvenn[6]+totvenn[7]);
  
  fprintf(stdout, "\toneonly: %d", totvenn[1]);
  fprintf(stdout, "\ttwoonly: %d", totvenn[2]);
  if (third)
    fprintf(stdout, "\tthreeonly: %d", totvenn[4]);
  
  fprintf(stdout, "\tone-two: %d", totvenn[3]);
  fprintf(stdout, "\n");
  */
  



}


void dochr(char *thischr, FILE *one, float gray, float red){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo, inthree;
  int venn[8];
  int started=0;
  float depth;
  
  memset(seq, 0, sizeof(char)*MAX);
  
  
  while (fscanf(one, "%s\t%d\t%d\t%f\n", chr, &s, &e, &depth) > 0){
    if (strcmp(chr, thischr)) continue;
    if (depth>=gray && depth<red)
      for (i=s;i<=e;i++) if (seq[i]!=2) seq[i]=1;
    else 
      for (i=s;i<=e;i++) seq[i]=2;
  }
  
  inone=0; intwo=0; inthree=0;

  memset(venn, 0, sizeof(int)*8);

  if (EXTRACT){
    for (i=0;i<MAX-1000;i++){
      if (seq[i]==1 && !started){
	fprintf(dump, "%s\t%d\t", thischr, i);
	fflush(dump);
	started=1;
      }
      else if (started && seq[i]!=1 && seq[i+1000]!=1){
	started=0;
	fprintf(dump, "%d\n", i);
      }
    }
  }

  
  

}
