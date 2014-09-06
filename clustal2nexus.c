#include <stdio.h>
#include <string.h>
#include <ctype.h>

main(){
  char ch=0;
  char seqs[1000][1000];
  int seqcnt=0;
  int seqtot=0;
  int len;
  int i=0; int k;
  int numseq=0;
  FILE *out;
  FILE *in;
  char fname[25];
  char outfname[25];
  int charcnt=0;
  char buf[10];
  while (scanf("%s",fname) > 0){
    seqtot=0;
    len=0;
    printf("%s \n",fname);
    for (i=0;i<1000;i++)
      seqs[i][0]=0;
    in = fopen(fname,"r");
    sprintf(outfname,"%s.nexus",fname);
    while (!isdigit(ch))
      fscanf(in,"%c",&ch);
    while (fscanf(in,"%c",&ch) > 0){
      
      k=0;
      //while (ch=='\n' || ch=='\r')
      while (!isdigit(ch) && fscanf(in,"%c",&ch)>0 )
	;
      if(isdigit(ch)){
	while(ch!='-')
	  fscanf(in,"%c",&ch);;
	fscanf(in,"%c",&ch);
	while(ch!='-')
	  fscanf(in,"%c",&ch);
	fscanf(in,"%c",&ch);
 	while(ch!='-'){
	  buf[k++]=ch;
	  fscanf(in,"%c",&ch);
	}
	buf[k]=0;
      }
	/*
      else if (ch==' ' || ch=='*'){
	while (ch!='\n' && ch!='\r' && fscanf(in,"%c",&ch)>0)
	  ;
	if (fscanf(in,"%c",&ch) > 0)
	  ;
	while(ch!='-'){
	  buf[k++]=ch;
	  fscanf(in,"%c",&ch);
	}
	buf[k]=0;	
	} */
      while(ch!=' ' && fscanf(in,"%c",&ch) > 0)
	;
      while(ch==' ' && fscanf(in,"%c",&ch) > 0)
	;
      seqcnt=atoi(buf);//%1000;
      printf("scnt: %d\n",seqcnt);
      if (seqcnt > seqtot)
	seqtot = seqcnt;
      i=strlen(seqs[seqcnt]);
      if (ch!='\n' && ch!='\r')
	seqs[seqcnt][i++]=ch;
      
      while((ch!='\n' && ch!='\r' && ch!=' ') && fscanf(in,"%c",&ch) > 0)
	if (ch!='\n' && ch!='\r' && ch!=' '){
	   seqs[seqcnt][i++]=ch; 
	}
	else{
           seqs[seqcnt][i++]=0;    
	   break;
	}
//      printf("seq: %d str: %s\n",seqcnt,seqs[seqcnt]);
  //    getchar();
    }
    out = fopen(outfname,"w");
    fprintf(out,"#NEXUS\n");
    
    len=strlen(seqs[0]);
    numseq=0;
    for (i=0;i<1000;i++)
      if (strlen(seqs[i]) > 0){
	//len = strlen(seqs[i]);
	numseq++;
      }
    fprintf(out,"\n\nbegin data;\n");
    fprintf(out,"\tdimensions ntax=%d nchar=%d;\n",numseq,len);
    fprintf(out,"\tformat datatype=DNA interleave gap=-;\n");
    fprintf(out,"\tmatrix\n");   
    i=0;
    while (i<1000){
      if (strlen(seqs[i]) > 0){
	if (i==seqtot)
	  fprintf(out,"\ttax%d %s;\n",i,seqs[i]);
	else
	  fprintf(out,"\ttax%d %s\n",i,seqs[i]);
      }
      i++;
    }
    fprintf(out,"end;\n");
    fprintf(out,"set criterion=distance;\n");
    fprintf(out,"bootstrap search=nj nreps=500 treefile=%s.bootstrap;\n",outfname);
    fprintf(out,"savetrees file=%s.out format=phylip;\n",outfname);
    fprintf(out,"quit warntsave=no;\n");
    fclose(out);
    fclose(in);
    printf("done\n");
  }
}
