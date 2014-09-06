#include <stdio.h>
#include <zlib.h>

void rcomp(char *, char *);
void reverse(char *, char *);

int main(int argc, char **argv){
  
  FILE *f1, *f2;

  int i;
  char fname[100]; char rname[100];
  int addsuff=0;
  int rcmp=0; 
  int rev=0;

  char s1[100], s2[100], n1[100], n2[100];
  char q1[100], q2[100], p1[100], p2[100];

  char n2rc[100];
  int GZ = 0;
  int fastq = 0;
  int fastqout = 1;
  int fixbgi=0;
  int fixeas=0;
  int secondstr=0;
  char line[100];

  fname[0]=0;
  rname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-f"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-r"))
      strcpy(rname, argv[i+1]);
    else if (!strcmp(argv[i], "-s"))    
      addsuff = 1;
    else if (!strcmp(argv[i], "-rc"))    
      rcmp = 1;
    else if (!strcmp(argv[i], "-rev"))    
      rev = 1;
    else if (!strcmp(argv[i], "-gz"))    
      GZ = 1;
    else if (!strcmp(argv[i], "-fo"))    
      fastqout = 0;
    else if (!strcmp(argv[i], "-bgi"))    
      fixbgi = 1;
    else if (!strcmp(argv[i], "-eas"))    
      fixeas = 1;
    else if (!strcmp(argv[i], "-sstr"))    
      secondstr = 1;
  }  

  if (fname[0]==0 || rname[0]==0)
    return;


  if (!GZ){
    f1=fopen(fname, "r");
    f2=fopen(rname, "r");
  }
  else{
    f1=gzopen(fname, "r");
    f2=gzopen(rname, "r");
  }

  if (f1==NULL || f2 == NULL)
    return;


  if (!GZ){
    while (1){
     
      if (!(fscanf(f1, "%s", s1) > 0 && fscanf(f2, "%s", s2) > 0))
	break;
      
      if (s1[0] == '@')
	fastq = 1;

      if (secondstr){
	fscanf(f1, "%s", s1);
	fscanf(f2, "%s", s2);
	fgets(line, 100, f1);
	fgets(line, 100, f2);
      }
      
      /*      
	      fscanf(f1, "%s\n%s\n", s1, n1) > 0;
	      fscanf(f2, "%s\n%s\n", s2, n2); */
      
      
      fscanf(f1, "%s\n", n1);
      fscanf(f2, "%s\n", n2);
      
      if (fastq){

	fgets(line, 100, f1);
	fgets(line, 100, f2);

	fscanf(f1, "%s\n", q1);
	fscanf(f2, "%s\n", q2);
      }

      if (rcmp){
	rcomp(n2, n2rc);
	strcpy(n2, n2rc);
      }
      else if (rev){
	reverse(n2, n2rc);
	strcpy(n2, n2rc);
      }
      if (!addsuff){
	printf("%s\n%s\n", s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	printf("%s\n%s\n", s2, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }
      else{
	printf("%s/1\n%s\n", s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	printf("%s/2\n%s\n", s2, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }
    }
  }
  
  else{
    while (!gzeof(f1) && !gzeof(f2)) {
      gzgets(f1, s1, 100);
      if (gzeof(f1) || gzeof(f2))
	break;
      
      gzgets(f1, n1, 100);
      gzgets(f2, s2, 100);
      gzgets(f2, n2, 100);
    
      if (s1[0] == '@')
	fastq = 1;

      s1[strlen(s1)-1] = 0;
      s2[strlen(s2)-1] = 0;
      n1[strlen(n1)-1] = 0;
      n2[strlen(n2)-1] = 0;

      if (fastq){
	gzgets(f1, p1, 100);
	gzgets(f1, q1, 100);
	gzgets(f2, p2, 100);
	gzgets(f2, q2, 100);  
	p1[strlen(p1)-1] = 0;
	p2[strlen(p2)-1] = 0;
	q1[strlen(q1)-1] = 0;
	q2[strlen(q2)-1] = 0;
      }

      
      if (rcmp){
	rcomp(n2, n2rc);
	strcpy(n2, n2rc);
      }
      else if (rev){
	reverse(n2, n2rc);
	strcpy(n2, n2rc);
      }

      if (!addsuff){
	printf("%s\n%s\n", s1, n1);
	if (fastq && fastqout)
	  printf("%s\n%s\n", p1, q1);
	printf("%s\n%s\n", s2, n2);
	if (fastq && fastqout)
	  printf("%s\n%s\n", p2, q2);
      }
      else{
	printf("%s/1\n%s\n", s1, n1);
	if (fastq && fastqout)
	  printf("%s\n%s\n", p1, q1);
	printf("%s/2\n%s\n", s2, n2);
	if (fastq && fastqout)
	  printf("%s\n%s\n", p2, q2);
      }

    }
  }

  return 1;
}



void rcomp(char *window, char *rwin){
  /* reverse complement */
  int i;
  int len = strlen(window);
  for (i=0;i<len;i++)
    switch (toupper(window[i])){
    case 'A':
      (rwin)[len-i-1] = 'T';
      break;
    case 'C':
      (rwin)[len-i-1] = 'G';
      break;
    case 'G':
      (rwin)[len-i-1] = 'C';
      break;
    case 'T':
      (rwin)[len-i-1] = 'A';
      break;
    default:
      (rwin)[len-i-1] = window[i];
      break;
    }
  (rwin)[len] = 0;
}

void reverse(char *window, char *rwin){
  /* reverse complement */
  int i;
  int len = strlen(window);
  for (i=0;i<len;i++)
    (rwin)[len-i-1] = window[i];
  
  (rwin)[len] = 0;
}
