/*  
    Opens "mask_out.seq.fa" and gropus sequences
    in files with different accession numbers
 */

#include <stdio.h>
#include <string.h>

main(){
  FILE *in=fopen("mask_out.seq.fa", "r");
  FILE *out=fopen("dummy","w");
  char ch;
  char current[20];
  char this[20];
  char seq[400];
  int i,j;
  int first=0;

  strcpy(current,"");
  
  this[0] = 0;
  i=0; j=0;
  //fscanf(in,"%c",&ch);

  while(1){
    //seq[0]='>';    

    while (ch != '-'){
      if (fscanf(in,"%c",&ch) > 0 )
	seq[i++]=ch;      
      else
	break;
      //printf("%c",ch);
      
    }
    ch=0;
    while (ch != '-'){
      fscanf(in,"%c",&ch);
      if (ch != '-')
	this[j++]=ch;
      seq[i++]=ch;
    }
    while (ch != '>'){
      if (fscanf(in,"%c",&ch) > 0 ){
	if (ch!='>')
	  seq[i++]=ch;
      }
      else{
	fprintf(out,">%s",seq);
	fclose(out);
	break;
      }
    }
    seq[i]=0;
    if (strcmp(this,current)){
      fclose(out);
      first = 0;
      out = fopen(this,"w");
      printf("generate: %s\n",this);
      strcpy(current,this);
    }
    else
      first=1;
    
    fprintf(out,">%s",seq);

    i=0;j=0; 
  } // while
  fclose(out);
  system("rm -f dummy");
} // main


