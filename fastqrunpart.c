#include <stdio.h>
#include <string.h>

main(){
  char line[3000];
  char fname[300], rname[300];
  char fseq[300], rseq[300];
  char runname[300];
  char prunname[300];
  char fqual[300], rqual[300];
  int i; FILE *fp=NULL;

  runname[0]=0;
  prunname[0]=0;

  while (fscanf(stdin, "@%s\n%s\n%s\n%s\n@%s\n%s\n%s\n%s\n", fname, fseq, line, fqual, rname, rseq, line, rqual) > 0){
    i=0; 
    while (fname[i]!=':'){
      runname[i]=fname[i]; i++;
    }
    runname[i]=fname[i]; i++;
    runname[i]=fname[i]; i++;
    runname[i]=0;
    if (strcmp(runname, prunname)){
      if (fp != NULL) fclose(fp);
      fp = fopen(runname, "a");
      fprintf (stderr, "Switching to %s\n", runname);
      strcpy(prunname, runname);
    }
    fprintf(fp, "@%s\n%s\n+\n%s\n@%s\n%s\n+\n%s\n", fname, fseq, fqual, rname, rseq, rqual);
  }
}
