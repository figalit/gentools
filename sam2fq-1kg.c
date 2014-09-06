#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define FASTQ_OFFSET 33
#define MAXSNP 2
#define SEQLEN 200

int  mytok(char *str, char delim, char *save, int start);


char rc(char c){
  if (c=='A') return 'T';
  else if (c=='C') return 'G';
  else if (c=='G') return 'C';
  else if (c=='T') return 'A';
  else return 'N';
}

int main(int argc, char **argv){
  int i, k;
  char fname[500];
  char *ref;
  FILE *reffile;

  
  char seqname[100];
  char dummy[500];
  char ch;

  int *acnt, *ccnt, *gcnt, *tcnt;

  int start, end, mm;
  char chr[30];
  char chr2[30];
  char *chptr;
  //char orient[2];
  char name[100];
  char name2[100];

  char seqqual[40];
  char type1, type2;
  char snp1[5], snp2[5];
  int loc1, loc2;
  char q1[5], q2[5];
  int offset;
  int pass;

  int total;
  int wasperfect;
  int  QUAL_CUTOFF=25;
  int nptotal=0;
  int linecnt=0;

  int orient;  int flag; int orient2, flag2;
  int mapqual, mpos, isize;
  char cigar[100], mrnm[100], sequence[SEQLEN], quality[SEQLEN], dist[100], md[100], nm[100];
  char cigar2[100], mrnm2[100], sequence2[SEQLEN], quality2[SEQLEN], dist2[100];
  char rest1[500];  char rest2[500];
  int start2, mapqual2, mpos2, isize2;


  char token[100];
  int tokenpos;


  

  char txtswap[SEQLEN];

  int snppos[MAXSNP];
  char buf[10];
  int bufk;

  char refseq[SEQLEN];
  char pemarker[5];
  int markend=1;

  int mincut=0; int maxcut=0;
  int isconcordant;
  int span;
  int unmappedonly=0;
  

  char prevfname[1000];
  FILE *out;
  char rg[100];
  char rg2[100];
  char prefix[1000];

  seqname[0] = 0;
  fname[0] = 0;
  prefix[0] = 0;
  out = NULL;


  i=1;
  while (i<argc){
    //  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-min"))
      mincut = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-max"))
      maxcut = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-unmapped"))
      unmappedonly = 1;
    else if (!strcmp(argv[i], "-s"))
      strcpy(prefix, argv[++i]);
    else i++;
  }

  if (!unmappedonly && mincut==0 && maxcut==0){
    fprintf(stderr, "Need min/max\n");
    return -1;
  }

  i=0;


  //scanf("%s", dummy);
  //  printf("dum:%s\n", dummy);


  while (scanf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s", name, &flag, chr, &start, &mapqual, cigar, mrnm, &mpos, &isize, sequence, quality) > 0){

    fgets(rest1, 500, stdin);

    if (!(flag & 0x0001)){  //single end
      //printf("SINGLE END\n");
      continue; 
    }

    scanf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s", name2, &flag2, chr2, &start2, &mapqual2, cigar2, mrnm2, &mpos2, &isize2, sequence2, quality2);
    fgets(rest2, 500, stdin);


    token[0]=0; tokenpos=0;

    while (!strstr(token, "RG:Z:"))
      tokenpos = mytok(rest1, ' ', token, tokenpos);
    strcpy(rg, token+strlen("RG:Z:"));
	   
    token[0]=0; tokenpos=0;
    while (!strstr(token, "RG:Z:"))
      tokenpos = mytok(rest2, ' ', token, tokenpos);
    strcpy(rg2, token+strlen("RG:Z:"));
	   

    if (flag & 0x0010) orient=1;
    else orient = 0;
    
    if (flag & 0x0020) orient2=1;
    else orient2 = 0;


    isconcordant = 1;
    
    if (flag & 0x0004)  //unmapped
      isconcordant = 0;  
    else if (flag & 0x0008) //OEA
      isconcordant = 0;

    else if (orient == orient2 && !unmappedonly) //inverted or everted
      isconcordant = 0;

    else if (strcmp(mrnm, "=")) // trans
      isconcordant = 0;

    else if (!unmappedonly){
      if (abs(isize) < mincut || abs(isize) > maxcut)
	isconcordant = 0;
    }
    
    else if (strchr(cigar, 'S')!=NULL || strchr(cigar, 'D')!=NULL|| strchr(cigar, 'I')!=NULL || strchr(cigar, 'H')!=NULL || strchr(cigar, 'N')!=NULL || strchr(cigar, 'P')!=NULL)
      isconcordant = 0;
    else if (strchr(cigar2, 'S')!=NULL || strchr(cigar2, 'D')!=NULL|| strchr(cigar2, 'I')!=NULL || strchr(cigar2, 'H')!=NULL || strchr(cigar2, 'N')!=NULL || strchr(cigar2, 'P')!=NULL)
      isconcordant = 0;
    
    
    if (!isconcordant){

      if (orient==1){
	//	fprintf(stderr, "reversing %s/1\n", name);
	for (i=strlen(sequence)-1; i>=0; i--){
	  txtswap[strlen(sequence)-1 - i] = rc(sequence[i]);
	}
	txtswap[strlen(sequence)]=0;
	strcpy(sequence, txtswap);
	
	for (i=strlen(quality)-1; i>=0; i--){
	  txtswap[strlen(quality)-1 - i] = quality[i];
	}
	txtswap[strlen(quality)]=0;
	strcpy(quality, txtswap);
      }

      if (orient2==1){
	//fprintf(stderr, "reversing %s/2\n", name2);                                                                                                                                                                                        
        for (i=strlen(sequence2)-1; i>=0; i--){
          txtswap[strlen(sequence2)-1 - i] = rc(sequence2[i]);
	}
        txtswap[strlen(sequence2)]=0;
        strcpy(sequence2, txtswap);

        for (i=strlen(quality2)-1; i>=0; i--){
          txtswap[strlen(quality2)-1 - i] = quality2[i];
        }
        txtswap[strlen(quality2)]=0;
	strcpy(quality2, txtswap);

      }

      
      
    }
      
    linecnt++;
    
    if (prefix[0] != 0)
      sprintf(fname, "%s-%s.fastq", prefix, rg);
    else 
      sprintf(fname, "%s.fastq", rg);
    

    if (strcmp(fname, prevfname)){
      strcpy(prevfname, fname);
      
      if (out != NULL)
	fclose(out);
      out = fopen(fname, "a");
    }

    
      if (flag & 0x40){
	fprintf(out, "@%s/1\n%s\n+\n%s\n",  name, sequence, quality);
	fprintf(out, "@%s/2\n%s\n+\n%s\n",  name, sequence2, quality2);
      }
      else{
	fprintf(out, "@%s/1\n%s\n+\n%s\n",  name, sequence2, quality2);
	fprintf(out, "@%s/2\n%s\n+\n%s\n",  name, sequence, quality);
      } 
           
    }


    //    else{
    // fprintf(stderr, "CONCORDANT %s\n", name);
    //}
    
}






int  mytok(char *str, char delim, char *save, int start){
  int i,j;
  int len = strlen(str);
  save[0]=0; j=0;

  for (i=start; i<len; i++){
    if (str[i]==delim || isspace(str[i])){
      save[j]=0; 
      //printf("tok2: %s\n", save); 
      break;
    }
    else
      save[j++] = str[i];
  }
  //printf("tok: %s\ti:%d\tlen:%d\n", save, i, len);
  save[strlen(str)]=0;
  return i+1;
}


