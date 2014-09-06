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


  if (EXTRACT)
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
    

  fprintf(stdout, "\n\nTOTAL\n\n");
  fprintf(stdout, "\tone: %d", totvenn[1]+totvenn[3]+totvenn[5]+totvenn[7]);
  fprintf(stdout, "\ttwo: %d", totvenn[2]+totvenn[3]+totvenn[6]+totvenn[7]);
  
  fprintf(stdout, "\toneonly: %d", totvenn[1]);
  fprintf(stdout, "\ttwoonly: %d", totvenn[2]);
  
  fprintf(stdout, "\tone-two: %d", totvenn[3]);

  fprintf(stdout, "\n");

  



}


void dochr(char *thischr, FILE *one, FILE *two){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo;
  int venn[8];
  int started=0;
  
  
  memset(seq, 0, sizeof(char)*MAX);
  
  
  while (fscanf(one, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=1;
  }
  
  while (fscanf(two, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=2;
  }
  
  inone=0; intwo=0; 
  memset(venn, 0, sizeof(int)*8);
  
  for (j=0;j<MAX;j++){
    venn[seq[j]]++;
    totvenn[seq[j]]++;
    switch(seq[j]){  
    case 1: inone++;  break;	
    case 2: intwo++;  break;	
    case 3: inone++; intwo++; break;	
    default: 
      break;	
    }
  }

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

  
  fprintf(stdout, "%s", thischr);
  fprintf(stdout, "\tone: %d", inone);
  fprintf(stdout, "\ttwo: %d", intwo);

  
  fprintf(stdout, "\toneonly: %d", venn[1]);
  fprintf(stdout, "\ttwoonly: %d", venn[2]);
  
  fprintf(stdout, "\tone-two: %d", venn[3]);

  fprintf(stdout, "\n");
  

}
