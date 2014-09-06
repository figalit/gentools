#include <stdio.h>

main(int argc, char **argv){
  char ch;
  char name[100];
  int i;
  char fname[100];
  char seqname[100];
  int offset;
  int start;
  int end;
  char pch=0;

  for (i=0;i<argc;i++){
    if (!strcmp(argv[i], "-s"))
      strcpy(seqname, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      offset = atoi(argv[i+1]) - 1;
  }

  
  start = -1; end = -1;
  i=0;
  offset = 0;

  while(scanf("%c", &ch) > 0){
    if (ch == '>'){
      fgets(name, 100, stdin);
      name[strlen(name)-1] = 0;
      i = 0;
      continue;
    }
    else if (isspace(ch))
      continue;
    i++;
    if (ch == 'N'){
      if (pch!='N'){
	start=i+offset;
	if (start == -1) start = 0;
	printf("%s\t%d\t", name, start-1);
	start = -2;
      }
      
    }
    else{
      if (pch=='N'){
	end=i+offset-1;
	start = -1;
	printf("%d\n", end);
      }
    }
    pch=ch; 
  }
  
  if  (start == -2){
    end=i-1+offset;
    printf("%d\n", end);
  }
}
