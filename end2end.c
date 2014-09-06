#include <stdio.h>

int main(){
  char clone[120];
  int i,j;
  char newclone[121];
  char newrclone[121];
  int flag;
  while (scanf("%s", clone) > 0){
    j = 0;
    flag = 0;


    if (strstr(clone, "G248") || strstr(clone, "G630")){

      for (i=0; i<strlen(clone); i++){
	if ((clone[i] != 'F' && clone[i] != 'R') || flag){
	  newclone[j++] = clone[i];  
	}
	else if (clone[i] != 'F' || clone[i] != 'R'){
	  if (flag)
	    newclone[j++] = clone[i];  
	  else{
	    if (clone[i] == 'F')
	      newclone[j++] = 'R'; 
	    else if (clone[i] == 'R')
	      newclone[j++] = 'F';  
	    flag = 1;
	  }
	} 
	
      }
      newclone[j] = 0;
    }
    
    else if (strstr(clone, "FORWARD") || strstr(clone, "REVERSE")){
      i=0; j=0;
      while(clone[i] != '.')
	newclone[j++] = clone[i++];
      // here clone[i] is .

      newclone[j++] = clone[i++];
      newclone[j]=0;
      if (strstr(clone, "FORWARD"))
	strcat(newclone, "REVERSE");
      else
	strcat(newclone, "FORWARD");
      j = j+7;

      while(clone[i] != '.') i++;
      while (i<strlen(clone))
      newclone[j++] = clone[i++];
      newclone[j] = 0;

    }
    else {
      strcpy(newclone, clone);
      newclone[strlen(clone)-4] = 0;
    }

    printf("%s\n", newclone);  

  }

  return 1;
}
