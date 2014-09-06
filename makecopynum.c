#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv){
  
  int i;
  int len;

  char chr[20]; int s; int e;
  
  int *bpcnt;
  char fname[100];
  FILE *fp;
  char bname[100];
  FILE *bfp;
  float avecpy;
  int copy;
  int nbp;
  char thischr[30];

  len=0; fname[0]=0; bname[0]=0;

  for (i=1; i<argc; i++){
    if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-b"))
      strcpy(bname, argv[i+1]);
  }

  if (len==0 || fname[0]==0 || bname[0]==0){
    fprintf(stderr, "check input %d\t%s\t%s\n", len, fname, bname);
    return 0;
  }

  fp = fopen(fname, "r");

  bpcnt = (int *) malloc(len * sizeof (int));

  for (i=0;i<len;i++)
    bpcnt[i]=1;

  //  memset(bpcnt, 1, len*sizeof(int));

  //printf("%d\t%d\n", bpcnt[0], bpcnt[len-1]);

  while (fscanf(fp, "%s%d%d", chr, &s, &e) > 0){
    for (i=s;i<=e;i++)
      bpcnt[i]++;
  }

  strcpy(thischr, chr);
  //printf("%d\t%d\n", bpcnt[0], bpcnt[len-1]);

  fclose(fp);

  fprintf(stderr, "Dup table loaded.\n");
  
  bfp = fopen(bname, "r");

  if (bfp==NULL){
    fprintf(stderr, "can not open %s\n", bname);
    return 0;
  }


  while (fscanf(bfp, "%s%d%d", chr, &s, &e) > 0){
    if (strcmp(chr, thischr)) continue;
    nbp = e-s+1;

    copy=0;
    
    for (i=s;i<=e;i++){
      copy+=bpcnt[i];
    }

    avecpy = ((float)copy  / (float)nbp) * 2;
    
    fprintf(stdout, "%s\t%d\t%d\t%d\t%f\n", chr, s,e, copy, avecpy);

  }
  
  
}
