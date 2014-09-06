#include <stdio.h>

main(int argc, char **argv){
  int length;
  int i;
  char name[100];
  strcpy(name, argv[1]);
  length = atoi(argv[2]);

  for (i=0; i<length-5000; i+=1000){
      printf("%s\t%d\t%d\n", name, i, (i+5000));
  }
  printf("%s\t%d\t%d\n", name, i, length);
}
