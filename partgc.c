#include <stdio.h>

int main(){
  float gc;
  float totgc;
  int cnt;
  int freq;
  int i;
  int k;

  i=100;
  totgc=0.0; freq=0;
  k=0;
  while (scanf("%d\t%f\n", &cnt, &gc) > 0){
    if (cnt / i <= k){
      totgc = totgc + gc;
      freq++;
      //fprintf(stderr, "increment %d\n", freq);
    }
    else{
      if (freq!=0)
	//printf("%d\t%d\t%d\t%f\t%f\n", k*i, (k+1)*i, freq, totgc, totgc/(float)freq);
	printf("\"%d-%d\"\t%f\n", k*i, (k+1)*i, totgc/(float)freq);

      
      k++;

      if (k==10) {
	freq=0; i=i*10;
	totgc = 0.0 ; k=0;
	fprintf(stderr, "i is now %d\n", i);
      }
    }
  }
  if (freq!=0)
    //printf("%d\t%d\t%d\t%f\t%f\n", k*i, (k+1)*i, freq, totgc, totgc/(float)freq);
    printf("\"%d-%d\"\t%f\n", k*i, (k+1)*i, totgc/(float)freq);
}
