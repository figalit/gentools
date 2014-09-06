#include <stdio.h>

int main(int argc, char **argv){
  
  int count[1000];
  float totdepth[1000];

  float gc, depth;
  int i, j;


  for (i=0;i<1000;i++) { totdepth[i]=0.0; count[i]=0; }

  while (fscanf(stdin, "%f\t%f\n", &gc, &depth) > 0){
    
    i = gc * 1000;

    count[i]++;
    totdepth[i]+=depth;

    //fprintf(stdout, "%f\t%f\n", (float)i/1000.0, depth);


  }

  for (i=0;i<1000;i++){

    if (count[i]==0 || totdepth[i] == 0){
      j=i-1;
      while (j>=0 && totdepth[j]==0) j--;

      if (totdepth[j]==0 || count[j]==0){
	j=i+1;
	while (j<1000 && totdepth[j]==0) j++;
      }

      totdepth[i]=totdepth[j];
      count[i]=count[j];
    }

    if (totdepth[i] != 0.0)
      //fprintf(stdout, "%d\t%f\t%d\t%f\n", i , totdepth[i], count[i], totdepth[i] / (float) count[i]);
      fprintf(stdout, "%d\t%f\n", i , totdepth[i] / (float) count[i]);
    else
      fprintf(stdout, "%d\t EMPTY\n", i);
  }
    
  
}
