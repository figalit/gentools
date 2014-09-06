#include <stdio.h>
#include <string.h>
//#define MAX 30000000
#define MAX 250000000


typedef struct mask{
  char chr[30];
  int start, end;
}_mask;

struct mask masq[1141763];
//struct mask masq[1043598];

char seq[MAX];

int main(int argc, char **argv){
  int i,j,m;
  char reptabname[100];
  FILE *reptab;
  char fastaname[100];
  FILE *fp;
  char outname[100];
  FILE *out;
  int tabcnt;
  char dum1[100], dum2[100];
  float div;
  char seqname[100];
  int istart=0;
  char ch;
  int seqlen;
  char chr[30]; int s,e;
  char maskchar='N';

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-t"))
      strcpy(reptabname, argv[i+1]);
    else if (!strcmp(argv[i], "-f"))
      strcpy(fastaname, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      strcpy(seqname, argv[i+1]);
    else if (!strcmp(argv[i], "-m"))
      maskchar = argv[i+1][0];
    else if (!strcmp(argv[i], "-istart"))
      istart = atoi(argv[i+1]);
  }

  fprintf(stdout, "Masking %s\n", fastaname);
  sprintf(outname, "%s.masked", fastaname);
  
  reptab = fopen(reptabname, "r");
  fp = fopen(fastaname, "r");
  out = fopen(outname, "w");

  i = 0;
  j = 0;

  //  while (fscanf(reptab, "%s%d%d%s%s%f", chr, &s, &e, dum1, dum2, &div) > 0){
  while (fscanf(reptab, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    if (!strcmp(chr, seqname) && s>=istart){
      strcpy(chr, masq[i].chr);
      masq[i].start = s;
      masq[i].end = e;
      i++;
    }
  }
  
  tabcnt = i;
  fprintf(stderr, "Will mask %d intervals.\n", tabcnt);

  fgets(dum1, 100, fp);
  
  i=0;

  while(fscanf(fp, "%c", &ch)>0){
    if (isalpha(ch))
      seq[i++]=ch;
  }
  seq[i]=0;
  seqlen=i;

  fprintf(out, "%s", dum1);

  j=0; m=0;
  for (i=0;i<seqlen;i++){
    while (masq[j].end < i+istart && j<tabcnt) j++;
    
    if (j == tabcnt){
      fprintf(out, "%c", seq[i]);
      m++;
    }
    else{
      if ((i+istart) >= masq[j].start && (i+istart) <= masq[j].end){
	//printf("\rMASKING %d\t%d\t%d\t%d\tchar:%c", masq[j].start, masq[j].end, (i+istart), i, maskchar);
	if (maskchar != '0')
	  fprintf(out, "%c", maskchar);
	else
	  fprintf(out, "%c", tolower(seq[i]));
      }
      else
	fprintf(out, "%c", seq[i]);
      m++;
    }
    
    if (m%60==0)
      fprintf(out, "\n");
    
  }

  printf("\n");
  

}
