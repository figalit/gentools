#include <stdio.h>

int main(int argc, char **argv){
  int i;
  char state;
  char ch;
  int count;
  int len;

  if (argc != 2){
    fprintf (stderr, "%s <raw string>\n", argv[0]);
    return -1;
  }

  len = strlen(argv[1]);
  if (len==0){
    fprintf (stderr, "Zero-length string.\n");
    return -1;
  }


  printf("raw:\t%s\ncigar:\t", argv[1]);
  state = 0;
  i = 0;
  count = 0;

  for (i=0;i<len;i++){
    ch = argv[1][i];
    if (ch==state)
      count++;
    else {
      if (state!=0)
	printf("%d%c", count, state);
      state = ch; count = 1;
    }
  }
  
  if (state!=0)
    printf("%d%c", count, state);

  printf ("\n");
}
