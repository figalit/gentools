#include <stdio.h>

main(){
  char fname[100]; char fseq[100]; char fqual[100];
  char rname[100]; char rseq[100]; char rqual[100];
  int i; char rc;
  while (scanf("@%s\n%s\n+\n%s\n@%s\n%s\n+\n%s\n", fname, fseq, fqual, rname, rseq, rqual) > 0){
    printf("@%s\n%s\n+\n%s\n", fname, fseq, fqual);
    printf("@%s\n", rname);
    for (i=strlen(rseq)-1;i>=0;i--){
      switch(toupper(rseq[i])){
      case 'A': rc='T'; break;      
      case 'C': rc='G'; break;
      case 'G': rc='C'; break;
      case 'T': rc='A'; break;
      default: 
	rc = rseq[i]; break;
      }
      printf("%c", rc);
    }
    printf("\n+\n%s\n", rqual);
  }
}
