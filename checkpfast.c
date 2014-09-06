#include <stdio.h>
char text[30000000];
char **seqs;
char **names;
#define MAXSEQ 5000
#define SEQLEN 50

main(int argc, char **argv){
  int seqcnt=0;
  char thisseq[SEQLEN];
  char thisname[SEQLEN];
  int i,j,k;
  char textname[SEQLEN*2];
  char readname[SEQLEN*2];
  char tabname[SEQLEN*2];
  FILE *textfile, *readfile, *tabfile;
  char dummy[SEQLEN];
  char ch;
  int textlen;
  int s, e, m;
  char o[2];

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-t"))
      strcpy(textname, argv[i+1]);
    else if (!strcmp(argv[i], "-r"))
      strcpy(readname, argv[i+1]);
    else if (!strcmp(argv[i], "-p"))
      strcpy(tabname, argv[i+1]);
  }
  
  textfile = fopen(textname, "r");
  readfile = fopen(readname, "r");
  tabfile = fopen(tabname, "r");
  
  seqs = (char **) malloc(sizeof(char *) * MAXSEQ);
  names = (char **) malloc(sizeof(char *) * MAXSEQ);

  while(fscanf(readfile, "%s%s", thisname, thisseq) > 0){
    seqs[seqcnt] = (char *) malloc(sizeof(char) * strlen(thisseq));
    names[seqcnt] = (char *) malloc(sizeof(char) * strlen(thisname));
    strcpy(seqs[seqcnt], thisseq);
    strcpy(names[seqcnt], (thisname+1));
    seqcnt++;
  }
  fclose(readfile);

  i=0;

  while(fscanf(textfile, "%c", &ch) > 0){
    if (ch == '>') fgets(dummy, SEQLEN, textfile);
    else if (isalpha(ch)) text[i++]=toupper(ch);
  }
  fclose(textfile);
  text[i]=0; textlen=i;

  i=0;

  while(fscanf(tabfile, "%s%d%d%d%s%s", thisname, &s, &e, &m, o, dummy) > 0){
    
    while (strcmp(names[i], thisname))
      i++;

    k=0;
    if (o[0]=='F'){
      for (j=s-1;j<e;j++){
	thisseq[k++] = text[j];
      }
      thisseq[k] = 0;
    }
    
    else if (o[0]=='R'){
      for (j=e-1;j>=s-1;j--){
	switch(text[j]){
	case 'A':
	  thisseq[k++] = 'T';
	  break;
	case 'C':
	  thisseq[k++] = 'G';
	  break;
	case 'G':
	  thisseq[k++] = 'C';
	  break;
	case 'T':
	  thisseq[k++] = 'A';
	  break;
	default:
	  thisseq[k++] = text[j];
	  break;

	}
      }
      thisseq[k] = 0;      
    }

    printf("%s\t%d\t%d\n", thisname, s, e);
    printf("\t%s\n\t%s\t\t%d\t%s\n\n", thisseq, seqs[i], m, o);
    
  } 
  

}
