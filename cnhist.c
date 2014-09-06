#include <stdio.h>
#include <math.h>

int neg[500];
int pos[500];

main(){
  float cn; int bases;
  int i;
  memset(neg, 0, 500*sizeof(int));
  memset(pos, 0, 500*sizeof(int));
  while(scanf("%f\t%d\n", &cn, &bases) > 0){
    if (cn >= 0.0)
      pos [ (int)(round(cn *10) ) ] += bases;
    else if ( round((-1*cn) *10) == 0)
      pos[0] +=bases;
    else
      neg [ (int)(round((-1*cn) *10) ) ] += bases;

  }

  for (i=1;i<500;i++){
    if (neg[i] != 0)
      printf ("%f\t%d\n", (-1*(float)i/10.0), neg[i]);
  }

  for (i=0;i<500;i++){
    if (pos[i] != 0)
      printf ("%f\t%d\n", ((float)i/10.0), pos[i]);
  }

}
