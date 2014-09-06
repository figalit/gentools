#include <stdio.h>

#define MAX 250000000
#define EXTRACT 0

short seq[MAX];


void dochr(char *thischr, FILE *one, FILE *two, FILE *three, FILE *four);

int third=0;
int fourth=0;

int totvenn[16];

FILE *dump;

int main(int argc, char **argv){

  FILE *one, *two, *three, *four;
  int i, j;


  char thischr[100];

  int inone, intwo, inthree, infour;
  unsigned int iter;
  unsigned int shiftr, shiftl;

  if (argc<3) 
    return;

  one = fopen(argv[1], "r");
  two = fopen(argv[2], "r");


  if (one==NULL || two==NULL)
    return;

  three=fopen(argv[3], "r"); 
  
  
  if (three==NULL) return;
  

  four=fopen(argv[4], "r"); fourth=1;
  
  
  if (four==NULL) return;
  

  if (EXTRACT)
    dump = fopen("common.tab", "w");

  memset(totvenn, 0, sizeof(int)*16);

  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    dochr(thischr, one, two, three, four);
    rewind(one); 
    rewind(two); 
    rewind(three); 
    rewind(four);
  }


  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  dochr(thischr, one, two, three, four);
  rewind(one); rewind(two); rewind(three); rewind(four);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  //rewind(one); rewind(two); rewind(three); rewind(four);
  dochr(thischr, one, two, three, four);
    


  inone=0; intwo=0; inthree=0; infour = 0;
  
  for (iter=1;iter<=15;iter++){
    shiftl = iter << 31;
    shiftr = shiftl >> 31;
    if (shiftr == 1) inone+=totvenn[iter];
  }

  for (iter=1;iter<=15;iter++){
    shiftl = iter << 30;
    shiftr = shiftl >> 31;
    if (shiftr == 1) intwo+=totvenn[iter];
  }

  for (iter=1;iter<=15;iter++){
    shiftl = iter << 29;
    shiftr = shiftl >> 31;
    if (shiftr == 1) inthree+=totvenn[iter];
  }

  for (iter=1;iter<=15;iter++){
    shiftl = iter << 28;
    shiftr = shiftl >> 31;
    if (shiftr == 1) infour+=totvenn[iter];
  }


  fprintf(stdout, "\n\nTOTAL\n\n");

  fprintf(stdout, "\tone: %d", inone);
  fprintf(stdout, "\ttwo: %d", intwo);
  fprintf(stdout, "\tthree: %d", inthree);
  fprintf(stdout, "\tfour: %d", infour);
  
  fprintf(stdout, "\toneonly: %d", totvenn[1]);
  fprintf(stdout, "\ttwoonly: %d", totvenn[2]);
  fprintf(stdout, "\tthreeonly: %d", totvenn[4]);
  fprintf(stdout, "\tfouronly: %d", totvenn[8]);
  
  fprintf(stdout, "\tone-two: %d", totvenn[3]);
  fprintf(stdout, "\tone-three: %d", totvenn[5]);
  fprintf(stdout, "\tone-four: %d", totvenn[9]);

  fprintf(stdout, "\ttwo-three: %d", totvenn[6]);
  fprintf(stdout, "\ttwo-four: %d", totvenn[10]);

  fprintf(stdout, "\tthree-four: %d", totvenn[12]);

  fprintf(stdout, "\tone-two-three: %d", totvenn[7]);
  fprintf(stdout, "\tone-two-four: %d", totvenn[11]);
  fprintf(stdout, "\tone-three-four: %d", totvenn[13]);
  fprintf(stdout, "\ttwo-three-four: %d", totvenn[14]);

  fprintf(stdout, "\tall: %d\n", totvenn[15]);
  



}


void dochr(char *thischr, FILE *one, FILE *two, FILE *three, FILE *four){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo, inthree, infour;
  int venn[16];
  int started=0;
  unsigned int iter;
  unsigned int shiftr, shiftl;

  fprintf(stderr, ">>>%s\n", thischr);
  
  memset(seq, 0, sizeof(short)*MAX);
  
  fprintf(stderr, "read one\n");
  while (fscanf(one, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=1;
  }
  
  fprintf(stderr, "read two\n");
  while (fscanf(two, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=2;
  }
  
  //  if (third){

  fprintf(stderr, "read three\n");

  while (fscanf(three, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=4;
  }

  fprintf(stderr, "read four\n");

  while (fscanf(four, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]+=8;
  }
  
    //}
  
  inone=0; intwo=0; inthree=0;
  memset(venn, 0, sizeof(int)*16);
  
  for (j=0;j<MAX;j++){
    venn[seq[j]]++;
    totvenn[seq[j]]++;
    /*
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
    */
  }

  
  inone=0; intwo=0; inthree=0; infour = 0;
  
  for (iter=1;iter<=15;iter++){
    shiftl = iter << 31;
    shiftr = shiftl >> 31;
    if (shiftr == 1) inone+=venn[iter];
  }

  for (iter=1;iter<=15;iter++){
    shiftl = iter << 30;
    shiftr = shiftl >> 31;
    if (shiftr == 1) intwo+=venn[iter];
  }

  for (iter=1;iter<=15;iter++){
    shiftl = iter << 29;
    shiftr = shiftl >> 31;
    if (shiftr == 1) inthree+=venn[iter];
  }

  for (iter=1;iter<=15;iter++){
    shiftl = iter << 28;
    shiftr = shiftl >> 31;
    if (shiftr == 1) infour+=venn[iter];
  }

  /*
  inone = venn[1] + venn[3] + venn[5] + venn[7] + venn[9] + venn[11] + venn[13] + venn[15];
  intwo = venn[2] + venn[3] + venn[
  */

  fprintf(stdout, "%s", thischr);
  fprintf(stdout, "\tone: %d", inone);
  fprintf(stdout, "\ttwo: %d", intwo);
  fprintf(stdout, "\tthree: %d", inthree);
  fprintf(stdout, "\tfour: %d", infour);
  
  fprintf(stdout, "\toneonly: %d", venn[1]);
  fprintf(stdout, "\ttwoonly: %d", venn[2]);
  fprintf(stdout, "\tthreeonly: %d", venn[4]);
  fprintf(stdout, "\tfouronly: %d", venn[8]);
  
  fprintf(stdout, "\tone-two: %d", venn[3]);
  fprintf(stdout, "\tone-three: %d", venn[5]);
  fprintf(stdout, "\tone-four: %d", venn[9]);

  fprintf(stdout, "\ttwo-three: %d", venn[6]);
  fprintf(stdout, "\ttwo-four: %d", venn[10]);

  fprintf(stdout, "\tthree-four: %d", venn[12]);

  fprintf(stdout, "\tone-two-three: %d", venn[7]);
  fprintf(stdout, "\tone-two-four: %d", venn[11]);
  fprintf(stdout, "\tone-three-four: %d", venn[13]);
  fprintf(stdout, "\ttwo-three-four: %d", venn[14]);

  fprintf(stdout, "\tall: %d\n", venn[15]);
  

}
