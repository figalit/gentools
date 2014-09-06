#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define WINDOW_SIZE 1000

main(int argc, char **argv){
  FILE *stdFile;
  int i, k, cnt, j;
  int cutoff;
  int slidepos; int value;
  FILE *in;
  char ch;
  FILE *out;
  char fname[25];
  char seqs[17][30000];
  for (i=0;i<17;i++){
    //i=0;
    sprintf(fname, "file%d.aln", (i+1));
    in = fopen(fname,"r");
    k=0;
    while (fscanf(in,"%c",&ch) > 0)
      if (ch != '\n')
	seqs[i][k++] = ch;
    seqs[i][k] = 0;
    fclose(in);
    printf("Sequence %d is read in %s!!\n",i,fname);
  }
  stdFile = fopen(argv[1], "r");
  cutoff = atoi(argv[2]);
  sprintf(fname,"%s.out",argv[1]);
  out = fopen(fname, "w");
  for (i=0;i<30000;i++){
    if (seqs[0][i] == 0)
      break;
    printf("%c",seqs[0][i]);
  }
  printf("\n");
  //printf("\n%s\n",seqs[0]);
  
  while (fscanf(stdFile, "%d %d", &slidepos, &value) > 0){
    if (value <= cutoff){
      printf("slidepos  = %d  value = %d \n",slidepos, value);
      cnt = slidepos;
      while (cnt < slidepos+WINDOW_SIZE){
	printf("cnt: %d\n",cnt);
	for (j=0; j<17; j++){
	  //if (cnt == slidepos)
	  if (j<10)
	    fprintf(out, "Seq %d:  ",j);
	  else
	    fprintf(out, "Seq %d: ",j);
	  for (i=cnt; i<cnt+50; i++){
	    //printf("j: %d i: %d ch: %c\n",j,i,seqs[j][i]);
	    if (i >= strlen(seqs[0]))
	      break;
	    fprintf(out, "%c", seqs[j][i]);
	  } // for i
	  fprintf(out, "\n");
	} // for j
	fprintf(out, "\n");
	cnt = i;
      } // while
      fclose(out);
      exit(0);
    } // if value
  } // while
  
} // main


