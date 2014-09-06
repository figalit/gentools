#include <stdio.h>

#define MAX 250000000
#define EXTRACT 1

char seq[MAX];


void dochr(char *thischr, FILE *one);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *one;
  int i, j;

  char fname[100];
  char thischr[100];



  one = fopen(argv[1], "r");


  if (one==NULL)
    return;

  sprintf(fname, "%s.extended", argv[1]);
  dump = fopen(fname, "w");

  memset(totvenn, 0, sizeof(int)*8);

  /*
  fprintf(stderr, "16p12\n");
  sprintf(thischr, "16p12");
  rewind(one); 
  dochr(thischr, one);

  fprintf(stderr, "H1\n");
  sprintf(thischr, "H1");
  rewind(one); 
  dochr(thischr, one);

  fprintf(stderr, "H2\n");
  sprintf(thischr, "H2");
  rewind(one); 
  dochr(thischr, one);
  */


  
  //for (i=16;i<=;i++){
  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(one); 
    dochr(thischr, one);
  }

  
  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(one); 
  dochr(thischr, one);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(one); 
  dochr(thischr, one);


  for (i=1;i<=22;i++){
    if (i==11 || i==14 || i==16 || i==18 || i ==20 || i ==21) continue;
    fprintf(stderr, "chr%d_random\n", i);
    sprintf(thischr, "chr%d_random", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(one); 
    dochr(thischr, one);
  }


  fprintf(stderr, "chrX_random\n");
  sprintf(thischr, "chrX_random");
  rewind(one); 
  dochr(thischr, one);
  

  
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


void dochr(char *thischr, FILE *one){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo, inthree;
  int venn[8];
  int started=0;
  
  
  memset(seq, 0, sizeof(char)*MAX);
  
  
  while (fscanf(one, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]=1;
  }
  
  inone=0; intwo=0; inthree=0;

  memset(venn, 0, sizeof(int)*8);

  if (EXTRACT){
    for (i=0;i<MAX-1000;i++){
      if (seq[i]!=0 && !started){
	fprintf(dump, "%s\t%d\t", thischr, i);
	fflush(dump);
	started=1;
      }
      else if (started && seq[i]==0){// && seq[i+1000]==0){
	started=0;
	fprintf(dump, "%d\n", i-1);
      }
    }
  }

  
  

}
