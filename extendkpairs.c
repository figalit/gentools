#include <stdio.h>
#define MAX 250000000

char seq[MAX];

int maxmm=1;

int main(int argc, char **argv){
  FILE *fasta;
  FILE *kmers;
  int i,j;
  char fastaname[100];
  char kmername[100];
  char ch;
  char line[1000];
  int len;
  char chr[100];
  int s1, s2, e1, e2;
  int cmp;
  int dist=0;


  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-f"))
      strcpy(fastaname, argv[i+1]);
    else if (!strcmp(argv[i], "-k"))
      strcpy(kmername, argv[i+1]);
  }

  fprintf(stderr, "%s\t%s\n", fastaname, kmername);

  fasta = fopen(fastaname, "r");
  kmers = fopen(kmername, "r");

  if (fasta == NULL || kmers == NULL){
    fprintf(stderr, "File open error\n"); return 0;
  }

  i=0;

  while(fscanf(fasta, "%c", &ch) > 0){
    if (ch=='>')
      fgets(line, 1000, fasta);
    else{
      if (!isspace(ch)) seq[i++]=toupper(ch);
    }
  }
  seq[i]=0;
  len=i;

  fclose(fasta);

  fprintf(stderr, "File %s read, %d characters.\n", fastaname, len);
  
  

  while(fscanf(kmers, "%s\t%d\t%d\t%d\t%d\n", chr, &s1, &e1, &s2, &e2) > 0){

    dist = 0;
    i=s1; j=s2;

    /* extend upstream */
    while (dist<=maxmm && s1>=0 && s2>=0){
      s1=i;s2=j;
      if (seq[i-1] == seq[j-1]){ 
	i--; j--;
      }
      else{
	dist++;	
	if (seq[i-2] == seq[j-1]){
	  //del in left kmer
	  i--;
	} 
	else if (seq[i-1] == seq[j-2]){
	  //del in right kmer
	  j--;
	} 
      }
    }

    //s1=i+1; s2=j+1;

    /* extend downstream */
    i=e1; j=e2;
    while (dist<=maxmm && e1<len && e2<len){
      e1=i; e2=j;
      if (seq[i+1] == seq[j+1]){ 
	i++; j++;
      }
      else{
	dist++;	
	if (seq[i+2] == seq[j+1]){
	  //ins in left kmer
	  i++;
	} 
	else if (seq[i+1] == seq[j+2]){
	  //ins in right kmer
	  j++;
	} 
      }
    }



    fprintf(stdout, "%s\t%d\t%d\t%d\t%d\n", chr, s1, e1, s2, e2);
    
  }



  
}
