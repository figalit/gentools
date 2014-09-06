#include <stdio.h>
#include <string.h>

main(){
  char line[1000];
  char rest[10000];
  char fname[1000];
  /*
    , rname[100];
    char fseq[100], rseq[100];
    char runname[100];

    char fqual[100], rqual[100];
  */

  char prunname[1000];
  int i; FILE *fp=NULL;
  

  prunname[0]=0;

  while (fscanf(stdin, "%s", fname) > 0){
    fgets(rest, 10000, stdin);
    if (strcmp(fname, prunname)){
      if (fp != NULL) fclose(fp);

      sprintf (line, "%s.txt", fname);
      fp = fopen(line, "a");
      fprintf (stderr, "Switching to %s\n", fname);
      strcpy(prunname, fname);
    }
    fprintf(fp, "%s%s", fname, rest);
  }
}
