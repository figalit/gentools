#include <stdio.h>

int main(){
  char clone[100];
  int i,j;
  char newclone[100];
  char newrclone[100];
  int flag;
  int ifdot;
  while (scanf("%s", clone) > 0){
    if (!strstr(clone, "G248") && !strstr(clone, "S213")){
      //printf("%s\n", clone);

      j = strlen(clone)+8; flag = 0;
      newclone[j] = 0 ; newrclone[j] = 0; j--;
      
      if (strstr(clone, ".")) ifdot = 1;
      else ifdot = 0;
      
      for (i=strlen(clone)-1; i>=0 ; i--){
	newclone[j] = clone[i];  newrclone[j] = clone[i]; j--;
	if (clone[i]=='.' && !flag){ // && !flag && !ifdot){
	  flag = 1;
	  newclone[j] = 'D'; 
	  newrclone[j] = 'E'; 
	  j--;
	  newclone[j] = 'R'; 
	  newrclone[j] = 'S'; 
	  j--;
	  newclone[j] = 'A'; 
	  newrclone[j] = 'R'; 
	  j--;
	  newclone[j] = 'W'; 
	  newrclone[j] = 'E'; 
	  j--;
	  newclone[j] = 'R'; 
	  newrclone[j] = 'V'; 
	  j--;
	  newclone[j] = 'O'; 
	  newrclone[j] = 'E'; 
	  j--;
	  newclone[j] = 'F'; 
	  newrclone[j] = 'R'; 
	  j--;
	  newclone[j] = '.'; 
	  newrclone[j] = '.'; 
	  j--;
	}
	//if (clone[i] == '.') ifdot = 0;
      }
      printf("%s\n%s\n", newclone, newrclone); 
    
      continue;
    }

    j = strlen(clone)+1; flag = 0;
    newclone[j] = 0 ; newrclone[j] = 0; j--;

    if (strstr(clone, ".")) ifdot = 1;
    else ifdot = 0;

    for (i=strlen(clone)-1; i>=0 ; i--){
      newclone[j] = clone[i];  newrclone[j] = clone[i]; j--;
      if (isalpha(clone[i]) && !flag && !ifdot){
	flag = 1;
	newclone[j] = 'F'; newrclone[j] = 'R'; j--;
      }
      if (clone[i] == '.') ifdot = 0;
    }
    printf("%s\n%s\n", newclone, newrclone);  
  }

  return 1;
}
