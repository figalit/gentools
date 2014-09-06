#include "stdio.h"
#include "assert.h"

void decode_window(int, char *);
char reverseindex(int);

main(){
  int code;
  char str[100];
  char decoded[20];

  scanf("%s", str); // graph
  printf("%s ", str);
  scanf("%s", str); // G{
  printf("%s\n", str);
  printf("\tgraph\t[fontsize=8];\n");
  printf("\tedge\t[fontsize=8];\n");
  printf("\tnode\t[fontsize=8];\n");

  str[0]=0;
  while (str[0]!='}'){
    if (!strcmp(str, "--")){
      printf("\t--\t"); 
    }
    else if (strstr(str, ";")){
      str[strlen(str)-1]=0;
      code = atoi(str);
      decode_window(code, decoded);
      printf("%s;\n", decoded);
    }
    else if (isdigit(str[0])){
      code = atoi(str);
      decode_window(code, decoded);
      printf("\t%s", decoded);
    }
    scanf("%s", str);
  }

  printf("}\n");
  
}


void decode_window(int code, char *window){
  int i;
  int shiftleft, shiftright;
  unsigned int left, right;
  int totalbits = sizeof(int)*8;
  int WS = 12;

  for (i=0;i<WS;i++){
    shiftleft = totalbits - 2*WS + 2*i;
    shiftright = totalbits - 2; 
    left = code << shiftleft;
    right = left >> shiftright;
    window[i] = reverseindex(right);
  }
  window[WS]=0;
}

char reverseindex(int code){
  char ch;
  switch(code){
  case 0:
    ch = 'A';
    break;
  case 1:
    ch = 'C';
    break;
  case 2:
    ch = 'G';
    break;
  case 3:
    ch = 'T';
    break;
  default:
    printf("alooooooooooooooooo %d\n", code);
    assert(0);
    break;
  }
  return ch;
}
