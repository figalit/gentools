#include <stdio.h>

int main(int argc, char **argv){
  char infile[100];
  char outfile[100];
  int i,j;
  FILE *in;
  FILE *out;
  char name[100];
  char ch;
  int nocompl=0;
  char seqbuf[5000];
  if (argc < 5)
    return 0;
  infile[0]=0;
  outfile[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-nocomplement"))
      nocompl=1;
    
  }

  in = fopen(infile, "r");
  if (in==NULL)
    return 0;

  out = fopen(outfile, "w");
  if (out==NULL)
    return 0;

  seqbuf[0]=0;
  while(fscanf(in, "%c", &ch) > 0){
    if (ch=='>'){
      seqbuf[i]=0; j=0;
      for (i=strlen(seqbuf)-1;i>=0;i--){
	//if (j % 60 == 0)
	//fprintf(out, "\n");
	j++;
	if (nocompl)
	  fprintf(out, "%c", seqbuf[i]);
	else{
	  switch(toupper(seqbuf[i])){
	  case 'A':
	    fprintf(out,"T");
	    break;
	  case 'C':
	    fprintf(out,"G");
	    break;
	  case 'G':
	    fprintf(out,"C");
	    break;
	  case 'T':
	    fprintf(out,"A");
	    break;
	  default:
	    fprintf(out,"%c",seqbuf[i]);
	    break;	  
	  }
	}
      }
      fprintf(out, "\n");
      fgets(name, 100, in);
      fprintf(out, ">%s", name);	
      i=0;
    }
    else if (!isspace(ch))
	seqbuf[i++]=ch;
  }
  
  seqbuf[i]=0; j=0;
  for (i=strlen(seqbuf)-1;i>=0;i--){
    //if (j % 60 == 0)
    //fprintf(out, "\n");
    j++;
    if (nocompl)
      fprintf(out, "%c", seqbuf[i]);
    else{
      switch(toupper(seqbuf[i])){
      case 'A':
	fprintf(out,"T");
	break;
      case 'C':
	fprintf(out,"G");
	break;
      case 'G':
	fprintf(out,"C");
	break;
      case 'T':
	fprintf(out,"A");
	break;
      default:
	fprintf(out,"%c",seqbuf[i]);
	break;	  
      }
    }
  }
  
  fprintf(out, "\n");

  return 1;
}
