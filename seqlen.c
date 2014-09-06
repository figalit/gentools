#include<stdio.h>
int main(int argc, char **argv){
  FILE *in;
  int count=0;
  char ch;
  char firstline[200];

  if (argc != 2){
    fprintf(stderr, "%s [infile]\n", argv[0]);
    return 0;
  }
  in=fopen(argv[1], "r");

  fgets(firstline, 200, in);
  //fscanf(in, "%s", firstline);
  while (fscanf(in, "%c", &ch) >0){
    //if (toupper(ch) == 'A' || toupper(ch) == 'C' || toupper(ch) ==
    //'G' || toupper(ch) == 'T' || toupper(ch) == 'U' || toupper(ch)
    if(isalpha(ch)||ch=='-')
      count++;
  }
  printf("%d\n",count);
  return 1;
}
