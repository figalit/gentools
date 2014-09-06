#include <stdio.h>

main(){
  char name[100];
  char prev[100];
  char rest[200000];
  name[0] = 0;
  prev[0] = 0;

  while(scanf("%s", name) > 0){
    fgets(rest, 200000, stdin);
    if (strcmp(name, prev)){
      printf("%s%s", name, rest);
      strcpy(prev, name);
    }
  }
}
