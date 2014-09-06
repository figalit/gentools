#include <stdio.h>

#define MAX 250000000
char seq[MAX];

int main(int argc, char **argv){
  char ch;
  int i;
  char name[100];
  char dummy[100];
  int len;
  int chrlen;
  int s, e;
  char infile[100];
  FILE *in;
  
  FILE *out;

  char outfile[100];


  strcpy(infile, argv[1]);
  in = fopen(infile, "r");


  i=0;
 
  fscanf(in, ">%s", name);
  fgets(dummy, 100, in);

  while(fscanf(in, "%c", &ch) > 0){
    
    if (!isspace(ch)) seq[i++]=ch;

  }
  
  seq[i]=0;
  chrlen = i;

  sprintf(outfile, "%s.copynumber", infile);
  out  = fopen(outfile, "w");

  s=0;e=0;
  len = 0;


  /* copynumber */
  for (i=0;i<chrlen;i++){
    //fprintf (stderr, "\rWriting copynumber\t%5.2f", 100 * ((float)i/(float)chrlen));
    if (seq[i]!='N' && seq[i]!='X') len++;
    if ((len == 1000 || seq[i]=='X') && len !=0){
      e = i;
      if (seq[i]=='X' && len < 1000)
	;
      else
	fprintf(out, "%s\t%d\t%d\n", name, s, e);
      s = i+1;
      len = 0;
    }
    if (seq[i]=='X') s = i+1;
		       
  }

  //  fprintf(stderr, "\n");

  /*
  if (len != 1000 && len!=0 && s!=chrlen){
    len = 0;
    e = chrlen-1;
    fprintf(out, "%s\t%d\t%d\n", name, s, e);
    s = i+1;
    }*/

  fclose(out);


  /* wssd */
  sprintf(outfile, "%s.wssd", infile);
  out  = fopen(outfile, "w");

  s=0;e=0;
  len = 0;
  
  i=0;
  //  for (i=0;i<chrlen;i++){
  while (i<chrlen){
    //fprintf (stderr, "\rWriting wssd\t%5.2f", 100 * ((float)i/(float)chrlen));
    if (seq[i]!='N' && seq[i]!='X') len++;
    if ((len == 1000 || seq[i]=='X') && len !=0){
      e = i;
      if (seq[i]=='X' && len < 1000)
	;
      else
	fprintf(out, "%s\t%d\t%d\n", name, s, e);
      s = s+1000;
      i=s-1;
      len = 0;
    }
    if (seq[i]=='X'){ s = i+1; i = s-1;}

    i++;
   		       
  }

  /*
  if (len != 1000 && len!=0 && s!=chrlen){
    len = 0;
    e = chrlen-1;
    fprintf(out, "%s\t%d\t%d\n", name, s, e);
    s = i+1;
    }*/

  fclose(out);
  //  fprintf(stderr, "\n");

  /* coverage */

  sprintf(outfile, "%s.coverage", infile);
  out  = fopen(outfile, "w");

  s=0;e=0;
  len = 0;
  i=0;


  while (i<chrlen){
    //  for (i=0;i<chrlen;i++){
    //    fprintf (stderr, "\rWriting coverage\t%5.2f", 100 * ((float)i/(float)chrlen));
    
    if (seq[i]!='N' && seq[i]!='X') len++;
    if ((len == 5000 || seq[i]=='X') && len !=0){
      e = i;
      if (seq[i]=='X' && len < 5000)
	;
      else
	fprintf(out, "%s\t%d\t%d\n", name, s, e);
      s = s+1000;
      i=s-1;
      len = 0;
    }
    if (seq[i]=='X') {s = i+1; i=s-1;}
		       
    i++;
  }

  /*
  if (len != 5000 && len!=0 && s!=chrlen){
    len = 0;
    e = chrlen-1;
    fprintf(out, "%s\t%d\t%d\n", name, s, e);
    s = i+1;
    }*/

  //  fprintf(stderr, "\n");

  fclose(out);

}
