/* sorts a blast parse output by the hits that give the most coverage */

#include <stdio.h>

int cover[500];
char hitname[500][500];

int main(int argc, char **argv){
  int i;
  int j;
  int zero, one;
  char fname[100];
  FILE *fp;
  char line[1000];
  char header[1000];

  char gene[100];
  int gs, ge, gl;
  
  char hit[100];
  int hs, he, hl;
  int numfrag=0;

  char prevhit[100];
  
  int numhit=0;
  int coverswap; 
  int isHeader = 1;
  
  fname[0] = 0;
  
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-nh"))
      isHeader = 0;
  }

  fp = fopen (fname, "r");

  if (isHeader)
    fgets(header, 1000, fp); // pass header

  memset(cover, 0, sizeof(char)*500);
  prevhit[0]=0;

  for (i=0;i<500;i++) hitname[i][0]=0;

  i=-1;
  
  while(fscanf(fp, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t", gene, &gs, &ge, &gl, hit, &hs, &he, &hl) > 0){
    fgets(line, 1000, fp); // pass the rest
    if (strcmp(prevhit, hit)){
      strcpy(prevhit, hit);
      i++;
      strcpy(hitname[i], hit);
      cover[i] = ge-gs+1;
      numhit++;
    }
    else
      cover[i] += ge-gs+1;
  }

  for (i=0;i<numhit;i++){
    for (j=0;j<numhit;j++){
      if (i>j && cover[i]>cover[j]){
	coverswap = cover[i];
	strcpy(prevhit, hitname[i]);
	cover[i] = cover[j];
	strcpy(hitname[i], hitname[j]);
	cover[j] = coverswap;
	strcpy(hitname[j], prevhit);
      }
    }
  }

  printf("%s", header);
  for (i=0;i<numhit;i++){
    rewind(fp);
    fgets(line, 1000, fp); // pass header

    while(fscanf(fp, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d", gene, &gs, &ge, &gl, hit, &hs, &he, &hl) > 0){
      fgets(line, 1000, fp); // pass the rest
      if (!strcmp(hit, hitname[i]))
	printf("%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d%s", gene, gs, ge, gl, hit, hs, he, hl, line);
    }
    
  }
  
}
