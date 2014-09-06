#include <stdio.h>
#include <string.h>

main(){
  char line[1000];
  char fname[100], rname[100];
  char fseq[100], rseq[100];
  char runname[100];
  char prunname[100];
  char fqual[100], rqual[100];
  int i; FILE *fp=NULL;
  
  runname[0]=0;
  prunname[0]=0;

  while (fscanf(stdin, "%s\n", fname) > 0){
    i=0; 
    while (fname[i]!='_'){
      runname[i]=fname[i]; i++;
    }
    runname[i]=fname[i]; i++;
    runname[i]=fname[i]; i++;
    runname[i]=0;
    if (strcmp(runname, prunname)){
      if (fp != NULL) fclose(fp);
      sprintf (line, "%s.txt", runname);
      fp = fopen(line, "a");
      fprintf (stderr, "Switching to %s\n", runname);
      strcpy(prunname, runname);
    }
    fprintf(fp, "%s\n", fname);
  }
}
