#include <stdio.h>

#define MAX 300000000
#define EXTRACT 1

short seq[MAX];


void dochr(char *thischr, FILE *one, FILE *two, FILE *three);

int third=0;

int totvenn[8];

FILE *dump;

int main(int argc, char **argv){

  FILE *one, *two, *three;
  int i, j;


  char thischr[100];


  if (argc<3) 
    return;

  one = fopen(argv[1], "r");
  two = fopen(argv[2], "r");


  if (one==NULL || two==NULL)
    return;

  if (argc==4){
    three=fopen(argv[3], "r"); third=1;
    if (three==NULL) return;
  }

  if (EXTRACT)
    dump = fopen("common.tab", "w");

  memset(totvenn, 0, sizeof(int)*8);

  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(one); rewind(two); if (third) rewind(three);
    dochr(thischr, one, two, three);
  }


  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(one); rewind(two); if (third) rewind(three);
  dochr(thischr, one, two, three);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(one); rewind(two); if (third) rewind(three);
  dochr(thischr, one, two, three);
    

  fprintf(stdout, "\n\nTOTAL\n\n");
  fprintf(stdout, "\tone: %d", totvenn[1]+totvenn[3]+totvenn[5]+totvenn[7]);
  fprintf(stdout, "\ttwo: %d", totvenn[2]+totvenn[3]+totvenn[6]+totvenn[7]);
  if (third)
    fprintf(stdout, "\tthree: %d", totvenn[4]+totvenn[5]+totvenn[6]+totvenn[7]);
  
  fprintf(stdout, "\toneonly: %d", totvenn[1]);
  fprintf(stdout, "\ttwoonly: %d", totvenn[2]);
  if (third)
    fprintf(stdout, "\tthreeonly: %d", totvenn[4]);
  
  fprintf(stdout, "\tone-two: %d", totvenn[3]);
  if (third){
    fprintf(stdout, "\tone-three: %d", totvenn[5]);
    fprintf(stdout, "\ttwo-three: %d", totvenn[6]);
    fprintf(stdout, "\tall: %d\n", totvenn[7]);
  }
  else
    fprintf(stdout, "\n");

  



}


void dochr(char *thischr, FILE *one, FILE *two, FILE *three){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo, inthree;
  int venn[8];
  int started=0;
  

  fprintf(stderr, ">>>%s\n", thischr);
  
  memset(seq, 0, sizeof(short)*MAX);
  
  
  while (fscanf(one, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=1;
  }
  
  while (fscanf(two, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=2;
  }
  
  if (third){
    while (fscanf(three, "%s\t%d\t%d\n", chr, &s, &e) > 0){
      if (strcmp(chr, thischr)) continue;
      for (i=s;i<=e;i++) seq[i]+=4;
    }
    
  }
  
  inone=0; intwo=0; inthree=0;
  memset(venn, 0, sizeof(int)*8);
  
  for (j=0;j<MAX;j++){
    venn[seq[j]]++;
    totvenn[seq[j]]++;
    switch(seq[j]){  
    case 1: inone++;  break;	
    case 2: intwo++;  break;	
    case 3: inone++; intwo++; break;	
    case 4: inthree++; break;	
    case 5: inone++; inthree++; break;	
    case 6: inthree++; intwo++; break;	
    case 7: inone++; inthree++; intwo++; break;	
    default: 
      break;	
    }
  }

  if (EXTRACT){
    for (i=0;i<MAX-2000;i++){
      //if (seq[i]>=3 && !started){
      if (seq[i]==7 && !started){
	fprintf(dump, "%s\t%d\t", thischr, i);
	fflush(dump);
	started=1;
      }
      //      else if (started && seq[i]<3 && seq[i+2000]<3){
      else if (started && seq[i]<7){ // && seq[i+2000]<7){
	started=0;
	fprintf(dump, "%d\n", i);
      }
    }
  }

  
  fprintf(stdout, "%s", thischr);
  fprintf(stdout, "\tone: %d", inone);
  fprintf(stdout, "\ttwo: %d", intwo);
  if (third)
    fprintf(stdout, "\tthree: %d", inthree);
  
  fprintf(stdout, "\toneonly: %d", venn[1]);
  fprintf(stdout, "\ttwoonly: %d", venn[2]);
  if (third)
    fprintf(stdout, "\tthreeonly: %d", venn[4]);
  
  fprintf(stdout, "\tone-two: %d", venn[3]);
  if (third){
    fprintf(stdout, "\tone-three: %d", venn[5]);
    fprintf(stdout, "\ttwo-three: %d", venn[6]);
    fprintf(stdout, "\tall: %d\n", venn[7]);
  }
  else
    fprintf(stdout, "\n");
  

}
