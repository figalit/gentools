#include <stdio.h>
#include <string.h>

main(){
  int i=0;
  FILE *in;
  FILE *out;
  FILE *out2;
  char ch;
  in = fopen("second.dat", "r");
  while (fscanf(in, "%c", &ch) > 0){
    i++;
    if (i==36001)
      out=fopen("seq1", "w");
    if (i>=36001 && i<=37000)
      fprintf(out, "%c", ch);
    if (i==37000)
      fclose(out);
    if (i==43201)
      out=fopen("seq2", "w");
    if (i>=43201 && i<=44200)
      fprintf(out, "%c", ch);
    if (i==44200)
      fclose(out);
    if (i==56801)
      out=fopen("seq3", "w");
    if (i>=56801 && i<=57800)
      fprintf(out, "%c", ch);
    if (i==57800)
      fclose(out);
    if (i==61601)
      out=fopen("seq4", "w");
    if (i>=61601 && i<=62600)
      fprintf(out, "%c", ch);
    if (i==62600)
      fclose(out);
    if (i==92801)
      out=fopen("seq5", "w");
    if (i>=92801 && i<=93800)
      fprintf(out, "%c", ch);
    if (i==93800)
      fclose(out);
    if (i==93601)
      out2=fopen("seq6", "w");
    if (i>=93601 && i<=94600)
      fprintf(out2, "%c", ch);
    if (i==94600)
      fclose(out2);
    if (i==99201)
      out=fopen("seq7", "w");
    if (i>=99201 && i<=100200)
      fprintf(out, "%c", ch);
    if (i==100200)
      fclose(out);
    if (i==122401)
      out=fopen("seq8", "w");
    if (i>=122401 && i<=123400)
      fprintf(out, "%c", ch);
    if (i==123400)
      fclose(out);
  }
}
