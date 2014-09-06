#include <stdio.h>
#include <stdlib.h>

#define MAX 250000000
#define EXTRACT 1

int seq[MAX];

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
  char bacfile[100];

  if (argc<3) 
    return;

  bacfile[0]=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(acghfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-bac"))
      strcpy(bacfile, argv[i+1]);
  }

  if (bacfile[0]==0)
    border = fopen(wssdfile, "r");
  acgh = fopen(acghfile, "r");


  if ((bacfile[0]==0 && border==NULL) || acgh==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");


  if (bacfile[0]==0){

    for (i=1;i<=22;i++){
      
      fprintf(stderr, "chr%d\n", i);
      sprintf(thischr, "chr%d", i);
      fprintf(stderr, "-->%s\n", thischr);
      rewind(border); rewind(acgh); 
      dochr(thischr, acgh, border);
    }
    
    
    fprintf(stderr, "chrX\n");
    sprintf(thischr, "chrX");
    rewind(border); rewind(acgh); 
    dochr(thischr, border, acgh);
    
    fprintf(stderr, "chrY\n");
    sprintf(thischr, "chrY");
    rewind(border);  rewind(acgh);
    dochr(thischr, acgh, border);
    
  }

  else{
    sprintf(thischr, bacfile);
    border = fopen(bacfile, "r");
    rewind(border); rewind(acgh); 
    dochr(thischr, acgh, border);
  }    


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
  int log2;
  int *thiswin;
  int count;   int sukcount;
  float mean;
  int total;


  memset(seq, -1, sizeof(int)*MAX);
  
  while (fscanf(acgh, "%s\t%d\t%d\n", chr, &s, &log2) > 0){
    if (strcmp(chr, thischr)) continue;
    seq[s] = log2;
  }
  
  
  fprintf(stderr, "bp_depth load done, reading windows: %s\n", thischr);

  while (fscanf(border, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;

    count=0; sukcount=0;

    for (i=s;i<=e;i++) if (seq[i]!=-1) count++; 

    //    if (count < 10 ) continue;

    thiswin = (int *) malloc(sizeof(int) * count);
    total = 0;
    j=0;
    for (i=s;i<=e;i++){
      if (seq[i]!=-1){
	thiswin[j++] = seq[i]; total += seq[i];
      }
    }
    
    qsort(thiswin, count, sizeof (int),  compare);
    
    if (count != 0)
      fprintf(dump, "%s\t%d\t%d\t%d\t%f\n", chr, s, e,  thiswin[count/2], ((float)total/(float)count));
    else
      fprintf(dump, "%s\t%d\t%d\t0\t0\n", chr, s, e);

    free(thiswin);
  }

}






static int compare(const void *p1, const void *p2){
  float a, b;

  a = *((int *)p1);
  b = *((int *)p2);

  if (a>b) 
    return 1;
  else
    return -1;
}

