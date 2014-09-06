#include <stdio.h>
main(){
  char name[700];
  char seq[700];
  char qual[500];
  char plus[3];
  int i;
  int limit;
  
 while(scanf("%s\n%s\n", name, seq) > 0){
   printf("%s\n", name);
   limit = 36; 
   if (strlen(seq)<limit) limit = strlen(seq);
   for (i=0;i<limit;i++)
     printf("%c", seq[i]);
   printf("\n");
 }

}
