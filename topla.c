#include <stdio.h>

main(){
  long long num;
  char line[100];
  long long total=0;
	/*
	while (scanf("%s", line)>0)
	  total += (long long)atoi(line);
	*/
	while (scanf("%lld", &num)>0)
	  total += (long long)num;

	printf("total: %lld\n",total);
}
