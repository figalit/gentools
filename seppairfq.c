#include <stdio.h>

int main(int argc, char **argv){
  FILE *fp;
  int i;
  char fname[100];
  char fname2[100];
  FILE *one, *two;
  char name[200], seq[200], qual[200], plus[200];

  fname[0]=0;
  for (i=1; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
  }

  fp = fopen(fname, "r");
  
  if (fp == NULL) return 0;

  sprintf(fname2, "%s_1", fname);
  one = fopen(fname2, "w");

  sprintf(fname2, "%s_2", fname);
  two = fopen(fname2, "w");


  while (1){
    fgets(name, 200, fp);
    if (feof (fp) ) break;
    fgets(seq, 200, fp);
    fgets(plus, 200, fp);
    fgets(qual, 200, fp);

    fprintf(one, "%s%s%s%s", name, seq,plus, qual);


    fgets(name, 200, fp);
    if (feof (fp) ) break;
    fgets(seq, 200, fp);
    fgets(plus, 200, fp);
    fgets(qual, 200, fp);

    fprintf(two, "%s%s%s%s", name, seq,plus, qual);
    

  }
  
}
