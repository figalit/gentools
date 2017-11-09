#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>


#define MAXCHROMLEN 50
#define MAXLINE 2000

void copy_chrom_names( char **, char **, FILE *);
int findChrom(char *, char **, int);
void print_help(char *);

int main(int argc, char **argv){

  char **ucsc, **grc;
  int i; int o; int index;
  static int ucsctogrc = 1;
  static int grctoucsc = 0;
  char inFileName[MAXCHROMLEN * 3];
  char outFileName[MAXCHROMLEN * 3];
  char conversionFileName[MAXCHROMLEN * 3];
  FILE *inFile, *outFile, *convFile;
  char inChrom[MAXCHROMLEN];
  char line[MAXLINE];

  int MAXCHROM = 0;

  char **from, **to;

  inFileName[0] = 0; outFileName[0] = 0; conversionFileName[0] = 0;

  static struct option long_options[] = 
    {
      {"input"  , required_argument,   0, 'i'},
      {"out"    , required_argument, 0, 'o'},
      {"conversion"    , required_argument, 0, 'c'},
      {"forward", no_argument,    0, 'f'},
      {"reverse", no_argument, 0, 'r'},
      {"help"   , no_argument,         0, 'h'},
      {0        , 0,                   0,  0 }
    };

  if (argc == 1){
    print_help(argv[0]);
    return 0;
  }

  while( ( o = getopt_long( argc, argv, "hfr:i:o:c:", long_options, &index)) != -1){
    switch(o){
    case 'i':
      strcpy(inFileName, optarg);
      break;
    case 'o':
      strcpy(outFileName, optarg);
      break;
    case 'c':
      strcpy(conversionFileName, optarg);
      break;
    case 'r':
      grctoucsc = 1;
      ucsctogrc = 0;
      break;
    case 'f':
      grctoucsc = 0;
      ucsctogrc = 1;
      break;
    case 'h':
      print_help(argv[0]);
      return 0;
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

  if (conversionFileName[0] == 0){
    fprintf (stderr, "Conversion file name (-c) missing.\n");
    exit (-1);
  }

  if (ucsctogrc == 1 && grctoucsc == 1){
    fprintf (stderr, "Select either FIRST to SECOND (-f) or SECOND to FIRST (-r). Not both.\n");
    exit (-1);
  }

  if (ucsctogrc == 0 && grctoucsc == 0){
    fprintf (stderr, "Select either FIRST to SECOND (-f) or SECOND to FIRST (-r).\n");
    exit (-1);
  }

  inFile = fopen(inFileName, "r");
  if (inFile == NULL){
    fprintf (stderr, "Cannot open %s for reading.\n", inFileName);
    exit (-1);
  }

  convFile = fopen(conversionFileName, "r");
  if (convFile == NULL){
    fprintf (stderr, "Cannot open %s for reading.\n", conversionFileName);
    exit (-1);
  }

  outFile = fopen(outFileName, "w");
  if (outFile == NULL){
    fprintf (stderr, "Cannot open %s for writing.\n", outFileName);
    exit (-1);
  }

  while (1){
    if (feof(convFile)) break;
    fgets (line, MAXLINE, convFile);
    if (feof(convFile)) break;
    if (line[0] != '#')
      MAXCHROM++;
  }

  fprintf(stderr, "%d chromosomes.\n", MAXCHROM);
  rewind(convFile);

  ucsc = (char **) malloc (sizeof (char *) * MAXCHROM);
  grc = (char **) malloc (sizeof (char *) * MAXCHROM);
  for (i = 0; i < MAXCHROM; i++){
    ucsc[i] = (char *) malloc (sizeof (char) * MAXCHROMLEN);
    grc[i] = (char *) malloc (sizeof (char) * MAXCHROMLEN);
  }

  i = 0;
  copy_chrom_names(ucsc, grc, convFile);
  fclose(convFile);

  if (ucsctogrc){
    from = ucsc;
    to = grc;
    fprintf(stderr, "Converting from FIRST to SECOND.\n");
  }
  else if (grctoucsc){
    from = grc;
    to = ucsc;
    fprintf(stderr, "Converting from SECOND to FIRST.\n");
  }

  while (fscanf(inFile, "%s", inChrom) > 0){
    fgets(line, MAXLINE, inFile);
    if (i == -1) i = 0;
    if (strcmp(inChrom, from[i]))
      i = findChrom(inChrom, from, MAXCHROM);
    if (i != -1)
      fprintf(outFile, "%s%s", to[i], line);
  }
  
  fclose(inFile); fclose(outFile);
}
 
void copy_chrom_names(char **ucsc, char **grc, FILE *convFile){
  int i = 0;
  char ucscchr[MAXCHROMLEN], grcchr[MAXCHROMLEN];
  char line[MAXLINE];
  while (fscanf(convFile, "%s", ucscchr) > 0){
    if (ucscchr[0]=='#')
      fgets (line, MAXLINE, convFile);
    else{
      fscanf(convFile, "\t%s\n", grcchr);
      /*      fprintf(stderr, "%d %s %s\n", i, ucscchr, grcchr); */
      strcpy(ucsc[i], ucscchr);
      strcpy(grc[i++], grcchr);
    }
  }

}

int findChrom(char *chrom, char **from, int MAXCHROM){
  int i;
  for (i = 0; i < MAXCHROM; i++){
    if (!strcmp (chrom, from[i]))
      return i;
  }
  return -1;
}

void print_help(char *progname){
  fprintf(stderr, "Chromosome name converter by Can Alkan.\n");
  fprintf(stderr, "Renames chromosomes in BED-like files. \n\t(eg. between UCSC-style chromosome names (chr1,chr2,etc.) and GRC-style chromosome names (1,2,...)). \n");
  fprintf(stderr, "\t(Note that for some chromosomes, the name change can be done simply by using sed. But some are more complicated (GL/random chromosomes).) \n\n");
  fprintf(stderr, "Usage: \n");
  fprintf(stderr, "\t%s [options] \n\n", progname);
  fprintf(stderr, "\t -i [input_file]       : Input file to convert. The first column should be chromosome name. The rest doesn't matter.\n");
  fprintf(stderr, "\t -o [output_file]      : Output file name. \n");
  fprintf(stderr, "\t -c [conversion table] : Tab-delimited chromosome name mapping table.\n");
  fprintf(stderr, "\t\tFormat:\n");
  fprintf(stderr, "\t\t#FIRST\tSECOND\n");
  fprintf(stderr, "\t\tchrUn_gl000233\tGL000233.1\n");
  fprintf(stderr, "\t\tchrUn_gl000234\tGL000234.1\n");
  fprintf(stderr, "\t\tSee https://github.com/calkan/gentools/blob/master/hg19_to_b37.tsv as an example.\n");
  fprintf(stderr, "\t -f                    : Rename from FIRST to SECOND (i.e. forward map [default]).\n");
  fprintf(stderr, "\t -r                    : Rename from SECOND to FIRST (i.e. reverse map).\n");
  fprintf(stderr, "\t -h                    : Print this help screen and exit.\n");
  fprintf(stderr, " \n");
}
