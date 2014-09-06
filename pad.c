#include <stdio.h>

#define MAX 250000000
#define EXTRACT 1

#define MAXCHAIN 600000

char seq[MAX];


void dochr(char *thischr, FILE *one, FILE *two);

int third=0;

int totvenn[8];

FILE *dump;

int most;
int size=10000;

FILE *flog;

int main(int argc, char **argv){

  FILE *one, *two;
  int i, j;


  char thischr[100];
  char padfile[100];
  char wssdfile[100];
  char outfile[100];
  char logfile[100];


  if (argc<3) 
    return;

  most = MAX;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-w"))
      strcpy(wssdfile, argv[i+1]);
    else if (!strcmp(argv[i], "-p"))
      strcpy(padfile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      most = atoi(argv[i+1]);
  }

  one = fopen(wssdfile, "r");
  two = fopen(padfile, "r");
  sprintf(logfile, "%s.padlog", wssdfile);
  flog = fopen(logfile, "w");

  if (one==NULL || two==NULL)
    return;


  if (EXTRACT)
    dump = fopen(outfile, "w");

  memset(totvenn, 0, sizeof(int)*8);

  for (i=1;i<=22;i++){

    fprintf(stderr, "chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    fprintf(stderr, "-->%s\n", thischr);
    rewind(one); rewind(two); 
    dochr(thischr, one, two);
  }


  fprintf(stderr, "chrX\n");
  sprintf(thischr, "chrX");
  rewind(one); rewind(two); 
  dochr(thischr, one, two);
    
  fprintf(stderr, "chrY\n");
  sprintf(thischr, "chrY");
  rewind(one); rewind(two); 
  dochr(thischr, one, two);
    
  /*
  fprintf(stdout, "\n\nTOTAL\n\n");
  fprintf(stdout, "\tone: %d", totvenn[1]+totvenn[3]+totvenn[5]+totvenn[7]);
  fprintf(stdout, "\ttwo: %d", totvenn[2]+totvenn[3]+totvenn[6]+totvenn[7]);
  
  fprintf(stdout, "\toneonly: %d", totvenn[1]);
  fprintf(stdout, "\ttwoonly: %d", totvenn[2]);
  
  fprintf(stdout, "\tone-two: %d", totvenn[3]);

  fprintf(stdout, "\n");

  */



}


void dochr(char *thischr, FILE *one, FILE *two){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inone, intwo;
  int venn[8];
  int started=0;
  int interval=0;
  int  lasti;
  
  int padlen;
  int padend;

  int padding=0;

  int state=0;


  memset(seq, 0, sizeof(char)*MAX);
  

  while (fscanf(two, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    if (e-s+1 > most) continue;
    for (i=s;i<=e;i++){
      /*      if (!strcmp(chr, "chr2") && i==87567498)
	      printf("HERE: %d\t%d\n", s, e);*/
      seq[i]=2;
    }
  }

  
  while (fscanf(one, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    for (i=s;i<=e;i++) seq[i]=1;
  }
  



  //  if (!strcmp(thischr, "chr2")) printf("CHECK: %d\n", seq[87567498]);
  


  
  state = 0;

  for (i=0;i<MAX-1000;i++){

    switch(state){
    case 0:
      padlen=-1;
      padding=0;
      if (seq[i]==1){
	s = i; e = i; state=1;
      }
      else if (seq[i]==2)
	state = 2;

      break;      
    case 1:
      if (seq[i]==0){
	if (!padding && (i-1-s >= size))
	  fprintf(dump, "%s\t%d\t%d\n", thischr, s, i-1);
	
	else{
	  fprintf(flog, "PADLEN: %d\n", padlen);
	  if (padlen <= MAXCHAIN){
	    fprintf(flog, "Padding %s\t%d\t%d\tLEN:\t%d\n", thischr, e, padend, padlen);
	    if (i-1-s >= size)
	      fprintf(dump, "%s\t%d\t%d\n", thischr, s, i-1);
	  }
	  else{
	    fprintf(flog, "NOT-Padding %s\t%d\t%d\tLEN:\t%d\n", thischr, e, padend, padlen);
	    if (e-s >= size)
	      fprintf(dump, "%s\t%d\t%d\n", thischr, s, e);
	    if (i-1 - padend -1 >= size)
	      fprintf(dump, "%s\t%d\t%d\n", thischr, padend+1, i-1);
	  }
	}
	state = 0;
      }
      else if (seq[i]==2){
 	e = i-1;
	fprintf(flog, "Enter State3: %s\t%d\t%d\n", thischr, s, e);
	state = 3; padding=1;
      }
      break;
      
    case 2:
      if (seq[i] == 0)
	state = 0; 
      else if (seq[i] == 1){
	s = i; e = i;
	state = 1; padding=0;
      }
      break;
    case 3:
      if (seq[i] == 0){
	fprintf(flog, "Pad break: %s\t%d\t%d\t PRINTING %d\t%d\n", thischr, s, i, s, e);
	if (e-s >= size)
	  fprintf(dump, "%s\t%d\t%d\n", thischr, s, e);
	state = 0;  padding=0;
      }
      else if (seq[i] == 1){
	padend=i-1;	padlen = padend-e;
	state = 1;
	fprintf(flog, "Exit State3: %s\t%d\te: %d\tpadlen:%d\n", thischr, i, e, padlen);
	if (padlen > MAXCHAIN){
	  fprintf(flog, "NOT-Padding %s\t%d\t%d\tLEN:\t%d\n", thischr, e, padend, padlen);
	  if (e-s >= size)
	    fprintf(dump, "%s\t%d\t%d\n", thischr, s, e);
	  s=i; e=i;
	}
      }
      break;

    default:
      fprintf(stderr, "Error happened\n");
      exit(0);
    }

  }


}
