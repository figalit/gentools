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
  char bacfile[100];

  if (argc<3) 
    return;

  bacfile[0]=0;

  for (i=1;i<argc;i++){
    //    if (!strcmp(argv[i], "-w"))
    //strcpy(wssdfile, argv[i+1]);
    if (!strcmp(argv[i], "-a"))
      strcpy(artfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-bac"))
      strcpy(bacfile, argv[i+1]);
  }

  //one = fopen(wssdfile, "r");
  two = fopen(artfile, "r");


  //if (one==NULL || two==NULL)
  if (two==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");

  if (bacfile[0]==0){

    for (i=1;i<=22;i++){
      
      fprintf(stderr, "chr%d\n", i);
      sprintf(thischr, "chr%d", i);
      sprintf(wssdfile, "chr%d.coverage", i); one = fopen(wssdfile, "r");
      fprintf(stderr, "-->%s\n", thischr);
      rewind(one); rewind(two); 
      dochr(thischr, one, two);
      fclose(one);
    }
    
    
    fprintf(stderr, "chrX\n");
    sprintf(thischr, "chrX");
    sprintf(wssdfile, "chrX.coverage"); one = fopen(wssdfile, "r");
    rewind(one); rewind(two); 
    dochr(thischr, one, two);
    fclose(one);
    
    fprintf(stderr, "chrY\n");
    sprintf(thischr, "chrY");
    sprintf(wssdfile, "chrY.coverage"); one = fopen(wssdfile, "r");
    rewind(one); rewind(two); 
    dochr(thischr, one, two);
    fclose(one);
  }

  else{
    one = fopen(bacfile, "r");
    fprintf(stderr, "-->%s\n", bacfile);
    rewind(one); rewind(two); 
    dochr(bacfile, one, two);
    fclose(one);
  }


}


void dochr(char *thischr, FILE *one, FILE *two){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo;
  int venn[8];
  int started=0;
  
  
  memset(seq, 0, sizeof(char)*MAX);
  
  
  while (fscanf(two, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]=1;
  }
  
  fprintf(stderr, "%s repeat table loaded\n", thischr);
  while (fscanf(one, "%d\t%d\n", &s, &e) > 0){
    //    if (strcmp(chr, thischr)) continue;
    if (seq[s-1] == 0)
      fprintf(dump, "%s\t%d\t%d\n", thischr, s, e);
  }
  



  

}
