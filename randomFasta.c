#include <time.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv){
  int len;
  int i;
  int r;
  if (argc != 2){
    fprintf(stderr, "%s [length]\n", argv[0]);
    return 0;
  }
  i=0;
  len = atoi(argv[1]);
  if (len <= 0){
    fprintf(stderr, "Invalid length.\n");
    return 0;
  }
  srand(time(NULL));

  printf(">random sequence\n");
  for (i=0;i<len;i++){
    if (i%60 == 0 && i!=0)
      printf("\n");
    r = rand() % 4;
    switch(r){
    case 0:
      printf("A");
      break;
    case 1:
      printf("C");
      break;
    case 2:
      printf("G");
      break;
    case 3:
      printf("T");
      break;
    default:
      assert(0);
      break;
    }
  }
  printf("\n");
  
  return 1;
}
