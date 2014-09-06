#include <stdio.h>

int main(int argc, char **argv){
  int i;
  FILE *fp;
  char filename[100];
  char newname[1000];
  char line[1000];
  if (argc != 3){
    printf("fasta sequence renamer. 1 sequence per file.\n%s <fasta> <newname>\n", argv[0]);
    return -1;
  }

  strcpy(filename, argv[1]);
  strcpy(newname, argv[2]);
  fp = fopen(filename, "r");
  if (fp==0) { printf("File %s not opened.\n", filename); return -1;}
  
  while (1){
    fgets(line, 1000, fp);
    if (feof(fp)) break;
    if (line[0]=='>') printf(">%s\n", newname);
    else printf("%s", line);
  }
  
}
