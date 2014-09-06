#include <stdio.h>
#include <ctype.h>

#define FASTQ_OFFSET 33
#define MAXSNP 2
#define SEQLEN 150

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
  int reflen;
  char seqname[100];
  char dummy[500];
  char ch;

  int *acnt, *ccnt, *gcnt, *tcnt;

  int start, end, mm;
  char chr[30];
  //char orient[2];
  char name[100];
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

  int orient;  int flag;
  int mapqual, mpos, isize;
  char cigar[100], mrnm[100], sequence[SEQLEN], quality[SEQLEN], dist[100], md[100], nm[100];
  char token[30];
  int tokenpos;
  
  char txtswap[SEQLEN];

  int snppos[MAXSNP];
  char buf[10];
  int bufk;

  char refseq[SEQLEN];
  char pemarker[5];

  seqname[0] = 0;
  fname[0] = 0;

  reflen = 250000000;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-p"))
      doperfect = 1;
    else if (!strcmp(argv[i], "-npt"))
      nptotal = 1;
    else if (!strcmp(argv[i], "-q"))
      QUAL_CUTOFF = atoi(argv[i+1]);
    /*
    else if (!strcmp(argv[i], "-s"))    
      strcpy(seqname, argv[i+1]);
    */
  }

  i=0;


  //scanf("%s", dummy);
  //  printf("dum:%s\n", dummy);


  while (scanf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n", name, &flag, chr, &start, &mapqual, cigar, mrnm, &mpos, &isize, sequence, quality, nm, md) > 0){

    /*
      M_SOLEXA-GA02_JK_PE_SL10:1:57:783:419 16 chr1 1 0 75M6I * 0 0 TAAACATAACCATAAGCCTAAACCTAAACATAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAA
      >DD><?9>B:=;=?A=844@A>8>79==7=9==M9TNOKRMXPSRTSR5:9:A>77;<@?;<;>C??;4;BE8<2?<<;5;  AS:i:177
    */


    if (flag & 0x0010) orient=1;
    else orient = 0;
    
    if ((flag & 0x0010) ^ (flag &0x0020)){
      if (isize > 0) 
	printf ("%d\n", isize);
    }
      //    printf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n", name, &flag, chr, &start, &mapqual, cigar, mrnm, &mpos, &isize, sequence, quality, nm, md) > 0){
    
    
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
  save[j]=0;
  return i+1;
}
