#include <stdio.h>
main()
{
int i=0;
char c;
while(scanf("%c", &c) >0)
 if (c!='\n' && c!='\r')
  i++;
printf("%d\n",i);
}
