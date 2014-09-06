#include <stdio.h>
#include <string.h>

char seq[20000000];

int main(int argc, char **argv){
  FILE *out;
  char fname[1000];
  char outdir[1000];
  long i;
  int batch=0;
  char name[100];

  char dummy[10000];
  char *dot;
  int crop;

  strcpy(outdir, "./");
  crop = 0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-n"))
      batch = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outdir, argv[i+1]);
    else if (!strcmp(argv[i], "-crop"))
      crop = atoi(argv[i+1]);
  }
  
  i=0;
  fname[0]=0;
  while(!feof(stdin)){
    //while(scanf("%s\n%s\n", name, seq) > 0){
    if (scanf("%s", name) <= 0) break;
    fgets(dummy, 10000, stdin);
    if (name[0]=='#') continue;
    scanf("%s\n", seq);
    if (i%batch == 0){
      if (i!=0) fclose(out);
      if (batch!=1){
	sprintf(fname, "%s%d", outdir, i);
      }
      else
	sprintf(fname, "%s/%s", outdir, (name+1));
      fprintf(stderr, "%s\n", fname);
      out = fopen(fname, "w");

    }
    /*
    dot = strchr(seq, '.');
    if (dot != NULL) *dot = 0;*/
    if (crop != 0 && crop<=strlen(seq)) seq[crop]=0;
    fprintf(out, "%s\n%s\n", name, seq);
    i++;
  }
}
