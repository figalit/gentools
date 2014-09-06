#include <stdio.h>

main(){
  char line[10000];
  char name[1000];
  char pline[10000];
  char pname[1000];
  int flag=1;

  name[0]=0; pname[0]=0;
  
  while (scanf("%s", name)>0){
    fgets(line, 10000, stdin);
    if (strcmp(name, pname)){
      if (pname[0]!=0 && flag)
	printf("%s%s", pname, pline);
      strcpy(pname, name);
      strcpy(pline, line); flag=1;
    }
    else
      flag=0;
  }

  if (pname[0]!=0 && flag)
    printf("%s%s", pname, pline);

}
