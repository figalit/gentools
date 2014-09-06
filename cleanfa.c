#include <stdio.h>
#include <ctype.h>
int main(int argc, char **argv){
 char ch;
 int i;
 int isRNA=0;
 int withName=0;
 char name[1000];
 int nondna=0;
 if (argc > 2){
   for (i=1;i<argc; i++){
   if (!strcmp(argv[i], "-rna")) 
     isRNA=1;
   if (!strcmp(argv[i], "-name")) 
     withName=1;
   if (!strcmp(argv[i], "-nondna"))
     nondna = 1;
   }
 }
 i=0;
 while (scanf("%c", &ch)>0){
   if (ch == '>'){
     fgets(name, 1000, stdin);
     if (withName)
       printf("\n>%s",name);
     i=0;
   }
   if (!nondna){
     if (tolower(ch) == 'a' || tolower(ch) == 'c' || tolower(ch) == 'g' || tolower(ch) == 't'){
       if (isRNA && tolower(ch) == 't') 
	 ch='U';
       printf("%c", toupper(ch));
       i++;
       if (i%60 == 0)
	 printf("\n");
     }
   }
   else{
     if (isalpha(ch)){
       if (isRNA && tolower(ch) == 't') 
	 ch='U';
       printf("%c", toupper(ch));
       i++;
       if (i%60 == 0)
	 printf("\n");
     }
   }
 }
 return 0;
}
