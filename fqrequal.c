#include <stdio.h>
#define MODE64to32 0
#define MODE32to64 1


int main(int argc, char **argv){
  char str[1000];
  char line[1000];
  char last;
  char *crop;
  char fname[100];
  int i;
  char mode;
  strcpy(fname, "stdin");

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    
  }
    
  while(fscanf(fname, "%s", str) > 0){
    if (str[0]=='@'){
      fgets(line, 1000, fname);
      if (!strchr(str, '#'))
	  fprintf(stdout, "%s\n", str);
      else{
	last = str[strlen(str) - 1];
	crop = strrchr(str, '#');
	*crop = 0;
	if (last == '1' || last == '2')
	  fprintf(stdout, "%s/%c\n", str, last);
	else
	  fprintf(stdout, "%s\n", str);
      }
      fscanf(fname, "%s\n", str); // seq;
      fprintf(stdout, "%s\n", str);
      fgets(line, 1000, fname); // + line;
      fprintf(stdout, "+\n");
      fscanf(fname, "%s\n", str); // qual;
      fprintf(stdout, "%s\n", str);      
    }
  }
}
