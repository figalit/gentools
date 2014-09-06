#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX 300000000
char repeats[MAX];
char dups[MAX];


float calc_dup(int, int);
float calc_rep(int, int);


char header[1000]= "CHR\tSTART_OUTER\tSTART_INNER\tEND_INNER\tEND_OUTER\tTYPE_OF_EVENT\tSUPPORTING_PAIRS\tSAMPLEs\n";


FILE *in;
FILE *out;
FILE *rep;
FILE *dup;

char samplename[100];

int id = 1;

void do_chrom(char *thischrom, int i, int minsup, int maxsup, int mininvsup, float maxdup, float prune);

int total_del_bp=0;
int total_ins_bp=0;
int min_del=100000;
int max_del=0;
int del_cnt = 0;
int ins_cnt = 0;
int inv_cnt = 0;
int ram_for_del = 1;

void printHelp(char *progname){
  fprintf(stdout, "Usage:\n");
  fprintf(stdout, "%s -i <VH_calls> -o <tabular_out> -d <SegDup BED> -r <Transposons BED> -min <min_support> -max <max_support> -minv <min_sup_inversions> -f <max_SD_overlap_ratio> -s <sample_name>\n", progname);

  fprintf(stdout, "\nREQUIRED:\n");
  fprintf(stdout, "\t-i <VH_calls>: Input the calls file generated with VariationHunter.\n");
  fprintf(stdout, "\t-o <tabular_out>: Output file in tab delimited format.\n");
  fprintf(stdout, "\t-d <SegDup BED>: Coordinates for segmental duplications in BED format.\n");
  fprintf(stdout, "\t-r <Transposons BED>: Coordinates for transposons (Alu, L1, LTR, SVA, ERV) in BED format.\n");

  fprintf(stdout, "\nOPTIONAL:\n");
  fprintf(stdout, "\t-min <min_support>: Minimum number of supporting read pairs. Default is 2.\n");
  fprintf(stdout, "\t-max <max_support>: Maximum number of supporting read pairs. Default is 100.\n");
  fprintf(stdout, "\t-minv <min_sup_inversions>: Minimum number of supporting read pairs to call inversions. Default is equal to -min.\n");
  fprintf(stdout, "\t-f <max_SD_overlap_ratio>: Maximum ratio of segmental duplication intersection allowed. Default is 0.3.\n");
  fprintf(stdout, "\t-s <sample_name>: Individual sample name. Default will be the first library name in the VH calls file.\n");
  fprintf(stdout, "\t-noram: Do not apply repeat-associated-mapping filter for deletions. This filter removes calls that also has a transposon insertion signature.\n");
  fprintf(stdout, "\t-xx: Sample is female. This option removes chrY calls.\n");
  fprintf(stdout, "\t-p <prune_filter>: Threshold to prune based on concordant read depth. Used only for deletion calls, only if the pruning values are pre-calculated.\n");
  fprintf(stdout, "\n");
}

int main(int argc, char **argv){
  int minsup=2;
  int maxsup=100;
  int i, j;
  char fname[100];
  char outfname[100];

  char rname[100];
  char dname[100];
  float maxdup = 0.3;
  int s, e;
  char chrom[50];
  char thischrom[50];
  int os, oe, is, ie;
  char type;
  int support;
  char word[100];
  float dup_content;
  float rep_content_1;
  float rep_content_2;
  float prune = 10000;

  time_t rawtime;
  struct tm * timeinfo;

  int male = 1;
  int mininvsup=-1;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  strcpy(samplename, "VHcall_");


  if (argc == 1){
    printHelp(argv[0]);
    return -1;
  }

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i")) strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-o")) strcpy(outfname, argv[i+1]);
    else if (!strcmp(argv[i], "-r")) strcpy(rname, argv[i+1]);
    else if (!strcmp(argv[i], "-d")) strcpy(dname, argv[i+1]);
    else if (!strcmp(argv[i], "-min")) minsup = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-minv")) mininvsup = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-max")) maxsup = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-f")) maxdup = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-p")) prune = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-xx")) male = 0;
    else if (!strcmp(argv[i], "-s")) strcpy(samplename, argv[i+1]);
    else if (!strcmp(argv[i], "-h")) {printHelp(argv[0]); return 0;}
    else if (!strcmp(argv[i], "-noram")) ram_for_del = 0;
  }

  if (mininvsup == -1)
    mininvsup = minsup;

  if (fname[0]==0){ 
    fprintf(stderr, "VH_calls file needed through -i\n"); return -1;
  }

  if (outfname[0]==0){ 
    fprintf(stderr, "Output file needed through -o\n"); return -1;
  }

  if (rname[0]==0){ 
    fprintf(stderr, "Transposons BED file needed through -r\n"); return -1;
  }

  if (dname[0]==0){ 
    fprintf(stderr, "SegDups BED file needed through -d\n"); return -1;
  }

  
  in = fopen (fname, "r");
  out = fopen (outfname, "w");
  rep = fopen (rname, "r");
  dup = fopen (dname, "r");

  if (in==NULL){
    fprintf(stderr, "VH_calls file %s not found.\n", fname); return -1;
  }

  if (out==NULL){
    fprintf(stderr, "Output file %s cannot be created.\n", outfname); return -1;
  }

  if (rep==NULL){
    fprintf(stderr, "Transposons BED file %s not found.\n", rname); return -1;
  }
  
  if (dup==NULL){
    fprintf(stderr, "SegDups BED file %s not found.\n", dname); return -1;
  }



  if (mininvsup == -1)
    mininvsup = minsup; 

  //  fprintf(stderr, "prune: %f\n", prune);

  fprintf(out, "%s", header);

 
  for (i=1; i<=22; i++){

    sprintf(thischrom, "chr%d", i);
    do_chrom(thischrom, i, minsup, maxsup, mininvsup, maxdup, prune);
  }

  i=23;
  sprintf(thischrom, "chrX", i);
  do_chrom(thischrom, i, minsup, maxsup, mininvsup, maxdup, prune);

  if (male){
    i=24;
    sprintf(thischrom, "chrY", i);
    do_chrom(thischrom, i, minsup, maxsup, mininvsup, maxdup, prune);
  }

  fprintf(stdout, "Sample = %s\n", samplename);
  fprintf(stdout, "%d dels, %d ins, %d invs.\n", del_cnt, ins_cnt, inv_cnt);
  fprintf(stdout, "Total del %d bp.\nTotal ins %d bp.\n", total_del_bp, total_ins_bp);
  fprintf(stdout, "Min del %d bp.\nMax del %d bp.\n", min_del, max_del);

}


float calc_dup(int s, int e){
  int len = e-s+1;
  int dupbp = 0;
  int i;

  for (i=s-1; i<e; i++) if (dups[i]==1) dupbp++;

  if (len > 0)
    return (float)dupbp / (float)len;
  else
    return (dups[s-1]==1) * 100.0;
  
}

float calc_rep(int s, int e){
  int len = e-s+1;
  int repbp = 0;
  int i;

  for (i=s-1; i<e; i++) if (repeats[i]==1) repbp++;

  if (len > 0)
    return (float)repbp / (float)len;
  else
    return (repeats[s-1]==1) * 100.0;
  
}


void do_chrom(char *thischrom, int i, int minsup, int maxsup, int mininvsup, float maxdup, float prune){

  int j;
  int s, e;
  char chrom[50];

  int os, oe, is, ie;
  char type;
  int support;
  char word[100];
  float dup_content;
  float rep_content_1;
  float rep_content_2;

  char _alt[20];
  char line[10000];
  float thisprune;

    fprintf(stderr, "chromosome %s\n", thischrom);
    rewind(rep); rewind(dup);
    rewind (in);

    memset (repeats, 0, MAX*sizeof(char));
    memset (dups, 0, MAX*sizeof(char));



    while (fscanf(rep, "%s\t%d\t%d\n", chrom, &s, &e) > 0){
      if (!strcmp(chrom, thischrom)){
	for (j=s;j<e;j++) repeats[j]=1;
      }
    }

    while (fscanf(dup, "%s\t%d\t%d\n", chrom, &s, &e) > 0){
      if (!strcmp(chrom, thischrom)){
	for (j=s;j<e;j++) dups[j]=1;
      }
    }
   
    while (fscanf(in, "%s", word) > 0){

      if (!strstr(word, "Chr:")){	
	fgets(line, 10000, in);
	continue;
      }
      
      strcpy (chrom, word+strlen("Chr:"));
     
      if (strcmp(chrom, thischrom)) {
	fgets(line, 10000, in);
	continue;
      }
 
      
      fscanf(in, "%s", word);
      os = atoi(word + strlen("Start_Outer:"));
      fscanf(in, "%s", word);
      is = atoi(word + strlen("Start_Inner:"));
      fscanf(in, "%s", word);
      ie = atoi(word + strlen("End_Inner:"));
      fscanf(in, "%s", word);
      oe = atoi(word + strlen("End_Outer:"));

      fscanf(in, "%s", word);
      type = word[7];

      fscanf(in, "%s", word);
      support = atoi(word + strlen("sup:"));

      if (type=='D' && prune!=10000)
	fscanf(in, "%s", word); // avg_span

      fscanf(in, "%s", word); // sum_weight
      fscanf(in, "%s", word); // lib

      if (!strcmp(samplename, "VHcall_")){
	strcpy(samplename, word+strlen("Lib:"));
      }

      thisprune = 0;
      if (prune == 10000 || type != 'D')
	fgets (word, 100, in); // libsup; skip
      else{
	fscanf(in, "%s", word); // libsup
	fscanf(in, "%s", word); // numcon
	thisprune = atof(word + strlen("NumCon:"));
	//fprintf(stderr, "prune: %f\tthisprune: %f\n", prune, thisprune);
      }
      
      if (support < minsup) continue;
      if (support > maxsup) continue;
      if (type == 'V' && support < mininvsup) continue;

      if (type == 'D' && thisprune > prune) continue;

      if (type=='D') strcpy(_alt, "DEL");
      else if (type=='I') strcpy(_alt, "INS");
      else if (type=='V') strcpy(_alt, "INV");

      if (type != 'V'){
	dup_content = calc_dup(is, ie);

	if (dup_content > maxdup)
	  continue;	   
	

      }

      if (type == 'V' || (ram_for_del && type == 'D')){

	rep_content_1 = calc_rep(os, is);
	rep_content_2 = calc_rep(ie, oe);

	if ((rep_content_1 >= 0.75 && rep_content_2 < 0.75) || (rep_content_1 < 0.75 && rep_content_2 >= 0.75)) continue;
	
	rep_content_1 = calc_dup(os, is);
	rep_content_2 = calc_dup(ie, oe);

	if ((rep_content_1 >= 0.75 && rep_content_2 < 0.75) || (rep_content_1 < 0.75 && rep_content_2 >= 0.75)) continue;
	
      }

      // "CHR\tSTART_OUTER\tSTART_INNER\tEND_INNER\tEND_OUTER\tTYPE_OF_EVENT\ttSUPPORTING_PAIRS\tSAMPLE\n"

      fprintf(out, "%s\t%d\t%d\t%d\t%d\t%s\t%d\t%s\n", thischrom, os, is, ie, oe, _alt, support, samplename);

      if (type=='D'){
	total_del_bp += (ie-is+1);
	if (min_del > ie-is+1) min_del = ie-is+1;
	if (max_del < ie-is+1) max_del = ie-is+1;
	del_cnt++;
      }
      else if (type=='I'){
	total_ins_bp += (ie-is+1);
	ins_cnt++;
      }
      else if (type=='V'){
	inv_cnt++;
      }


    }
}
