#include <stdio.h>

#define MAXCHR 250000000

int numbers[24][MAXCHR];

int main(int argc, char **argv){
  char chr[30];
  char chr2[30];
  int s,e,c;
  int i,j;
  int width;
  int winsize=500;
  int slide=100;
  long line;
  int maxe=0;

  char fname[100];
  char winname5k[100];
  char winname1k[100];
  char winnamecn[100];

  char winname5kout[100];
  char winname1kout[100];
  char winnamecnout[100];

  char seqname[100]; int flag;
  char rest[1000];

  FILE *in, *out;
  int chrom;

  
  //  for (i=0;i<MAXCHR;i++)
  //numbers[i] = 0;

  /*
  for (i=1;i<argc;i++){
    
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-w"))
      strcpy(winname, argv[i+1]);
  }
  */

  //  if (strcmp(fname, "stdin"))
  //    in = fopen(fname, "r");
  //  else
    
  if (argc != 7) {
    printf("%s\t5Kwinborders 1Kwinborders CNwinborders 5Kout 1Kout CNout.\n");
    return 0;
  }
  
  strcpy(winname5k, argv[1]);
  strcpy(winname1k, argv[2]);
  strcpy(winnamecn, argv[3]);

  strcpy(winname5kout, argv[4]);
  strcpy(winname1kout, argv[5]);
  strcpy(winnamecnout, argv[6]);


  in = stdin;

  for (i=0;i<24;i++)
    memset(numbers[i], 0, sizeof(int)*MAXCHR);

  line=0;
  while (fscanf(in, "%s\t%d\t%s\t%d\n", seqname, flag, chr, &s)>0){
    fgets(rest, 1000, in);
    if (s>=MAXCHR) continue;
    if (strstr(chr, "random") || strstr(chr, "chrM")) continue;
    line++;
    if (!strcmp(chr, "chrX")) chrom=23;
    else if (!strcmp(chr, "chrY")) chrom=24;
    else chrom = atoi(chr+3);

    if (chrom==0) continue;
    
    s--;

    numbers[chrom-1][s]++;

  }


  fclose(in);

  out = fopen(winname5kout, "w");

  for (i=0;i<24;i++){

    in = fopen(winname5k, "r");

    if (i==22)  strcpy(chr, "chrX");
    else if (i==23)  strcpy(chr, "chrY");
    else sprintf(chr, "chr%d", (i+1));
    
    while (fscanf(in, "%s %d %d\n", chr2, &s, &e)>0){
      
      if (strcmp(chr, chr2)) continue;
      
      width=0;
      
      for (j=s;j<e;j++){
	width+=numbers[i][j];
      }
      //printf("j:%d\n", j);                                                                                                                                                                                                                   
      //if (width!=0)
      fprintf(out, "%s\t%d\t%d\t%d\n", chr, s, e, width);
    }

    fclose(in); 

  }

  fclose(out);

  out = fopen(winname1kout, "w");

  for (i=0;i<24;i++){

    in = fopen(winname1k, "r");

    if (i==22)  strcpy(chr, "chrX");
    else if (i==23)  strcpy(chr, "chrY");
    else sprintf(chr, "chr%d", (i+1));
    
    while (fscanf(in, "%s %d %d\n", chr2, &s, &e)>0){
      
      if (strcmp(chr, chr2)) continue;
      
      width=0;
      
      for (j=s;j<e;j++){
	width+=numbers[i][j];
      }
      //printf("j:%d\n", j);                                                                                                                                                                                                                   
      //if (width!=0)
      fprintf(out, "%s\t%d\t%d\t%d\n", chr, s, e, width);
    }

    fclose(in); 

  }

  fclose(out);
  out = fopen(winnamecnout, "w");
    
  for (i=0;i<24;i++){

    in = fopen(winnamecn, "r");

    if (i==22)  strcpy(chr, "chrX");
    else if (i==23)  strcpy(chr, "chrY");
    else sprintf(chr, "chr%d", (i+1));
    
    while (fscanf(in, "%s %d %d\n", chr2, &s, &e)>0){
      
      if (strcmp(chr, chr2)) continue;
      
      width=0;
      
      for (j=s;j<e;j++){
	width+=numbers[i][j];
      }
      //printf("j:%d\n", j);                                                                                                                                                                                                                   
      //if (width!=0)
      fprintf(out, "%s\t%d\t%d\t%d\n", chr, s, e, width);
    }

    fclose(in); 

  }

  fclose(out);

}
