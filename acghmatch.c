#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 250000000
#define EXTRACT 1

#define SENTINEL 0

float seq[MAX];

static int compare(const void *p1, const void *p2);

void dochr(char *thischr, FILE *border, FILE *acgh);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *border, *acgh;
  int i, j;


  char thischr[100];
  char acghfile[100];
  char wssdfile[100];
  char outfile[100];


  if (argc<3) 
    return;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(acghfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
  }

  border = fopen(wssdfile, "r");
  acgh = fopen(acghfile, "r");


  if (border==NULL || acgh==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");

  memset(totvenn, 0, sizeof(int)*8);


  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(border); rewind(acgh); 
    dochr(thischr, border, acgh);
  }

  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(border); rewind(acgh); 
  dochr(thischr, border, acgh);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(border);  rewind(acgh); 
  dochr(thischr, border, acgh);


    


}


void dochr(char *thischr, FILE *border, FILE *acgh){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inborder, inacgh;
  int venn[8];
  int started=0;
  int interval=0;
  int  lasti;
  float log2;
  float *thiswin;
  int count;
  float mean;
  float total;
  float median;

  memset(seq, SENTINEL, sizeof(float)*MAX);
  
  
  while (fscanf(acgh, "%s\t%d\t%d\t%f\n", chr, &s, &e, &log2) > 0){
    if (strcmp(chr, thischr)) continue;
    //for (i=s; i<=e; i++) seq[i] = log2;
    seq[s] = log2;
  }
  
  
  fprintf(stderr, "aCGH load done, reading windows\n");

  while (fscanf(border, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;

    count=0; 

    for (i=s;i<=e;i++) if (seq[i] != SENTINEL) count++;
    //count = e-s+1;

    //    if (count < 10 ) continue;

    thiswin = (float *) malloc(sizeof(float) * count);
    total = 0;
    j=0;

    for (i=s;i<=e;i++){
      if (seq[i] != SENTINEL){
	total += seq[i];
	thiswin[j++] = seq[i];
      }
    }
    
    qsort(thiswin, count, sizeof (float),  compare);
    
    if (count != 0){
      if (count % 2 != 0)
	median = thiswin[count/2];
      else{
	median = (thiswin[count/2] + thiswin[count/2 - 1]) / 2;
      }
      fprintf(dump, "%s\t%d\t%d\t\t%d\t%f\t%f\n", chr, s, e, count, median, total/count);
    }
    else
      fprintf(dump, "%s\t%d\t%d\t\t%d\t%f\t%f\n", chr, s, e, count, 0, 0);
      
    free(thiswin);
  }

}






static int compare(const void *p1, const void *p2){
  float a, b;

  a = *((float *)p1);
  b = *((float *)p2);

  if (a>b) 
    return 1;
  else
    return -1;
}

