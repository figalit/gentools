#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 250000000

#define MAXGENE 40000

unsigned int seq[MAX];

unsigned short genes[MAX];

static int compare(const void *p1, const void *p2);

typedef struct gene{
  char chr[20];
  int s, e;
  char *name;
}_gene;



void dochr(char *thischr, FILE **fp, int fileStart, int fileEnd);

int third=0;


FILE *dump;

FILE **fp;

FILE *genefile;

int genecount = 0;

int baseone;

int main(int argc, char **argv){

  FILE *acgh;
  
  int i, j;


  char thischr[100];
  char acghfile[100];
  char wssdfile[100];
  char outfile[100];

  int fileStart;
  int fileEnd;
  int fileCount;

  char chr[100]; int s, e;
  char rest[10000]; int counttab;
  char genefname[100];

  char gname[200];


  if (argc<3){
    printf("BED file breaker by Can Alkan.\n\nVersion June 17, 2010.\n\n");
    printf("%s [parameters]\n", argv[0]);
    printf("\t-a [input BED files]:\tList of BED files that will be used for breaking intervals.\t\t\t<mandatory>\n");
    printf("\t-o [output file]:\tOutput file.\t\t\t\t\t\t\t\t<mandatory>\n");
    printf("\nAll options except -g are mandatory!\nInput files (-a) should be BED3.\nAll tab-delimited text files.\n");
    printf("\nExample:\n\t%s -a wgac.bed wssd.bed repeatmasker.bed -o microhotspots.annotated_output.bed\n", argv[0]);
    printf("\n\nThis version supports only chr1-chr22-chrX-chrY. _random chromosomes, and chromosome names for other species (chr2a, chr2b, etc.) will be added later.\n");
    return;
  }

  i=1;

  genefile = NULL;
  genefname[0]=0;
  fileEnd = 0; fileStart = 1;

  while (i<argc){
    if (!strcmp(argv[i], "-a")){
      fileStart=i+1; fileEnd=i+1; i++;
      while (i<argc && argv[i][0]!='-') fileEnd=i++;
    }
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[++i]);
    else i++;
  }

  
  fileCount = fileEnd-fileStart+1;

  if (fileCount > 32){
    fprintf(stderr, "Too many files (%d) for annotation. Make it less than 32.\n", fileCount);
    return 0;
  }

  if (fileCount <=0 ){
    fprintf(stderr, "No files given for annotation. Make it more than 0.\n");
    return 0;
  }

  printf("# of files: %d\n", fileCount);



  fp = (FILE **) malloc(sizeof(FILE *) * fileCount);

  for (i=0;i<fileCount;i++){
    printf("File %d: %s\n", i, argv[i+fileStart]);
    fp[i] = fopen(argv[i+fileStart], "r");
    if (fp[i] == NULL){
      fprintf(stderr, "Unable to open annotation file %s\n", argv[i+fileStart]);
      return 0;
    }
  }

  dump = fopen(outfile, "w");

  if (dump==NULL){
    fprintf(stderr, "Unable to open output file %s\n", outfile);
    return 0;
  }


  fprintf(dump, "chrom\tstart\tend");


  fprintf(dump, "\n");


  for (i=1;i<=22;i++){

    fprintf(stderr, "Annotating chr%d\n", i);
    sprintf(thischr, "chr%d", i);
    for (j=0;j<fileCount;j++)  rewind(fp[j]); 
    dochr(thischr, fp, fileStart, fileEnd);
  }


  fprintf(stderr, "Annotating chrX\n");
  sprintf(thischr, "chrX");
  for (j=0;j<fileCount;j++)  rewind(fp[j]); 
  dochr(thischr,  fp, fileStart, fileEnd);
    
  fprintf(stderr, "Annotating chrY\n");
  sprintf(thischr, "chrY");
  for (j=0;j<fileCount;j++)  rewind(fp[j]); 
  dochr(thischr,  fp, fileStart, fileEnd);

  


}


void dochr(char *thischr, FILE **fp, int fileStart, int fileEnd){

  int i,j;
  
  char  chr[100];
  int s, e;

  int inborder, inacgh;
  int started=0;
  int interval=0;
  int  lasti;
  float log2;
  float *thiswin;
  int count;
  float mean;
  float total;

  unsigned int marker;
  int fileCount = fileEnd - fileStart + 1;
  char rest[10000];

  char genestring[10000];
  int lastgene;
  int curmark=0;
  int lastend=-1;

  memset(seq, 0, sizeof(unsigned int)*MAX);


  
  for (j=0; j<fileCount; j++){
    //marker = (unsigned int) pow (2, j);
    marker = 1;
    marker = marker << j;
    while (fscanf(fp[j], "%s%d%d\n", chr, &s, &e) > 0){
      if (strcmp(chr, thischr)) continue;
      for (i=s; i<=e; i++) {
	seq[i] = seq[i] | marker;
      }
    }
  }  


  
  fprintf(stderr, "Annotation tables are loaded, dumping breaks.\n");


  for (i=0; i<MAX; i++){
    if (seq[i]!=curmark){
      curmark = seq[i];
      if (lastend != -1)
	fprintf(dump, "%d\n", i-1);
      if (curmark != 0){
	fprintf(dump, "%s\t%d\t", thischr, i);	
	lastend=1;
      }
      else
	lastend=-1;
    }
  }
  if (lastend != -1)
    fprintf(dump, "%d\n", i-1);
  
}






static int compare(const void *p1, const void *p2){
  float a, b;

  a = *((float *)p1);
  b = *((float *)p2);

  if (a>b) 
    return 1;
  else
    return -1;
}

