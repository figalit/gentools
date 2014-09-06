#include <stdio.h>

#define MAX 1000000

static int maxfunc(const void *, const void *);

int nums[MAX];

int main(){
  int num;
  int cnt;
  int total=0;
  int newtotal;
  int i;
  
  cnt = 0;
  
  while(fscanf(stdin, "%d", &num) > 0){
    nums[cnt++] = num;
    total+=num;
  }
  fprintf(stderr, "%d nums read.\n", cnt);

  qsort(nums, cnt, sizeof(int), maxfunc);

  newtotal = 0;
  for (i=0;i<cnt;i++){
    newtotal+=nums[i];
    if (newtotal >= total/2){
      printf("N50: %d\n", nums[i]);
      return 0;
    }
  }
  
}


static int maxfunc(const void *p1, const void *p2){
  int a, b;
  a = (int)(*((int *)p1));
  b = (int)(*((int *)p2));
  return (b-a);
}

