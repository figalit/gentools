/*
  given list of names of sequences 
  and SUBFAMILIES.assign file
  renames the number of sequences with the names of sequences
*/

#include <stdio.h>

#define subfile "SUBFAMILIES.assign"
#define outfile "NEW.SUBFAMILIES.assign"


char **names;

int main(int argc, char **argv){
  
  FILE *nfile;
  FILE *afile;
  FILE *ofile;
  FILE *ffile;

  int pos;
  int i;
  int nseq;
  int subfamily;
  char thisname[200];

  if (argc != 2){
    fprintf(stderr, "Given list of names of sequences it renames the number of sequences with the names of sequences.\n");
    fprintf(stderr, "Usage: %s [names]\n", argv[0]);
    return 0;
  }


  nfile = fopen(argv[1], "r");
  afile = fopen(subfile, "r");
  
  if (afile == NULL || nfile == NULL){
    fprintf(stderr, "unable to open file.\n");
    return 0;
  }

  ofile = fopen(outfile, "w");
  
  nseq=0;
  while(fscanf(nfile, "%s", thisname) > 0)
    nseq++;

  rewind(nfile);
  mkdir("families", 0755);

  names = (char **) malloc (sizeof(char *) * nseq);
  for (i=0;i<nseq;i++)
    names[i] = (char *) malloc (sizeof(char) * 150);


  i = 0;
  while(fscanf(nfile, "%s", names[i++]) > 0)
    ;

  fclose(nfile);

  while (fscanf(afile, "%d %d", &pos, &subfamily) > 0){
    fprintf(stderr, "\r%d of %d", (pos+1), nseq);
    fprintf(ofile, "%s\t%d\n", names[pos], subfamily);
    sprintf(thisname, "families/%d.family", subfamily);
    ffile = fopen(thisname, "a+");
    //fseek(ffile, SEEK_END);
    fprintf(ffile, "%s\n", names[pos]);
    fclose(ffile);
  }
  fprintf(stderr, "\n");
  fclose(afile);
  fclose(ofile);

  return 1;

}
