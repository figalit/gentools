#include <stdio.h>
main(){
  FILE *out;
  char fname[20];
  char dumseq[100];
  int isopen=0;
  while (scanf("%s", dumseq) > 0){
    if (dumseq[0] == '_'){
      if (isopen)
	fclose(out);
      scanf("%s", dumseq);
      out = fopen(dumseq, "w");
      isopen = 1;
    }
    else{
      fprintf(out, "%s\n", dumseq);
    }
  }
}
