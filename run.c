#include <stdio.h>
main(){
  int i;
  char comm[300]
  for (i=600; i<=6000; i+=600){
    sprintf(comm, "time -o timeout_%d clustalw mask_out.seq.fa.new_%d", i, i);
    system(comm);
  }
}
