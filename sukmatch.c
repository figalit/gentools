#include <stdio.h>
#include <stdlib.h>

#define MAX 250000000
#define EXTRACT 1

int seq[MAX];

static int compare(const void *p1, const void *p2);

void dochr(char *thischr, FILE *border, FILE *acgh, FILE *suk);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *border, *acgh;
  FILE *suk;
  int i, j;


  char thischr[100];
  char acghfile[100];
  char wssdfile[100];
  char outfile[100];
  char sukfile[100];

  if (argc<3) 
    return;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(acghfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      strcpy(sukfile, argv[i+1]);
  }

  border = fopen(wssdfile, "r");
  acgh = fopen(acghfile, "r");
  suk = fopen(sukfile, "r");


  if (border==NULL || acgh==NULL || suk==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");

  memset(totvenn, 0, sizeof(int)*8);

  /*
  fprintf(stderr, "chr17_random\n");
  sprintf(thischr, "chr17_random");
  rewind(border);  rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk);
  */
  fprintf(stderr, "chr5\n");
  sprintf(thischr, "chr5");
  rewind(border);  rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk);

  return;

  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(border); rewind(acgh); rewind(suk); 
    dochr(thischr, border, acgh, suk); 
  }


  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(border); rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(border);  rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk);

  fprintf(stderr, "chr17_random\n");
  sprintf(thischr, "chr17_random");
  rewind(border);  rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk);


    


}


void dochr(char *thischr, FILE *border, FILE *acgh, FILE *suk){

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
  int tenpercent;

  memset(seq, -1, sizeof(int)*MAX);
  
  
  while (fscanf(suk, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    seq[s] = 0;
  }
  
  while (fscanf(acgh, "%s\t%d\t%d\t%d\n", chr, &s, &e, &log2) > 0){
    if (strcmp(chr, thischr)) continue;
    seq[s] = log2;
    //for (i=s;i<=e;i++){ if (seq[i]==-1) seq[i] = log2; else seq[i]+=log2;}
  }
  
  
  fprintf(stderr, "aCGH load done, reading windows\n");

  while (fscanf(border, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;

    count=0; sukcount=0;

    for (i=s;i<=e;i++) if (seq[i]!=-1) count++; if (seq[i]==0) sukcount++;

    //    if (count < 10 ) continue;

    thiswin = (int *) malloc(sizeof(int) * count);
    total = 0;
    j=0;
    for (i=s;i<=e;i++){
      //      if (seq[i]!=-1 && seq[i]!=0){
      if (seq[i]!=-1){
	thiswin[j++] = seq[i]; total += seq[i];
      }
    }
    
    qsort(thiswin, count, sizeof (int),  compare);

    /*
    tenpercent = count / 10;
    for (i=count-tenpercent;i<count;i++)
      total-=thiswin[i];
    */
    tenpercent = 0;
    if (count != 0)
      fprintf(dump, "%s\t%d\t%d\t%d\t%d\t%d\t%f\n", chr, s, e, count, sukcount, thiswin[count/2], ((float)total/(float)(count-tenpercent)));
    else 
      fprintf(dump, "%s\t%d\t%d\t%d\t%d\t%d\t%f\n", chr, s, e, count, sukcount, 0, 0.0);
    //else
    // fprintf(dump, "%s\t%d\t%d\t%d\n", chr, s, e, count);

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

