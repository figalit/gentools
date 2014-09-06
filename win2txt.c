#include <stdio.h>
#include <math.h>

main(int argc, char **argv){
  FILE *winlog;
  int WS;
  char ch;
  int wincnt;
  int status;
  int index;
  int cnt;
  int i;
  int myid;

	winlog = fopen(argv[1], "r");

  fscanf(winlog, "%d", &WS);
  // pass newline
  fscanf(winlog, "%c", &ch);

  wincnt = pow(4, WS);
  printf("read WS %d\twincnt %d\n", WS, wincnt);

  while (!feof(winlog)){
    status = fread(&index, sizeof(int), 1, winlog);
    if (status == 0)
      break;
    fread(&cnt, sizeof(int), 1, winlog);
    printf("%d\t%d", index,cnt);
    for (i=0;i<cnt;i++){
      fread(&myid, sizeof(int), 1, winlog);
      printf("\t%d", myid);
    }   
    printf("\n");
  }
}
