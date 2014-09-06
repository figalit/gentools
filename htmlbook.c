#include <stdio.h>
#include <string.h>
#include <ctype.h>

main(int argc, char **argv){
  FILE *list=fopen(argv[1],"r");
  FILE *in;
  FILE *out;
  char infile[40];
  int numpairs;
  int percentage;
  int count=0;
  out = fopen("table.html","w");
  fprintf(out,"<html><head><title>Degrees of Divergence</title></head>\n");
  fprintf(out,"<body bgcolor=white text=black>\n");
  fprintf(out,"<table border=0>\n<tr>\n");
  while (fscanf(list, "%s", infile) > 0){
    in = fopen(infile,"r");
    fprintf(out,"<td>\n");
    fprintf(out,"<table border=1>\n<tr><td colspan=\"4\" height=\"35\">"); 
    fprintf(out,"<div align=\"center\"><b>%s</b></div></td></tr>\n",infile);
    fprintf(out,"<tr><td><div align=\"center\"><i>Percentage</i></div></td>\n");
    fprintf(out,"<td><div align=\"center\"><i>NumOfPairs</i></div></td></tr>\n");
    
    while(fscanf(in,"%d %d",&percentage,&numpairs) > 0){
      fprintf(out,"<tr><td><div align=\"center\">%d</div></td>\n",percentage);
      fprintf(out,"<td><div align=\"center\">%d</div></td></tr>\n",numpairs);
    } // while in
    fprintf(out,"</table>\n");
    fprintf(out,"</td>\n");
    fclose(in);
    printf("%s done\n",infile);
    count++;
    if (count % 3 == 0){
      fprintf(out,"</tr><tr></tr><tr></tr><tr></tr><tr></tr><tr></tr><tr></tr>\n");
      fprintf(out,"<tr></tr><tr></tr><tr></tr><tr></tr><tr></tr><tr></tr>\n");
      if (count != 33)
	fprintf(out,"<tr>\n");
    }
  } // while list
  fprintf(out,"</table>\n");
  fprintf(out,"</body></html>\n");
} // main
