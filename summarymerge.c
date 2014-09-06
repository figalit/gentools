#include <stdio.h>

main(){
  char name[100]; 
  int d0, d1, d2;
  char call[10];
  int u0,u1,u2,r0,r1,r2;

  int sequence_count = 0;
  long total_placed=0;

  u0=0;  u1=0;  u2=0;
  r0=0;  r1=0;  r2=0;

  while (scanf("%s%d%d%d%s", name, &d0, &d1, &d2, call) > 0){
    if (!strcmp(name, "Total"))
      break;
    sequence_count++;
    printf("%s\t%d\t%d\t%d\t", name, d0,d1,d2);
    total_placed += d0+d1+d2;

    if (d0==1){
      fprintf(stdout, "U0\n");
      u0++;
    }
    else if (d0>1){
      fprintf(stdout, "R0\n");
      r0++;
    }
    else if (d1==1){
      fprintf(stdout, "U1\n");
      u1++;
    }
    else if (d1>1){
      fprintf(stdout, "R1\n");
      r1++;
    }
    else if (d2==1){
      fprintf(stdout, "U2\n");
      u2++;
    }
    else if (d2>1){
      fprintf(stdout, "R2\n");
      r2++;
    }
    else
      fprintf(stdout, "NM\n");

  }


  fprintf(stdout, "\n\n\nTotal Reads Placed: %d (%f%%)\n", (u0+u1+u2+r0+r1+r2), (((float)(u0+u1+u2+r0+r1+r2) / (float)sequence_count)*100));
  fprintf(stdout, "Total Map Locations: %ld \n", total_placed);


  fprintf(stdout, "\tU0:\t%d\n\tU1:\t%d\n\tU2:\t%d\n\tR0:\t%d\n\tR1:\t%d\n\tR2:\t%d\n\tNM:\t%d\n", u0, u1, u2, r0, r1, r2, (sequence_count-u0-u1-u2-r0-r1-r2));


}
