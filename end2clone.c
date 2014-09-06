#include <stdio.h>

void end2clone(char *, char *);

int main(){
  char clone[200];
  int i,j;
  char newclone[201];
  char newrclone[201];
  int flag;

  //while (scanf("%s", clone) > 0){
  while (fgets(clone, 200, stdin) > 0){
    j = 0;
    flag = 0;
    clone[strlen(clone)-1] = 0;
    end2clone(newclone, clone);
    printf("%s\n", newclone);  

  }

  return 1;
}


void end2clone(char *clone, char *end){
  int i, len, j;
  int flag = 0;
  char *rloc, *floc;
  char *pass;
  
  len = strlen(end);

  rloc = strstr(end, "REVERSE");
  floc = strstr(end, "FORWARD");
  
  if (floc!=NULL || rloc!=NULL){
    
    if (rloc != NULL)
      pass = rloc;
    else
      pass = floc;
    
    i = 0; j=0;
    while (end+i+1 != pass){
      clone[j] = end[i];
      i++; j++;
    }
    // we're at the '.' before REVERSE/FORWARD
    // pass that
    i++;
    // find the '.' after REVERSE/FORWARD
    while (end[i]!='.') i++;
    // don't pass that 
    // copy the rest
    while(i<strlen(end))
      clone[j++] = end[i++];
    // terminate
    clone[j] = 0;
    return;
  }


  j=0;
  for (i=0; i<strlen(end); i++){
    if ((end[i] != 'F' && end[i] != 'R') || flag){
      clone[j++] = end[i];  
    }
    else if (end[i] != 'F' || end[i] != 'R'){
      if (flag)
	clone[j++] = end[i];  
      else{
	flag = 1;
      }
    } 
    
  }
  clone[j] = 0;

  //fprintf(stderr, "name:\t%s\tclone:\t%s\n", end, clone);
  
}
