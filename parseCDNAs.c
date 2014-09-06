#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
main(){
  char ch;
  int i,j;
  int status=1;
  int coord1;
  int coord2;
  char *coords;
  char loc[30];
  int isComplement;
  coords = (char *)malloc(30);
  while (scanf("%c", &ch) > 0){
    
    switch(status){
    case 1:
      if(ch == 'g')
	status=2;
      break;
      
    case 2:
      if(ch == 'e')
	status=3;
      else
	status=1;
      break;
    case 3:
      if(ch == 'n')
	status=4;
      else
	status=1;
      break;
    case 4:
      if(ch == 'e')
	status=5;
      else
	status=1;
      break;
    case 5:
      if(ch == '<'){
	// accept state
	isComplement=0;
	scanf("%s", coords); // pass </a>
	scanf("%s", coords); // herearethe coords
	i=0;j=0;
	if (strstr(coords, "complement")) isComplement=1;
	while(coords[i] != '\0'){
	  if (isdigit(coords[i]))
	    loc[j++]=coords[i];
	  else if (coords[i] == '.'){
	    loc[j] = 0;
	    coord1 = atoi(loc);
	    i++; // pass 2nd dot
	    j=0;
	  }
	  i++;
	}
	loc[j] = 0;
	coord2 = atoi(loc);
	if (coord1!=0 && coord2!=0 && coord1<INT_MAX&&coord2<INT_MAX &&coord1>INT_MIN&& coord2>INT_MIN)
	  printf("%d %d %d\n", coord1, coord2, isComplement);
      }
      else
	status=1;
     break;
      
    }

  }
}
