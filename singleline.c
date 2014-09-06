#include <stdio.h>
#include <ctype.h>
int main(int argc, char **argv){
 char ch;
 int i;
 int isRNA=0;
 int withName=0;
 char line[1000];
 if (argc == 2)
   if (!strcmp(argv[1], "-rna")) 
     isRNA=1;
   else if (!strcmp(argv[1], "-name")) 
     withName=1;
 i=0;
 while (scanf("%c", &ch)>0){
   if (ch=='>'){
     fgets(line, 1000, stdin);
     if (withName)
       printf(">%s", line);
   }
   if (tolower(ch) == 'a' || tolower(ch) == 'c' || tolower(ch) == 'g' || tolower(ch) == 't' || tolower(ch)=='n'){
     if (isRNA && tolower(ch) == 't') 
       ch='u';
     printf("%c", toupper(ch));
     i++;
   }
 }
 printf("\n");
 return 0;
}
