#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 250000000

#define MAXGENE 100000

unsigned int seq[MAX];

unsigned short genes[MAX];

static int compare(const void *p1, const void *p2);

typedef struct gene{
  char chr[20];
  int s, e;
  char *name;
}_gene;

struct gene genetab[MAXGENE];

void dochr(char *thischr, FILE *border, FILE **fp, int fileStart, int fileEnd);

int third=0;


FILE *dump;

FILE **fp;

FILE *genefile;

int genecount = 0;

int baseone;

int main(int argc, char **argv){

  FILE *border, *acgh;
  
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

  int withchr = 0;
  if (argc<3){
    printf("BED file annotator by Can Alkan.\n\nVersion April 27, 2010.\n\n");
    printf("%s [parameters]\n", argv[0]);
    printf("\t-i [filename]:\t\tPivot file name. The coordinates in this BED file will be annotated.\t<mandatory>\n");
    printf("\t-a [annotation files]:\tList of BED files that will be used for annotation.\t\t\t<mandatory>\n");
    printf("\t-o [output file]:\tOutput file.\t\t\t\t\t\t\t\t<mandatory>\n");
    printf("\t-g [gene file]:\t\tGene table as a BED4 file.\t\t\t\t\t\t<optional>\n");
    printf("\t-c :\t\tPrefix chr to chromosome names.\t\t\t\t\t\t<optional>\n");
    printf("\nAll options except -g are mandatory!\nAnnotation files (-a) should be BED3, gene table should be BED4.\nAll tab-delimited text files.\n");
    printf("\nExample:\n\t%s -i microhotspots.bed -a wgac.bed wssd.bed repeatmasker.bed -o microhotspots.annotated_output.bed\n", argv[0]);
    printf("\n\nThis version supports only chr1-chr22-chrX-chrY. _random chromosomes, and chromosome names for other species (chr2a, chr2b, etc.) will be added later.\n");
    return;
  }

  i=1;

  genefile = NULL;
  genefname[0]=0;
  fileEnd = 0; fileStart = 1;

  while (i<argc){
    if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "-i"))
      strcpy(wssdfile, argv[++i]);
    else if (!strcmp(argv[i], "-a")){
      fileStart=i+1; fileEnd=i+1; i++;
      while (i<argc && argv[i][0]!='-') fileEnd=i++;
    }
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[++i]);
    else if (!strcmp(argv[i], "-g"))
      strcpy(genefname, argv[++i]);
    else if (!strcmp(argv[i], "-c")){
      withchr=1; i++;
    }
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


  border = fopen(wssdfile, "r");

  if (border==NULL){
    fprintf(stderr, "Unable to open pivot file %s\n", wssdfile);
    return 0;
  }

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


  if (genefname[0]!=0){
    genefile = fopen(genefname, "r");

    if (genefile==NULL){
      fprintf(stderr, "Unable to open gene file %s\n", outfile);
      return 0;
    }

    printf("Gene File: %s\n", genefname);

    genecount=1;  // don't use index 0, it messes things up later on
    
    while (fscanf(genefile, "%s\t%d\t%d\t%s\n", chr, &s, &e, gname) > 0){

      genetab[genecount].name = (char *) malloc(sizeof (char) * (strlen(gname)+1));
      strcpy(genetab[genecount].name, gname);
      strcpy(genetab[genecount].chr, chr);
      genetab[genecount].s = s;
      genetab[genecount].e = e;
      genecount++;
    }

    fclose(genefile);
    fprintf(stderr, "%d genes loaded.\n", (genecount-1));
  }


  fprintf(dump, "chrom\tstart\tend");


  fscanf(border, "%s%d%d", chr, &s, &e);
  fgets(rest, 10000, border);
  rest[strlen(rest)-1] = 0;
  counttab=0;
  for (j=0;j<strlen(rest);j++){
    if (rest[j]=='\t') counttab++;
  }
  rewind(border);
  fprintf(stderr, "Extra user columns: %d\n", counttab);

  for (i=0;i<counttab;i++) fprintf(dump,"\tuser_column_%d", i+1);

  for (i=fileStart; i<=fileEnd; i++){
    fprintf(dump, "\t%s_bp\t%s_perc", argv[i], argv[i]);
  }
  
  if (genecount != 0){
    fprintf(dump, "\tGene_bp\tGene_perc\tGenes");
  }


  fprintf(dump, "\n");

  fprintf(stderr, "Withchr: %d\n", withchr);

  for (i=1;i<=22;i++){

    fprintf(stderr, "Annotating chr%d\n", i);
    if (withchr)
      sprintf(thischr, "chr%d", i);
    else
      sprintf(thischr, "%d", i);
    rewind(border); 
    for (j=0;j<fileCount;j++)  rewind(fp[j]); 
    dochr(thischr, border, fp, fileStart, fileEnd);
  }


  fprintf(stderr, "Annotating chrX\n");
  if (withchr)
    sprintf(thischr, "chrX");
  else
    sprintf(thischr, "X");
  rewind(border); 
  for (j=0;j<fileCount;j++)  rewind(fp[j]); 
  dochr(thischr, border, fp, fileStart, fileEnd);
    
  fprintf(stderr, "Annotating chrY\n");
  if (withchr)
    sprintf(thischr, "chrY");
  else
    sprintf(thischr, "Y");
  rewind(border);  

  for (j=0;j<fileCount;j++)  rewind(fp[j]); 
  
  dochr(thischr, border, fp, fileStart, fileEnd);


    


}


void dochr(char *thischr, FILE *border, FILE **fp, int fileStart, int fileEnd){

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

  memset(seq, 0, sizeof(unsigned int)*MAX);
  memset(genes, 0, sizeof(unsigned short)*MAX);
  
  
  for (j=0; j<fileCount; j++){
    //marker = (unsigned int) pow (2, j);
    marker = 1;
    marker = marker << j;
    while (fscanf(fp[j], "%s\t%d\t%d\n", chr, &s, &e) > 0){
      //printf("chr %s this %s\n", chr, thischr);
	
      if (strcmp(chr, thischr)) continue;
      for (i=s; i<e; i++) {
	seq[i] = seq[i] | marker;
      }
    }
  }  


  if (genecount != 0){
    //    fprintf(stderr, "load genes\n");
    for (j=1;j<genecount;j++){
      if (!strcmp(thischr, genetab[j].chr)){
	//fprintf(stderr, "load gene %s\t%d\t%d\t%s\n", genetab[j].chr, genetab[j].s, genetab[j].e, genetab[j].name);
	for (i=genetab[j].s; i<genetab[j].e; i++) {
	  genes[i] = j;
	}
      }
    }

  }
  
  fprintf(stderr, "Annotation tables are loaded, reading pivot file\n");

  while (fscanf(border, "%s\t%d\t%d", chr, &s, &e) > 0){
    fgets(rest, 10000, border);
    rest[strlen(rest)-1] = 0;
    if (strcmp(chr, thischr)) continue;
    fprintf(dump, "%s\t%d\t%d", chr, s, e);
    /////////////////////////////////
    if (rest[0] != 0)
      fprintf(dump, "%s", rest);
    /////////////////////////////////
    for (j=0;j<fileCount;j++){
      //marker = (unsigned int) pow(2, j);
      marker = 1;
      marker = marker << j;
      count=0; 
      for (i=s;i<e;i++){
	if ((seq[i] & marker) == marker){
	  count++;
	}
      }
      fprintf(dump, "\t%d\t%f", count, ((float)count/(float)(e-s)));
    }

    genestring[0] = 0;

    if (genecount != 0){
      count=0; lastgene=0;

      for (i=s;i<e;i++){
	if (genes[i] != 0){
	  //fprintf(stderr, "FOUND GENE %d-%d\n", s, e);

	  count++;
	  if (genes[i] != lastgene){
	    lastgene = genes[i];
 	    strcat(genestring, genetab[lastgene].name);
 	    strcat(genestring, "; ");
	  }
	}
      }
      if (genestring[0] != 0){
	if (genestring[strlen(genestring)-1] == ' ') genestring[strlen(genestring)-1] = 0;
	if (genestring[strlen(genestring)-1] == ';') genestring[strlen(genestring)-1] = 0;
      }
      fprintf(dump, "\t%d\t%f\t%s", count, ((float)count/(float)(e-s)), genestring);
    }
    fprintf(dump, "\n");
  }
  
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

