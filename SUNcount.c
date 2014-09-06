#include <stdio.h>
#include <stdlib.h>

#define MAX 250000000
#define EXTRACT 1

int seq[MAX];
char sun[MAX];

static int compare(const void *p1, const void *p2);

void dochr(char *thischr, FILE *border, FILE *acgh, FILE *suk, FILE *sundepth);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *border, *acgh, *sundepth;
  FILE *suk;
  int i, j;


  char thischr[100];
  char acghfile[100];
  char wssdfile[100];
  char outfile[100];
  char sukfile[100];
  char sundepthfile[100];

  if (argc<3) 
    return;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(acghfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-os"))
      strcpy(sundepthfile, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      strcpy(sukfile, argv[i+1]);
  }

  border = fopen(wssdfile, "r");
  acgh = fopen(acghfile, "r");
  suk = fopen(sukfile, "r");
  sundepth = fopen(sundepthfile, "w");


  if (border==NULL || acgh==NULL || suk==NULL || sundepth==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");

  memset(totvenn, 0, sizeof(int)*8);


  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(border); rewind(acgh); rewind(suk); 
    dochr(thischr, border, acgh, suk, sundepth); 
  }


  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(border); rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk, sundepth);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(border);  rewind(acgh); rewind(suk); 
  dochr(thischr, border, acgh, suk, sundepth);


    


}


void dochr(char *thischr, FILE *border, FILE *acgh, FILE *suk, FILE *sundepth){

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


  memset(seq, 0, sizeof(int)*MAX);
  memset(sun, 0, sizeof(char)*MAX);
  
  
  while (fscanf(suk, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<e;i++) sun[s] = 1;
  }
  
  while (fscanf(acgh, "%s\t%d\t%d\t%d\n", chr, &s, &e, &log2) > 0){
    if (strcmp(chr, thischr)) continue;
    //seq[s] = log2;
    for (i=s;i<=e;i++){ seq[i]+=log2;}
  }
  

  
  fprintf(stderr, "aCGH load done, reading windows\n");

  while (fscanf(border, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;

    count=0; sukcount=0;

    for (i=s;i<=e;i++) if (sun[i]!=0) count++; //if (seq[i]==0) sukcount++;

    //    if (count < 10 ) continue;

    thiswin = (int *) malloc(sizeof(int) * count);
    total = 0;
    j=0;
    for (i=s;i<=e;i++){
      //      if (seq[i]!=-1 && seq[i]!=0){
      if (sun[i]!=0){
	thiswin[j++] = seq[i]; total += seq[i];
	if (seq[i]!=0) sukcount++;
	fprintf(sundepth, "%s\t%d\t%d\t%d\n", chr, i, i+1, seq[i]);
      }
    }
    
    qsort(thiswin, count, sizeof (int),  compare);
    
    if (count != 0)
      fprintf(dump, "%s\t%d\t%d\t%d\t%d\t%d\t%f\n", chr, s, e, count, sukcount, thiswin[count/2], ((float)total/(float)count));
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

