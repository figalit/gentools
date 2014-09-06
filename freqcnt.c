#include <stdio.h>

int frequencies[4000];

main(){
  int cnt=1;
  int freq;
  int exfreq=0;
  int i;
  for (i=0;i<4000;i++)
    frequencies[i]=0;
  while (scanf("%d", &freq)>0){
    if (freq==exfreq)
      cnt++;
    else{
      if (exfreq!=0)
	printf("%d %d\n", exfreq, cnt);
      //frequencies[exfreq]=cnt;
      cnt=1;
      exfreq=freq;
    }
  }
  /*
  for (i=1;i<4000;i++)
    printf("%d\t%d\n", i, frequencies[i]);
  */
}
