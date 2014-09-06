#include <stdio.h>

int main(int argc, char **argv){
  int nline=0;
  int i;
  int cnt;
  char line[1000];
  char fname[100];
  char outname[200];
  FILE *in, *out;
  char addline[1000];

  addline[0]=0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-n"))
      nline = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-a"))
      strcpy(addline, argv[i+1]);

  }


  if (nline==0)
    return 0;
  cnt = 0;

  in = fopen(fname, "r");
  sprintf(outname, "%s.%d", fname, cnt);
  out = fopen(outname, "w");


  if (addline[0]!=0){
    fprintf(out, "%s\n", addline);
    fprintf(stdout, "HEADER: %s\n", addline);
  }

  while (1){
    fgets(line, 1000, in);
    if (feof(in))
      break;
    fprintf(out, "%s", line);
    cnt++;
    if (cnt % nline == 0){
      fclose(out);
      sprintf(outname, "%s.%d", fname, cnt);
      out = fopen(outname, "w");
      
      if (addline[0]!=0)
	fprintf(out, "%s\n", addline);
    }
  }
}
