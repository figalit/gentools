#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define FASTQ_OFFSET 33
#define MAXSNP 2
#define SEQLEN 150

int  mytok(char *str, char delim, char *save, int start);
int checkLane(char **argv, int laneStart, int laneEnd, char *name);

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
  int reflen;
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
  int doperfect=0;
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


  char token[30];
  int tokenpos;


  

  char txtswap[SEQLEN];

  int snppos[MAXSNP];
  char buf[10];
  int bufk;

  char refseq[SEQLEN];
  char pemarker[5];
  int markend=1;

  int mincut=0; int maxcut=1000;
  int isconcordant;
  int span;
  int unmappedonly=0;

  int lanecnt;
  int laneStart, laneEnd;
  int recover;

  seqname[0] = 0;
  fname[0] = 0;

  reflen = 250000000;
  laneEnd = 0; laneStart = 1;

  i=0;
  while (i<argc){
  //  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-p"))
      doperfect = 1;
    else if (!strcmp(argv[i], "-npt"))
      nptotal = 1;
    else if (!strcmp(argv[i], "-nomark"))
      markend = 0;
    else if (!strcmp(argv[i], "-q"))
      QUAL_CUTOFF = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-min"))
      mincut = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-max"))
      maxcut = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-unmapped"))
      unmappedonly = 1;
    else if (!strcmp(argv[i], "-lanes")){
      laneStart=i+1; laneEnd=i+1; i++;
      while (i<argc && argv[i][0]!='-') laneEnd=i++;
    }
    /*
    else if (!strcmp(argv[i], "-s"))    
      strcpy(seqname, argv[i+1]);
    */
    else i++;
  }


  i=0;

  lanecnt = laneEnd - laneStart + 1;

  if (lanecnt != 0){
    fprintf(stderr, "Num_Lanes: %d\n", lanecnt);
    for (i=laneStart; i<=laneEnd; i++)
      fprintf(stderr, "Lane: %s\n", argv[i]);
    fflush(stderr);
  }

  //scanf("%s", dummy);
  //  printf("dum:%s\n", dummy);


  recover = 0;
  while (1){

    if (!recover){
      scanf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s", name, &flag, chr, &start, &mapqual, cigar, mrnm, &mpos, &isize, sequence, quality);
      if (feof(stdin))
	break;
      fgets(rest1, 500, stdin);
    }

    scanf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s", name2, &flag2, chr2, &start2, &mapqual2, cigar2, mrnm2, &mpos2, &isize2, sequence2, quality2);
    if (feof(stdin))
      break;
    fgets(rest2, 500, stdin);

    chptr = strrchr(name, '.');
    if (chptr != NULL)
      *chptr = 0;

    chptr = strrchr(name2, '.');
    *chptr = 0;

    if (!(strstr(name, "/1") && strstr(name2, "/2"))){
      fprintf(stderr, "Warning: names don't have /1 and /2:\t%s\t%s\n", name, name2);
      recover = 1;
      
      strcpy(name, name2);
      strcpy(chr, chr2);
      strcpy(cigar, cigar2);
      strcpy(mrnm, mrnm2);
      strcpy(sequence, sequence2);
      strcpy(quality, quality2);
      flag = flag2;
      start = start2;
      mapqual = mapqual2;
      mpos = mpos2;
      isize = isize2;

      continue;
      //return -1;
    }


    chptr = strrchr(name, '/');
    if (chptr != NULL)
      *chptr = 0;
    

    chptr = strrchr(name2, '/');
    *chptr = 0;

    if (strcmp(name, name2)){
      fprintf(stderr, "names don't match :\t%s\t%s\n", name, name2);

      recover = 1;
      
      strcpy(name, name2);
      strcpy(chr, chr2);
      strcpy(cigar, cigar2);
      strcpy(mrnm, mrnm2);
      strcpy(sequence, sequence2);
      strcpy(quality, quality2);
      flag = flag2;
      start = start2;
      mapqual = mapqual2;
      mpos = mpos2;
      isize = isize2;

      continue;

    }

    /*
      M_SOLEXA-GA02_JK_PE_SL10:1:57:783:419 16 chr1 1 0 75M6I * 0 0 TAAACATAACCATAAGCCTAAACCTAAACATAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAA
      >DD><?9>B:=;=?A=844@A>8>79==7=9==M9TNOKRMXPSRTSR5:9:A>77;<@?;<;>C??;4;BE8<2?<<;5;  AS:i:177
    */

    if (recover){
      fprintf(stderr, "recovered: %s\t%s\n", name, name2);
    }

    recover = 0;

    if (flag & 0x0010) orient=1;
    else orient = 0;

    if (flag2 & 0x0010) orient2=1;
    else orient2 = 0;

    isconcordant = 1;
    
    if (flag & 0x0004 || flag2 & 0x0004)
      isconcordant = 0;
    else if (orient == orient2 && !unmappedonly)
      isconcordant = 0;
    else if (strcmp(chr, chr2) && !unmappedonly)
      isconcordant = 0;
    else if (!unmappedonly){
      span = abs(start - start2) + strlen(sequence);
      if (span < mincut || span > maxcut)
	isconcordant = 0;
    }
    

    if (lanecnt != 0 &&  (!checkLane(argv, laneStart, laneEnd, name)))
      isconcordant = 0; // just skip no matter what             


    if (isconcordant){

      printf("%d\n", span);

      /*
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
      
      linecnt++;
      
      
      printf("@%s/1\n%s\n+\n%s\n", name, sequence, quality);
      
      printf("@%s/2\n%s\n+\n%s\n", name, sequence2, quality2);
      */
    }


    //    else{
    // fprintf(stderr, "CONCORDANT %s\n", name);
    //}
    
  }




}


int  mytok(char *str, char delim, char *save, int start){
  int i,j;
  int len = strlen(str);
  save[0]=0; j=0;
  for (i=start; i<len; i++){
    if (delim == str[i]){
      save[j]=0; 
      //printf("tok2: %s\n", save); 
      break;
    }
    else
      save[j++] = str[i];
  }
  //  printf("tok: %s\ti:%d\tlen:%d\n", save, i, len);
  save[strlen(str)]=0;
  return i+1;
}


int checkLane(char **argv, int laneStart, int laneEnd, char *name){
  int i;
  for (i=laneStart; i<=laneEnd; i++){
    //fprintf(stderr, "check %s in %s\n", argv[i], name);                                                                                                                                                                                    
    if (strstr(name, argv[i]))
      return 1;
  }
  return 0;
}



