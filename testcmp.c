#include <string.h>

main(){
	char str1[1000];
	char str2[1000];
	strcpy(str1, "g1k-sc-HG00553-C-6907");
	strcpy(str2, "g1k-sc-HG00553");
	printf("1/2: %d\n", strcmp(str1, str2));
	printf("2/1: %d\n", strcmp(str2, str1));
}

