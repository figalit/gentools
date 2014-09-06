/* loads a pileup file and a bed file to count SNVs */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX 500000
#define CHROMLEN 250000000

unsigned char A[CHROMLEN];
unsigned char C[CHROMLEN];
unsigned char G[CHROMLEN];
unsigned char T[CHROMLEN];

void do_chrom(int, FILE *, FILE *);

int main(int argc, char **argv){
  char bedfile[100];
  char pileupfile[100];
  FILE *pu, *bed;
  int i;

  bedfile[0]=0; pileupfile[0]=0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-pu"))
      strcpy(pileupfile, argv[i+1]);
    else if (!strcmp(argv[i], "-bed"))
      strcpy(bedfile, argv[i+1]);
  }

  if (pileupfile[0]==0 || bedfile[0]==0){
    fprintf(stderr, "-pu and/or -bed is/are missing.\n");
    return -1;
  }
  
  pu = fopen(pileupfile, "r");
  bed = fopen(bedfile, "r");

  if (pu==NULL || bed==NULL){
    fprintf(stderr, "-pu and/or -bed file(s) is/are not found.\n");
    return -1;
  }

  //  for (i=1; i<=24; i++){
    do_chrom(i, pu, bed);
    //  rewind(pu); rewind(bed);
    //  }
}


void do_chrom(int chromid, FILE *pu, FILE *bed){
  int i; int j;
  char chrom[100]; int loc; 
  char refch[2]; int cnt;
  char seq[MAX]; 
  char qual[MAX];
  char thischrom[100];
  char bedchrom[100];
  int num_indel;
  int s, e;
  int a,c,g,t;
  
  /*
  if (chromid==23) strcpy(thischrom, "chrX");
  else if (chromid==24) strcpy(thischrom, "chrY");
  else  sprintf(thischrom, "chr%d", chromid);
  */

  memset(A, 0, sizeof(unsigned char) * CHROMLEN);
  memset(C, 0, sizeof(unsigned char) * CHROMLEN);
  memset(G, 0, sizeof(unsigned char) * CHROMLEN);
  memset(T, 0, sizeof(unsigned char) * CHROMLEN);


  thischrom[0] = 0;
  while (fscanf(pu, "%s\t%d\t%s\t%d\t%s\t%s\n", chrom, &loc, refch, &cnt, seq, qual) > 0){
    
    if (thischrom[0] == 0){
      strcpy(thischrom, chrom);
      fprintf(stderr, "new chr: %s\n", thischrom);
    }
    else if (strcmp(chrom, thischrom)){
      rewind(bed);

      fprintf(stderr, "chrom %s is finished, reading bed.\n", thischrom);

      while(fscanf(bed, "%s\t%d\t%d\n", bedchrom, &s, &e) > 0){
	if (strcmp(bedchrom, thischrom))
	  continue;
	cnt = 0;
	for (i=s;i<e;i++){
	  cnt += A[i]+C[i]+G[i]+T[i];
	}
	fprintf(stdout, "%s\t%d\t%d\t%d\n", bedchrom, s, e, cnt);
      }
      
      strcpy(thischrom, chrom);
      memset(A, 0, sizeof(unsigned char) * CHROMLEN);
      memset(C, 0, sizeof(unsigned char) * CHROMLEN);
      memset(G, 0, sizeof(unsigned char) * CHROMLEN);
      memset(T, 0, sizeof(unsigned char) * CHROMLEN);

      fprintf(stderr, "new chr: %s\n", thischrom);
    }



    i=0; j=-1;

    a = 0; c = 0; g = 0 ; t = 0;
    while (i<strlen(seq)){

      if (seq[i]=='.' || seq[i]==',') {i++;  j++;} // ref base
      else if (seq[i]=='$') i++; // read end
      else if (seq[i]=='^') {i+=2;  j++;}// map qual
      else if (seq[i]=='+' || seq[i]=='-'){
	num_indel = seq[++i] - '0';
	i++; //  num_indel too
	i+=num_indel+1;
	j++;
      }
      else if (qual[j]-33 >= 20){
	
	if (toupper(seq[i])=='A') a++;
	else if (toupper(seq[i])=='C') c++;
	else if (toupper(seq[i])=='G') g++;
	else if (toupper(seq[i])=='T') t++;
	//printf("here:%d: %s %c %d %d %d %d\n", loc, refch, seq[i], a, c, g, t);
	i++; j++;
      }
      else {i++; j++;}
    }
    if (a>=2) A[loc-1]++;
    if (c>=2) C[loc-1]++;
    if (g>=2) G[loc-1]++;
    if (t>=2) T[loc-1]++;
    //printf("cnt: %d\n", A[loc-1]+C[loc-1]+G[loc-1]+T[loc-1]);
  }

  /* last chrom */

  fprintf(stderr, "chrom %s is finished, reading bed.\n", thischrom);
  while(fscanf(bed, "%s\t%d\t%d\n", bedchrom, &s, &e) > 0){
    if (strcmp(bedchrom, thischrom))
      continue;
    cnt = 0;
    for (i=s;i<e;i++){
      cnt += A[i]+C[i]+G[i]+T[i];
    }
    fprintf(stdout, "%s\t%d\t%d\t%d\n", bedchrom, s, e, cnt);
  }
}
