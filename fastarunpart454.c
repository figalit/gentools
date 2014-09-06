#include <stdio.h>
#include <string.h>
#define MAX 60000

main(){
  char line[3000];
  char fname[MAX], rname[MAX];
  char fseq[MAX], rseq[MAX];
  char runname[MAX];
  char runnamef[MAX];
  char prunname[MAX];
  char fqual[MAX], rqual[MAX];
  int i; FILE *fp=NULL;
  char ch;
  
  runname[0]=0;
  prunname[0]=0;
  ch=0;
  do{
    fscanf(stdin, "%c", &ch);
  }  while (ch != '>');
    
  while (fscanf(stdin, "%s", fname) > 0){
    fgets(line, 1000, stdin);
    
    ch=0; i=0;
    while (ch!='>'){
      fscanf(stdin, "%c", &ch);

      if (feof(stdin)) break;
      if (!isspace(ch) && ch!='>')
	fseq[i++] = ch;
    }
    fseq[i] = 0;

    i=0; 

    /*
    while (fname[i]!=':'){
      runname[i]=fname[i]; i++;
    }
    runname[i]=fname[i]; i++;
    runname[i]=fname[i]; i++;
    runname[i]=0;
    */

    strcpy(runname, fname);
    runname[9] = 0;


    if (strcmp(runname, prunname)){
      if (fp != NULL) fclose(fp);
      fp = fopen(runname, "a");
      fprintf (stderr, "Switching to %s\n", runname);
      strcpy(prunname, runname);
    }
    fprintf(fp, ">%s\n%s\n", fname, fseq);
  }
}
