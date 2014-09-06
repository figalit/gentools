#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define NUM_SEQS 108
#define LEN 93399

int coords[NUM_SEQS][3];
main(){

  int i,j,k,l;
  char ch;
  FILE *locs;
  FILE *sequence;
  FILE *out=NULL;
  char fname[100];
  char loc1[8];
  char loc2[8];
  int isComplement;
  int checkpoint;
  int checkpoint2;
  char *Seq;
  char *genome;
  int genLength;


  locs = fopen("colIb.coords", "r");
  i=0;
  while(fscanf(locs,"%s %s %d", loc1, loc2, &isComplement) > 0){
    coords[i][0]=atoi(loc1);
    coords[i][1]=atoi(loc2);
    coords[i++][2]=isComplement;
  }
  fclose(locs);
  //for(i=0;i<141;i++)
  //printf("%d %d\n", coords[0][i],coords[1][i]);
  i=0;
  j=0;
  k=1;
  sequence=fopen("colIb.fa","r");
  fseek(sequence, 0,SEEK_END);
  genLength=ftell(sequence)+2;
  rewind(sequence);
  genome=(char *)malloc(genLength+1);
  fgets(genome, genLength+1, sequence);
  for (i=0;i<NUM_SEQS;i++){
    checkpoint = coords[i][0]-250;
    checkpoint2 = coords[i][1]+25;
    printf("%d %d \n", coords[i][0], coords[i][1]);

    if (checkpoint < 1)
      checkpoint = 1;
    /*
    if (i!=0){
      if (checkpoint < coords[1][i-1])
	checkpoint = coords[1][i-1]+1;
      /*if (checkpoint2 > coords[0][i+1])
	checkpoint2 = coords[0][i+1]-1;
    }*/
    if (checkpoint2 > LEN)
      checkpoint2 = LEN;

    j=checkpoint;
    /*
    for(j=1;j<checkpoint;j++)
      fscanf(sequence, "%c", &ch);
    */
    sprintf(fname,"ColIb.mRNA.extended.%d.fa", k);
    k++;
    out = fopen(fname, "w");
    fprintf(out, "%d %d %d %d\n", coords[i][0], coords[i][1], (coords[i][0]-checkpoint), (checkpoint2-coords[i][1]));
    
    if (!coords[i][2]){
      for(;j<=checkpoint2;j++){
	//fscanf(sequence, "%c", &ch);   
	ch=genome[j-1];
	fprintf(out, "%c", ch);
      }
    }
    else{
      l=0;
      Seq = (char *)malloc(checkpoint2-checkpoint+2);
      for(;j<=checkpoint2;j++){
	//fscanf(sequence, "%c", &ch);   
	ch=genome[j-1];
	Seq[l++]=ch;
      }
      Seq[l]=0;
      for (l=strlen(Seq)-1;l>=0;l--){
	if (Seq[l]=='A') fprintf(out, "U");
	else if (Seq[l]=='U') fprintf(out, "A");
	else if (Seq[l]=='G') fprintf(out, "C");
	else if (Seq[l]=='C') fprintf(out, "G");
      }
      free(Seq);
    }
    fclose(out);
  }

  /*
  while(fscanf(sequence, "%c", &ch) > 0){
    j++;
    if (j >= coords[0][i] && j<= coords[1][i]){
      if (j==coords[0][i]){
	sprintf(fname,"R1.mRNA.%d.fa", k);
	k++;
	out = fopen(fname, "w");
      }
      fprintf(out, "%c", ch);
      fflush(out);
      if(j==coords[1][i]){
	fclose(out);
	i++;
      }
    }
  }
  */
}
