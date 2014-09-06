#include <stdio.h>

#define MAX 250000000
#define EXTRACT 1

char seq[MAX];


void dochr(char *thischr, FILE *one, FILE *two);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *one, *two;
  int i, j;


  char thischr[100];
  char artfile[100];
  char wssdfile[100];
  char outfile[100];


  if (argc<3) 
    return;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(artfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
  }

  one = fopen(wssdfile, "r");
  two = fopen(artfile, "r");


  if (one==NULL || two==NULL)
    return;



  dump = fopen(outfile, "w");

  memset(totvenn, 0, sizeof(int)*8);

  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(one); rewind(two); 
    dochr(thischr, one, two);
  }


  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(one); rewind(two); 
  dochr(thischr, one, two);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(one); rewind(two); 
  dochr(thischr, one, two);
    
  fprintf(stderr, "chr2a\n");
  sprintf(thischr, "chr2a");
  rewind(one); rewind(two); 
  dochr(thischr, one, two);
    
  fprintf(stderr, "chr2b\n");
  sprintf(thischr, "chr2b");
  rewind(one); rewind(two); 
  dochr(thischr, one, two);
    


}


void dochr(char *thischr, FILE *one, FILE *two){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo;
  int venn[8];
  int started=0;
  char rest[1000];
  
  
  memset(seq, 0, sizeof(char)*MAX);
  
  
  while (fscanf(two, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]=1;
  }
  
  while (fscanf(one, "%s\t%d", chr, &s) > 0){
    fgets(rest, 1000, one);
    if (strcmp(chr, thischr)) continue;

    if (seq[s]==0)
      fprintf(dump, "%s\t%d%s", chr, s, rest);
  }
  
  

}
