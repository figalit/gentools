/*
ERAY TUZUN
ext29@eecs.cwru.edu
ClustaltoMega V 1.1
This program converts from clustalw format to Mega format.
07/08/2001

*/
#include <stdio.h>
#include <string.h>

  struct _acc{
    char *filename;

  };
typedef struct _acc accession;
accession file[1000];

int MaxFileNo=1;
FILE *gfopen(char *, char *);
int findstring(char *);
void SkipNlines(int,FILE *);
int main(int argc, char **argv){

  char temp[20];
  FILE *in;
  FILE *out1;
  FILE *out[1000];
  int cnt;
  
  int i=1;
  int fcount=1;
  char line[150];
  char infile[50]; //accno.aln files
  char ch;
  int maxsize=0;   
  int j;
  char command[50];

  if (argc!=3)
    {
      printf("Usage: ctom *.aln outputfile \n");
      return 0; 
    }


 in=gfopen(argv[1],"r");
 out1=gfopen(argv[2],"w");

  SkipNlines(3,in);
 
   //Preprocessing step finds the number of sequences and their names 
  while(fscanf(in,"%s",line) > 0)
    {
         
         if ((i % 2)==1) 
	 {
	   if (findstring(line)==0)
	     {
	       file[MaxFileNo].filename=(char *)malloc(50);
	       sprintf(file[MaxFileNo].filename,"%s",line);
	       //printf("%s\n",file[MaxFileNo].filename);
	       sprintf(infile,"%s.alnd",file[MaxFileNo].filename);
	       out[MaxFileNo]=fopen(infile,"w+"); 
	       // fprintf(out[MaxFileNo],">%s\n",file[MaxFileNo].filename);
	       MaxFileNo++;
	       if (sizeof(file[i].filename)>maxsize)
		 maxsize=sizeof(file[i].filename);
	     }
	   else
	     break;
	 }
	 i++;
    }
  maxsize=maxsize+7;
  //printf("max size is %d\n",maxsize);
  //printf("Preprocessing step done\n");
  //Seeks to beginning of file and starts to form the sequences
  fseek(in,0,0);
  SkipNlines(3,in);
  i=1;


 
   while(fscanf(in,"%s",line) > 0)
    {
      
       if ((i % 2)==0) 
	   fprintf(out[fcount++],"%s",line);
	 i++;
	 if (fcount==(MaxFileNo))
	   {
	     SkipNlines(2,in);
	     fcount=1;
	     continue;
	   }
	
    }  


   fclose(in);

   //write the header of mega file..
   fprintf(out1,"#mega \n");
   fprintf(out1,"TITLE: %s \n \n", argv[1]);
 

   for(i=1;i<MaxFileNo;i++)
     {
       cnt=0;
       fseek(out[i],0,0);
       fprintf(out1,"\n#%s\n",file[i].filename);
       /*
       for (j=1;j<maxsize-sizeof(file[i].filename);j++)
	 fprintf(out1," ");
       */
       while(fscanf(out[i],"%c",&ch)>0){
	 fprintf(out1,"%c",ch);
	 cnt++;
	 if (cnt % 50 == 0)
	   fprintf(out1,"\r");
       }
       fprintf(out1,"\n");
     }
   for (i=1;i<MaxFileNo;i++)
     fclose(out[i]);
   fclose(out1);
   
   sprintf(command,"rm *.alnd");
   system(command);
   

}

int findstring(char *input)
{
  int i;

  for (i=1;i<MaxFileNo;i++)
    if (strcmp(input,file[i].filename)==0)
		   return 1;
  //Exceptional Casess....
  if (input[0]=='*')
      return 1;

  return 0; //No match found
}

void SkipNlines(int n,FILE *in)

{
  char ch;
  int linecount=0;
  //Skips first n lines
  while(fscanf(in,"%c",&ch) > 0)
    {

      if (ch=='\n')
        linecount++;
      if (linecount==n)
	break;
    }

}

FILE *gfopen(char *fname, char *mode)
{

  FILE *fp;
  if ((fp=fopen(fname,mode))==0)
    {
      printf("Cannot open %s \n",fname);
      exit(1);

    }
  return fp;
}



