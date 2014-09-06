#include <stdio.h>

char *isrepeat;

int main(int argc, char **argv){

  int len;
  char fname[100];
  char repname[100];
  int i;
  char chr[10]; int s, e;
  FILE *rp; FILE *fp;
  int intersect;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-r"))
      strcpy(repname, argv[i+1]);
  }

  isrepeat = (char *) malloc(sizeof(char) * (len+1));

  memset(isrepeat, '\0', (sizeof(char)*(len+1)));

  fp = fopen(fname,"r");

  rp = fopen(repname, "r");

  while (fscanf(rp, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    for (i=s;i<=e;i++)
      isrepeat[i] = 1;
  }

  fclose(rp);

  fprintf(stderr, "repeats read\n");

  while (fscanf(fp, "%s\t%d\t%d\n", chr, &s, &e) > 0){
    
    intersect = 0;

    for (i=s; i<=e; i++){
      if (isrepeat[i]==1){
	intersect=1;
	break;
      }
    }
    
    if (!intersect){
      fprintf(stdout, "%s\t%d\t%d\n", chr, s, e);
    }
    
  }


}
