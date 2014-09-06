#include <stdio.h>

#define MAX 250000000
#define EXTRACT 1

char seq[MAX];


void dochr(char *thischr, FILE *one);

int third=0;

int totvenn[8];

FILE *dump;

int most;

int main(int argc, char **argv){

  FILE *one;
  int i, j;


  char thischr[100];
  char padfile[100];
  char wssdfile[100];
  char outfile[100];


  if (argc<3) 
    return;

  most = MAX;
  outfile[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      most = atoi(argv[i+1]);
  }

  one = fopen(wssdfile, "r");


  if (one==NULL)
    return;


  if (EXTRACT && outfile[0]!=0)
    dump = fopen(outfile, "w");

  memset(totvenn, 0, sizeof(int)*8);

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

  fprintf(stderr, "chr17_random\n");
  sprintf(thischr, "chr17_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr11_random\n");
  sprintf(thischr, "chr11_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr15_random\n");
  sprintf(thischr, "chr15_random");
  rewind(one);
  dochr(thischr, one);


  fprintf(stderr, "chr19_random\n");
  sprintf(thischr, "chr19_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr21_random\n");
  sprintf(thischr, "chr21_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr22_random\n");
  sprintf(thischr, "chr22_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr3_random\n");
  sprintf(thischr, "chr3_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr7_random\n");
  sprintf(thischr, "chr7_random");
  rewind(one);
  dochr(thischr, one);
    
  fprintf(stderr, "chr8_random\n");
  sprintf(thischr, "chr8_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr9_random\n");
  sprintf(thischr, "chr9_random");
  rewind(one);
  dochr(thischr, one);

  fprintf(stderr, "chr1_random\n");
  sprintf(thischr, "chr1_random");
  rewind(one); 
  dochr(thischr, one);
    
  fprintf(stderr, "chr6_random\n");
  sprintf(thischr, "chr6_random");
  rewind(one); 
  dochr(thischr, one);
    
  fprintf(stderr, "chrX_random\n");
  sprintf(thischr, "chrX_random");
  rewind(one); 
  dochr(thischr, one);
    


}

int findexon(char **therest, char *exon){
  int i=0;
  
  for (i=0;i<23000;i++){
    if (therest[i]==NULL) break;
    if (therest[i] != NULL && !strcmp(therest[i], exon)) return i;
  }

  //printf("new exon %d\t%s", i, exon);
  therest[i]=(char *)malloc(sizeof(char)*1000);
  strcpy(therest[i], exon);
  return i;

}

void dochr(char *thischr, FILE *one){

  int i,j;
  
  char  chr[100];
  int s, e;
  int s2, e2;

  int inone, intwo;
  int venn[8];
  int started=0;
  int interval=0;
  int  lasti;
  

  int state=0;

  char **therest;
  int exonid;
  char thisexon[1000];
  char exon[1000];
  char dummy[1000];
  int maxexon=-1;
  int start;

  memset(seq, -1, sizeof(char)*MAX);
  
  therest = (char **) malloc(sizeof(char *) * 23000);
  for  (i=0;i<23000;i++){
    therest[i]=NULL;
  }

  thisexon[0]=0;
  exonid=-1;

  fgets(dummy, 1000, one); // pass header

  while (fscanf(one, "%s\t%d\t%d\t", chr, &s, &e) > 0){
    fgets(exon, 1000, one);
    if (strcmp(chr, thischr)) continue;
    exonid = findexon(therest, exon);
    for (i=s-199;i<=s;i++)
      seq[i]=exonid;  
    for (i=e;i<=e+199;i++)
      seq[i]=exonid;
    if (exonid>maxexon) maxexon=exonid;
  }
  
  
  s=0;e=0; s2=0;e2=0; start=0; exonid=-1;
  for (i=0;i<MAX-1000;i++){
    switch (state){
    case 0:
      if (seq[i]==-1){
	state=0;
      }
      else if (seq[i]!=-1){
	exonid = seq[i]; state=1;
	s = i;
      }
      break;

    case 1:
      if (seq[i]==-1){
	e=i-1; state=2;
      }
      else if (seq[i]!=-1){
	state=1;
      }
      break;

    case 2:
      if (seq[i]==-1){
	state=2;
      }
      else if (seq[i]!=-1){
	s2=i; state=3;
      }
      break;

    case 3:
      if (seq[i]==-1){
	e2=i-1; state=0;
	if (exonid!=-1 && therest[exonid] != NULL)
	  fprintf(dump, "%s\t%d\t%d\t%d\t%d\t%s", thischr, s, e, s2, e2, therest[exonid]);
      }
      else if (seq[i]!=-1){
	state=3;
      }
      break;

    default: 
      break;

    }


  }

  /*

  for (i=0;i<MAX-1000;i++){

    switch(state){
    case 0:
      if (seq[i]==1){
	s = i; e = i; state=1;
      }
      else if (seq[i]==2)
	state = 2;

      break;      
    case 1:
      if (seq[i]==0){
	fprintf(dump, "%s\t%d\t%d\n", thischr, s, i-1);
	state = 0;
      }
      else if (seq[i]==2){
	e = i-1;
	state = 3;
      }
      break;
      
    case 2:
      if (seq[i] == 0)
	state = 0;
      else if (seq[i] == 1){
	s = i;
	state = 1;
      }
      break;
    case 3:
      if (seq[i] == 0){
	fprintf(dump, "%s\t%d\t%d\n", thischr, s, e);
	state = 0;	
      }
      else if (seq[i] == 1){
	state = 1;
      }
      break;

    default:
      fprintf(stderr, "Error happened\n");
      exit(0);
    }

  }

  */



  for  (i=0;i<23000;i++){
    if (therest[i]!=NULL) free(therest[i]);
  }
  free(therest);

  printf("%s --> %d exons\n", thischr, maxexon);

}
