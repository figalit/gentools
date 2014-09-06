#include <stdio.h>
#include <string.h>
#include <ctype.h>

main(int argc, char **argv){
  char ch;
  char accession[100];
  char current[100];
  int cnt=-1;
  FILE *in, *out;
  if (argc != 4){
    printf("Input: .dnd file. Marks human HOR sequences, and given accession\n");
    printf("Usage : %s [inFile] [outFile] [Accession]\n", argv[0]);
    exit(0);
  }
  strcpy(accession, argv[3]);
  if ((in=fopen(argv[1], "r")) == NULL){
    printf("Cannot open infile %s\n", argv[1]);
    exit(0);
  }
  out = fopen(argv[2], "w");
  while (fscanf(in, "%c",&ch) > 0){
    if (ch=='(' || ch==')' || ch=='\n')
      fprintf(out,"%c",ch);
    else{
      while (ch!=':' && fscanf(in,"%c",&ch) >0 ){
	if (ch == 'A')
	  cnt = 0;
	else if (ch == '-' && cnt>-1){
	  current[cnt] = 0;
	  
	  if (!strcmp(current, accession))  // pre-selected accession
	    fprintf(out, "+"); 
	  else if (strstr(current, "AlphaSat")) // human HOR
	    fprintf(out, "*");
	  else                    // all other accessions
	    fprintf(out, "-");
	    
	  if (cnt > 0){
	    while (ch != ':')
	      fscanf(in, "%c", &ch);
	  }
	  cnt = -1;
	}
	if (isalnum(ch) && cnt != -1)
	  current[cnt++] = ch;
      }
	
      fprintf(out,":");
      while (ch!=',' && ch!=')'){
	if (fscanf(in, "%c",&ch) <= 0)
	  break;
	fprintf(out, "%c",ch);
      }
    }
  }
}
