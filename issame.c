#include <stdio.h>
#include <string.h>
#include <ctype.h>
main(int argc, char **argv){
  FILE *one=fopen(argv[1],"r");
  FILE *two=fopen(argv[2],"r");
  int arone[1000];
  int artwo[1000];
  int i;
  char ch;
  char str[1000];
  char *token;
  for (i=0;i<1000;i++){
    arone[i]=-1;
    artwo[i]=-1;
  }
  i=0;
  while (fscanf(one,"%s",str) > 0){
    if (strstr(str,argv[3])){
      token=strtok(str,"-"); // skip 17... number
      token=strtok(NULL,"-"); // skip argv[3]
      token=strtok(NULL,"-"); // number
      arone[i++]=atoi(token);
    }
  }
  i=0;
  while (fscanf(two,"%s",str) > 0){
    if (strstr(str,argv[3])){
      token=strtok(str,"-"); // skip 17... number
      token=strtok(NULL,"-"); // skip argv[3]
      token=strtok(NULL,"-"); // number
      artwo[i++]=atoi(token);
    }
  }
  for (i=0;i<1000;i++){
    if (arone[i] != artwo[i]){
      printf("nope\n");
      exit(0);
    }
    //if (arone[i]!=-1)
    //printf("%d %d\n",arone[i],artwo[i]);
  }
  printf("same\n");
} // main
