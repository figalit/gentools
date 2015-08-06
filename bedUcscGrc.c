#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#define MAXCHROM 84
#define MAXCHROMLEN 40
#define MAXLINE 2000
void copy_chrom_names( char **, char **);
int findChrom(char *, char **);

int main(int argc, char **argv){

  char **ucsc, **grc;
  int i; int o; int index;
  static int ucsctogrc = 1;
  static int grctoucsc = 0;
  char inFileName[MAXCHROMLEN * 3];
  char outFileName[MAXCHROMLEN * 3];
  FILE *inFile, *outFile;
  char inChrom[MAXCHROMLEN];
  char line[MAXLINE];

  char **from, **to;

  inFileName[0] = 0; outFileName[0] = 0;

  static struct option long_options[] = 
    {
      {"input"  , required_argument,   0, 'i'},
      {"out"    , required_argument, 0, 'o'},
      {"ucsctogrc", no_argument,     &ucsctogrc    , 'u'},
      {"grctoucsc", no_argument, &grctoucsc, 'g'},
      {0        , 0,                   0,  0 }
    };

  while( ( o = getopt_long( argc, argv, "ug:i:o:", long_options, &index)) != -1){
    switch(o){
    case 'i':
      strcpy(inFileName, optarg);
      break;
    case 'o':
      strcpy(outFileName, optarg);
      break;
    }
  }

  if (inFileName[0] == 0){
    fprintf (stderr, "Input file name (-i) missing.\n");
    exit (-1);
  }

  if (outFileName[0] == 0){
    fprintf (stderr, "Output file name (-o) missing.\n");
    exit (-1);
  }

  if (ucsctogrc == 1 && grctoucsc == 1){
    fprintf (stderr, "Select either UCSC to GRC (-u) or GRC to UCSC (-g). Not both.\n");
    exit (-1);
  }

  if (ucsctogrc == 0 && grctoucsc == 0){
    fprintf (stderr, "Select either UCSC to GRC (-u) or GRC to UCSC (-g).\n");
    exit (-1);
  }

  inFile = fopen(inFileName, "r");
  if (inFile == NULL){
    fprintf (stderr, "Cannot open %s for reading.\n", inFileName);
    exit (-1);
  }

  outFile = fopen(outFileName, "w");
  if (inFile == NULL){
    fprintf (stderr, "Cannot open %s for writing.\n", outFileName);
    exit (-1);
  }

  ucsc = (char **) malloc (sizeof (char *) * MAXCHROM);
  grc = (char **) malloc (sizeof (char *) * MAXCHROM);
  for (i = 0; i < MAXCHROM; i++){
    ucsc[i] = (char *) malloc (sizeof (char) * MAXCHROMLEN);
    grc[i] = (char *) malloc (sizeof (char) * MAXCHROMLEN);
  }

  i = 0;
  copy_chrom_names(ucsc, grc);
  if (ucsctogrc){
    from = ucsc;
    to = grc;
    fprintf(stderr, "Converting from UCSC to GRC.\n");
  }
  else if (grctoucsc){
    from = grc;
    to = ucsc;
    fprintf(stderr, "Converting from GRC to UCSC.\n");
  }

  while (fscanf(inFile, "%s", inChrom) > 0){
    fgets(line, MAXLINE, inFile);
    if (i == -1) i = 0;
    if (strcmp(inChrom, from[i]))
      i = findChrom(inChrom, from);
    if (i != -1)
      fprintf(outFile, "%s\t%s", to[i], line);
  }
  
  fclose(inFile); fclose(outFile);
}
 
void copy_chrom_names( char **ucsc, char **grc){
  int i = 0;
  strcpy(ucsc[i], "chr1"); strcpy(grc[i++], "1");
  strcpy(ucsc[i], "chr10"); strcpy(grc[i++], "10");
  strcpy(ucsc[i], "chr11"); strcpy(grc[i++], "11");
  strcpy(ucsc[i], "chr12"); strcpy(grc[i++], "12");
  strcpy(ucsc[i], "chr13"); strcpy(grc[i++], "13");
  strcpy(ucsc[i], "chr14"); strcpy(grc[i++], "14");
  strcpy(ucsc[i], "chr15"); strcpy(grc[i++], "15");
  strcpy(ucsc[i], "chr16"); strcpy(grc[i++], "16");
  strcpy(ucsc[i], "chr17"); strcpy(grc[i++], "17");
  strcpy(ucsc[i], "chr18"); strcpy(grc[i++], "18");
  strcpy(ucsc[i], "chr19"); strcpy(grc[i++], "19");
  strcpy(ucsc[i], "chr2"); strcpy(grc[i++], "2");
  strcpy(ucsc[i], "chr20"); strcpy(grc[i++], "20");
  strcpy(ucsc[i], "chr21"); strcpy(grc[i++], "21");
  strcpy(ucsc[i], "chr22"); strcpy(grc[i++], "22");
  strcpy(ucsc[i], "chr3"); strcpy(grc[i++], "3");
  strcpy(ucsc[i], "chr4"); strcpy(grc[i++], "4");
  strcpy(ucsc[i], "chr5"); strcpy(grc[i++], "5");
  strcpy(ucsc[i], "chr6"); strcpy(grc[i++], "6");
  strcpy(ucsc[i], "chr7"); strcpy(grc[i++], "7");
  strcpy(ucsc[i], "chr8"); strcpy(grc[i++], "8");
  strcpy(ucsc[i], "chr9"); strcpy(grc[i++], "9");
  strcpy(ucsc[i], "chrM"); strcpy(grc[i++], "MT");
  strcpy(ucsc[i], "chrX"); strcpy(grc[i++], "X");
  strcpy(ucsc[i], "chrY"); strcpy(grc[i++], "Y");
  strcpy(ucsc[i], "chr1_gl000191_random"); strcpy(grc[i++], "GL000191.1");
  strcpy(ucsc[i], "chr1_gl000192_random"); strcpy(grc[i++], "GL000192.1");
  strcpy(ucsc[i], "chr4_gl000193_random"); strcpy(grc[i++], "GL000193.1");
  strcpy(ucsc[i], "chr4_gl000194_random"); strcpy(grc[i++], "GL000194.1");
  strcpy(ucsc[i], "chr7_gl000195_random"); strcpy(grc[i++], "GL000195.1");
  strcpy(ucsc[i], "chr8_gl000196_random"); strcpy(grc[i++], "GL000196.1");
  strcpy(ucsc[i], "chr8_gl000197_random"); strcpy(grc[i++], "GL000197.1");
  strcpy(ucsc[i], "chr9_gl000198_random"); strcpy(grc[i++], "GL000198.1");
  strcpy(ucsc[i], "chr9_gl000199_random"); strcpy(grc[i++], "GL000199.1");
  strcpy(ucsc[i], "chr9_gl000200_random"); strcpy(grc[i++], "GL000200.1");
  strcpy(ucsc[i], "chr9_gl000201_random"); strcpy(grc[i++], "GL000201.1");
  strcpy(ucsc[i], "chr11_gl000202_random"); strcpy(grc[i++], "GL000202.1");
  strcpy(ucsc[i], "chr17_gl000203_random"); strcpy(grc[i++], "GL000203.1");
  strcpy(ucsc[i], "chr17_gl000204_random"); strcpy(grc[i++], "GL000204.1");
  strcpy(ucsc[i], "chr17_gl000205_random"); strcpy(grc[i++], "GL000205.1");
  strcpy(ucsc[i], "chr17_gl000206_random"); strcpy(grc[i++], "GL000206.1");
  strcpy(ucsc[i], "chr18_gl000207_random"); strcpy(grc[i++], "GL000207.1");
  strcpy(ucsc[i], "chr19_gl000208_random"); strcpy(grc[i++], "GL000208.1");
  strcpy(ucsc[i], "chr19_gl000209_random"); strcpy(grc[i++], "GL000209.1");
  strcpy(ucsc[i], "chr21_gl000210_random"); strcpy(grc[i++], "GL000210.1");
  strcpy(ucsc[i], "chrUn_gl000211"); strcpy(grc[i++], "GL000211.1");
  strcpy(ucsc[i], "chrUn_gl000212"); strcpy(grc[i++], "GL000212.1");
  strcpy(ucsc[i], "chrUn_gl000213"); strcpy(grc[i++], "GL000213.1");
  strcpy(ucsc[i], "chrUn_gl000214"); strcpy(grc[i++], "GL000214.1");
  strcpy(ucsc[i], "chrUn_gl000215"); strcpy(grc[i++], "GL000215.1");
  strcpy(ucsc[i], "chrUn_gl000216"); strcpy(grc[i++], "GL000216.1");
  strcpy(ucsc[i], "chrUn_gl000217"); strcpy(grc[i++], "GL000217.1");
  strcpy(ucsc[i], "chrUn_gl000218"); strcpy(grc[i++], "GL000218.1");
  strcpy(ucsc[i], "chrUn_gl000219"); strcpy(grc[i++], "GL000219.1");
  strcpy(ucsc[i], "chrUn_gl000220"); strcpy(grc[i++], "GL000220.1");
  strcpy(ucsc[i], "chrUn_gl000221"); strcpy(grc[i++], "GL000221.1");
  strcpy(ucsc[i], "chrUn_gl000222"); strcpy(grc[i++], "GL000222.1");
  strcpy(ucsc[i], "chrUn_gl000223"); strcpy(grc[i++], "GL000223.1");
  strcpy(ucsc[i], "chrUn_gl000224"); strcpy(grc[i++], "GL000224.1");
  strcpy(ucsc[i], "chrUn_gl000225"); strcpy(grc[i++], "GL000225.1");
  strcpy(ucsc[i], "chrUn_gl000226"); strcpy(grc[i++], "GL000226.1");
  strcpy(ucsc[i], "chrUn_gl000227"); strcpy(grc[i++], "GL000227.1");
  strcpy(ucsc[i], "chrUn_gl000228"); strcpy(grc[i++], "GL000228.1");
  strcpy(ucsc[i], "chrUn_gl000229"); strcpy(grc[i++], "GL000229.1");
  strcpy(ucsc[i], "chrUn_gl000230"); strcpy(grc[i++], "GL000230.1");
  strcpy(ucsc[i], "chrUn_gl000231"); strcpy(grc[i++], "GL000231.1");
  strcpy(ucsc[i], "chrUn_gl000232"); strcpy(grc[i++], "GL000232.1");
  strcpy(ucsc[i], "chrUn_gl000233"); strcpy(grc[i++], "GL000233.1");
  strcpy(ucsc[i], "chrUn_gl000234"); strcpy(grc[i++], "GL000234.1");
  strcpy(ucsc[i], "chrUn_gl000235"); strcpy(grc[i++], "GL000235.1");
  strcpy(ucsc[i], "chrUn_gl000236"); strcpy(grc[i++], "GL000236.1");
  strcpy(ucsc[i], "chrUn_gl000237"); strcpy(grc[i++], "GL000237.1");
  strcpy(ucsc[i], "chrUn_gl000238"); strcpy(grc[i++], "GL000238.1");
  strcpy(ucsc[i], "chrUn_gl000239"); strcpy(grc[i++], "GL000239.1");
  strcpy(ucsc[i], "chrUn_gl000240"); strcpy(grc[i++], "GL000240.1");
  strcpy(ucsc[i], "chrUn_gl000241"); strcpy(grc[i++], "GL000241.1");
  strcpy(ucsc[i], "chrUn_gl000242"); strcpy(grc[i++], "GL000242.1");
  strcpy(ucsc[i], "chrUn_gl000243"); strcpy(grc[i++], "GL000243.1");
  strcpy(ucsc[i], "chrUn_gl000244"); strcpy(grc[i++], "GL000244.1");
  strcpy(ucsc[i], "chrUn_gl000245"); strcpy(grc[i++], "GL000245.1");
  strcpy(ucsc[i], "chrUn_gl000246"); strcpy(grc[i++], "GL000246.1");
  strcpy(ucsc[i], "chrUn_gl000247"); strcpy(grc[i++], "GL000247.1");
  strcpy(ucsc[i], "chrUn_gl000248"); strcpy(grc[i++], "GL000248.1");
  strcpy(ucsc[i], "chrUn_gl000249"); strcpy(grc[i++], "GL000249.1");
}

int findChrom(char *chrom, char **from){
  int i;
  for (i = 0; i < MAXCHROM; i++){
    if (!strcmp (chrom, from[i]))
      return i;
  }
  return -1;
}
