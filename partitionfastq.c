#include <stdio.h>
#include <math.h>
#define MAX 200


int encode_window(char *window);

int main(int argc, char **argv){
  int i;
  int numpart;
  char fname[100];
  char infile[100];
  FILE *infp;
  FILE **fp;
  char window[MAX];
  int numbits;

  char read1[MAX], qual1[MAX], read2[MAX], qual2[MAX], name1[MAX], name2[MAX], plus1[MAX], plus2[MAX];
  int code;

  infile[0]=0;
  numpart=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      numbits = atoi(argv[i+1]);
  }

  numpart = pow(4, numbits);
  if (infile[0]==0){ fprintf(stderr, "Input file ?\n"); return 0;}
  if (numpart<=0){ fprintf(stderr, "Number of parts ?\n"); return 0;}
  
  if(!strcmp(infile, "stdin")) infp=stdin;
  else infp=fopen(infile, "r");

  if (infp == NULL){ fprintf(stderr, "Input file %s cannot be opened\n", infile); return 0;}

  fprintf(stdout, "Creating %d files.\n", numpart);
  fp = (FILE **) malloc(sizeof (FILE *) * numpart);
  for (i=0;i<numpart;i++){
    sprintf(fname, "%d", i);
    fp[i] = fopen(fname, "w");
  }

  fprintf(stdout, "Splitting %s.\n", infile);
  
  while(!feof(infp)){
    fgets(name1, MAX, infp);
    if (feof(infp)) break;
    fgets(read1, MAX, infp);
    if (feof(infp)) break;
    fgets(plus1, MAX, infp);
    if (feof(infp)) break;
    fgets(qual1, MAX, infp);
    if (feof(infp)) break;


    fgets(name2, MAX, infp);
    if (feof(infp)) break;
    fgets(read2, MAX, infp);
    if (feof(infp)) break;
    fgets(plus2, MAX, infp);
    if (feof(infp)) break;
    fgets(qual2, MAX, infp);
    if (feof(infp)) break;

    memcpy(window, read1, numbits*sizeof(char));
    window[numbits]=0;

    code = encode_window(window);

    fprintf(fp[code], "%s%s%s%s%s%s%s%s", name1, read1, plus1, qual1, name2, read2, plus2, qual2);
   
  }

  fclose(infp);

}

int cindex(char ch){
  switch(toupper(ch)){
  case 'A':
    return 0;
    break;
  case 'C':
    return 1;
    break;
  case 'G':
    return 2;
    break;
  case 'T':
    return 3;
    break;
  }
  return 3;
}


int encode_window(char *window){
  int len = strlen(window);
  unsigned int code;
  unsigned int this;
  int shiftleft;
  int i;
  unsigned int left;

  code = 0;

  for (i=0;i<len;i++){
    this = cindex(window[i]);
    shiftleft = 2*(len-i-1);
    left = this << shiftleft;
    code = code | left;
  }
  return code;
}
