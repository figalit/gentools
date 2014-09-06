
/* read 1kb copy number estimates, and calculate median/mean basepair copy numbers in a BED file */

#include <stdio.h>
#include <stdlib.h>

#define MAX 250000000
#define EXTRACT 1

float seq[MAX];

static int compare(const void *p1, const void *p2);

void dochr(char *thischr, FILE *border, FILE *acgh);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *border, *acgh, *rep;
  int i, j;


  char thischr[100];
  char acghfile[100];
  char wssdfile[100];
  char outfile[100];
  char repeatfile[100];


  if (argc<3) 
    return;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(acghfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-r"))
      strcpy(repeatfile, argv[i+1]);
  }

  border = fopen(wssdfile, "r");
  acgh = fopen(acghfile, "r");
  //rep = fopen(repeatfile, "r");
  

  if (border==NULL || acgh==NULL)// || rep==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");

  fprintf(dump, "chrom\tstart\tend\tbp_length\tmedian_cn\tavg_cn\n");

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
  float gc;

  char rest[1000];

  memset(seq, 0, sizeof(float)*MAX);
  
  
  while (fscanf(acgh, "%s", chr) > 0){
    //while (fscanf(acgh, "%s\t%d\t%d\t%f\n", chr, &s, &e, &log2) > 0){
    if (strcmp(chr, thischr)) continue;
    fscanf(acgh, "\t%d\t%d\t%f\t%f\n",  &s, &e, &gc, &log2);
    if (s>e){ fprintf(stderr, "Start coordinate (%d) is bigger then end (%d). Exiting.\n", s, e); exit(0); }
    for (i=s; i<=e; i++) seq[i] = log2;
  }
  
  
  fprintf(stderr, "Copy number table is loaded, reading gene BED file\n");

  while (fscanf(border, "%s\t%d\t%d", chr, &s, &e) > 0){
    fgets(rest, 1000, border);
    rest[strlen(rest)-1] = 0;
    if (strcmp(chr, thischr)) continue;
    if (s>e){ fprintf(stderr, "Start coordinate (%d) is bigger then end (%d). Exiting.\n", s, e); exit(0); }
    count=0; 

    //    for (i=s;i<=e;i++) if (seq[i]!=0) count++;
    count = e-s+1;

    //    if (count < 10 ) continue;

    thiswin = (float *) malloc(sizeof(float) * count);
    total = 0;
    j=0;
    for (i=s;i<=e;i++){
      total += seq[i];
      //if (seq[i]!=0)
      thiswin[j++] = seq[i];
    }
    
    qsort(thiswin, count, sizeof (float),  compare);
    
    if (total != 0)
      fprintf(dump, "%s\t%d\t%d\t%d\t%s\t%f\t%f\n", chr, s, e, count, rest, thiswin[count/2], (total/count));
    else
      fprintf(dump, "%s\t%d\t%d\t%d\t%s\t%f\t%f\n", chr, s, e, count, rest, 0, 0);
      
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

