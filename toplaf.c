#include <stdio.h>
#include <stdlib.h>

main(){
	char line[100];
	double total=0.0;
	double thisguy;
	while (scanf("%s", line)>0){
		thisguy = atof(line);
		total = total + thisguy;
	}
	printf("total: %f\n",total);

}
