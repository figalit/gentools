#include <stdio.h>
#include <zlib.h>
#include <string.h>

void rcomp(char *, char *);
void reverse(char *, char *);

int main(int argc, char **argv){
  
  FILE *f1, *f2;

  int i;
  char fname[500]; char rname[500];
  int addsuff=0;
  int rcmp=0; 
  int rev=0;

  char s1[500], s2[500], n1[500], n2[500];
  char q1[500], q2[500], p1[500], p2[500];

  char checkstr1[500], checkstr2[500];

  char n2rc[500];
  int GZ = 0;
  int fastq = 0;
  int fastqout = 1;
  int fixname=0;

  int secondstr=0;
  char line[500];
  char head;
  char l1[500], l2[500];
  char dum[500];
  char *ch;
  int crop;
  int numfiles = 0;

  fname[0]=0;
  rname[0]=0;

  crop = 0;

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
    else if (!strcmp(argv[i], "-fix"))    
      fixname = 1;
    else if (!strcmp(argv[i], "-sstr"))    
      secondstr = 1;
    else if (!strcmp(argv[i], "-crop"))    
      crop = atoi(argv[i+1]);
  }  

  if (fname[0]==0 || rname[0]==0)
    return -1;


  if (!GZ){
    f1=fopen(fname, "r");
    f2=fopen(rname, "r");
  }
  else{
    f1=gzopen(fname, "r");
    f2=gzopen(rname, "r");
  }

  if (f1==NULL || f2 == NULL)
    return -1;


  if (!GZ){
    while (1){

      fgets(s1, 500, f1);
      fgets(s2, 500, f2);
      
      s1[strlen(s1)-1]=0;
      s2[strlen(s2)-1]=0;

      //      printf("READ S1: %s\nREAD S2: %s\n", s1, s2);
      
      /*
      fgets(checkstr1, 500, f1);
      fgets(checkstr2, 500, f2);
      */

      if (feof(f1) || feof(f2))
	break;

      /*      sscanf(checkstr1, "%s", s1);
      sscanf(checkstr2, "%s", s2);
      */

      /*
      if (!(fscanf(f1, "%s", s1) > 0 && fscanf(f2, "%s", s2) > 0))
	break;
      */

      if (s1[0] == '@'){
	//fprintf(stderr, "fastq\n");
	fastq = 1;
      }

      /*      
      fgets(checkstr1, 500, f1);
      fgets(checkstr2, 500, f2);
      
      */

      /*
      if (strchr(checkstr1, ' ') || strchr(checkstr1, '\t')){
	//fprintf(stderr, "Second Str on\n"); 
	secondstr=1;
	}*/

      if (secondstr){
	/*
	fscanf(f1, "%s", s1);
	fscanf(f2, "%s", s2);
	fgets(line, 500, f1);
	fgets(line, 500, f2);
	*/
	/*
	sscanf(checkstr1, "%s", s1);
	sscanf(checkstr2, "%s", s2);*/
	/*
	ch = strchr(checkstr1, ' ');
	*(ch)=0;
	ch = strchr(checkstr2, ' ');
	*(ch)=0;
	
	strcpy(s1, checkstr1);
	strcpy(s2, checkstr2);
	*/
      }
      
      /*      
	      fscanf(f1, "%s\n%s\n", s1, n1) > 0;
	      fscanf(f2, "%s\n%s\n", s2, n2); */
      
      
      fscanf(f1, "%s\n", n1);
      fscanf(f2, "%s\n", n2);

      if (crop != 0){
	n1[crop] = 0;
	n2[crop] = 0;
	
      }

      for (i=0; i<strlen(n1); i++)
	if (n1[i]=='.') n1[i]='N';
      for (i=0; i<strlen(n2); i++)
	if (n2[i]=='.') n2[i]='N';
      
      

      /*
      fprintf(stderr, "s1: %s\tr1: %s\n", s1, n1);
      fprintf(stderr, "s2: %s\tr2: %s\n", s2, n2);
      getchar();
      */

      if (addsuff){
	if (s1[strlen(s1)-2]=='/' && (s1[strlen(s1)-1]=='2' || s1[strlen(s1)-1]=='1'))
	  s1[strlen(s1)-2]=0;
	if (s2[strlen(s2)-2]=='/' && (s2[strlen(s2)-1]=='2' || s2[strlen(s2)-1]=='1'))
	  s2[strlen(s2)-2]=0;
      }
      

      if (fastq){

	fgets(line, 500, f1);
	fgets(line, 500, f2);

	fscanf(f1, "%s\n", q1);
	fscanf(f2, "%s\n", q2);
	if (crop != 0){
	  q1[crop] = 0;
	  q2[crop] = 0;
	  
	}

      }

      if (rcmp){
	rcomp(n2, n2rc);
	strcpy(n2, n2rc);
      }
      else if (rev){
	reverse(n2, n2rc);
	strcpy(n2, n2rc);
      }

      /*
      if (secondstr){
	if (fastq) head = '@';
	else head = '>';
      }
      else
	head = '';
      */

      //      printf("READ__ S1: %s\nREAD__ S2: %s\n", s1, s2);

      /*
      if (!addsuff){
	printf("%c%s\n%s\n", head, s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	printf("%c%s\n%s\n", head, s2, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }
      else{
	printf("%c%s/1\n%s\n", head, s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	printf("%c%s/2\n%s\n", head, s1, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }
      */

      if (strlen(n1) > strlen(n2)){
	n1[strlen(n2)] = 0;
	q1[strlen(n2)] = 0;
      }

      else if (strlen(n2) > strlen(n1)){
	n2[strlen(n1)] = 0;
	q2[strlen(n1)] = 0;
      }

      if (!addsuff){
	printf("%s\n%s\n",  s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	printf("%s\n%s\n",  s2, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }
      else{
	printf("%s/1\n%s\n",  s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	printf("%s/2\n%s\n",  s1, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }


    }
  }
  
  else{
    while (!gzeof(f1) && !gzeof(f2)) {
      gzgets(f1, s1, 500);
      if (gzeof(f1))
	break;
      if (s1[0] != '@' && s1[0]!='>')
	break;
      
      gzgets(f1, n1, 500);
      if (gzeof(f1))
	break;

      gzgets(f2, s2, 500);
      if (gzeof(f2))
	break;
      if (s2[0] != '@' && s2[0]!='>')
	break;

      gzgets(f2, n2, 500);
      if (gzeof(f2))
	break;
    
      if (s1[0] == '@')
	fastq = 1;

      if (strchr(s1, ' ')) secondstr=1;
      

      s1[strlen(s1)-1] = 0;
      s2[strlen(s2)-1] = 0;

      if (addsuff){
	if (s1[strlen(s1)-2]=='/' && (s1[strlen(s1)-1]=='2' || s1[strlen(s1)-1]=='1'))
	  s1[strlen(s1)-2]=0;
	if (s2[strlen(s2)-2]=='/' && (s2[strlen(s2)-1]=='2' || s2[strlen(s2)-1]=='1'))
	  s2[strlen(s2)-2]=0;
      }
      


      if (secondstr){
	sscanf(s1, "%s %s", dum, l1);
	sscanf(s2, "%s %s", dum, l2);
	strcpy(s1, l1);
	strcpy(s2, l2);
      }

      n1[strlen(n1)-1] = 0;
      n2[strlen(n2)-1] = 0;

      for (i=0; i<strlen(n1); i++)
	if (n1[i]=='.') n1[i]='N';
      for (i=0; i<strlen(n2); i++)
	if (n2[i]=='.') n2[i]='N';
      
      

      if (fastq){
	gzgets(f1, p1, 500);
	gzgets(f1, q1, 500);
	gzgets(f2, p2, 500);
	gzgets(f2, q2, 500);
	if (p1[0]!='+' || p2[0]!='+')
	  break;
	p1[strlen(p1)-1] = 0;
	p2[strlen(p2)-1] = 0;
	q1[strlen(q1)-1] = 0;
	q2[strlen(q2)-1] = 0;
      }

      
      if (crop != 0){
	n1[crop] = 0;
	n2[crop] = 0;
	q1[crop] = 0;
	q2[crop] = 0;
	
      }

      if (rcmp){
	rcomp(n2, n2rc);
	strcpy(n2, n2rc);
      }
      else if (rev){
	reverse(n2, n2rc);
	strcpy(n2, n2rc);
      }

      head = 0;
      if (secondstr){
	if (fastq) head = '@';
	else head = '>';
      }

      if (strlen(n1) > strlen(n2)){
	n1[strlen(n2)] = 0;
	q1[strlen(n2)] = 0;
      }

      else if (strlen(n2) > strlen(n1)){
	n2[strlen(n1)] = 0;
	q2[strlen(n1)] = 0;
      }


      if (!addsuff){
	if (head)
	  printf("%c%s\n%s\n", head, s1, n1);
	else
	  printf("%s\n%s\n", s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n", q1);
	if (head)
	  printf("%c%s\n%s\n", head, s2, n2);
	else
	  printf("%s\n%s\n", s2, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n", q2);
      }
      else{
	if (head)
	  printf("%c%s/1\n%s\n", head, s1, n1);
	else
	  printf("%s/1\n%s\n", s1, n1);
	if (fastq && fastqout)
	  printf("+\n%s\n",  q1);
	if (head)
	  printf("%c%s/2\n%s\n", head, s1, n2);
	else
	  printf("%s/2\n%s\n", s1, n2);
	if (fastq && fastqout)
	  printf("+\n%s\n",  q2);
      }

    }
  }

  return 0;
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

