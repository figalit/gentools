#include <stdio.h>
#include <math.h>


main(){

  float diff;
  int bases;
  int xintdiff=100;
  int intdiff;

  float xdiff=100;
  long long total=0;

  while (scanf("%f%d", &diff, &bases) > 0){

    intdiff = round(diff);
    //    if (diff!=xdiff){
    if (intdiff!=xintdiff){
      if (total!=0)
	//printf("%f\t%lld\n", xdiff, total);
	printf("%d\t%lld\n", xintdiff, total);
      total = bases;
      xintdiff = intdiff;
    }
    else
      total+=bases;

  }

  if (total!=0)
    printf("%d\t%lld\n", xintdiff, total);

}
