#include <stdio.h>
main(){
char name[150];
char seq[150];
char qual[150];
char plus[300];
int i;

 while(scanf("%s\n%s\n%s\n%s\n", name, seq, plus, qual) > 0){
   printf("%s\n", name);
   for (i=0;i<75;i++)
	  printf("%c", seq[i]);
   printf("\n");
   printf("%s\n", plus);
   for (i=0;i<75;i++)
	  printf("%c", qual[i]);
   printf("\n");
   
 }

}
