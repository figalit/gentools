#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

//More stable version, my only concern is I am allocating RESULT and ALIGNED 10K, but it is a kind of adhoc.. Try to change this later
//Difference from version eglobalaligner5quality.c
	//More error checking,
	//More stable memory allocation
//Derived from eglobalaligner5. Added functions to calculate quality
//TRACEback bug corrected
//WORKING VERSION, I am going to remove the checks if the gap open and gap extensions are not negatives in next version
//Added uppercase so the input files characters doesn't matter.
//CHanging the functions trying to find the bug, minimize the use of chars
//Get rid of some pointers

//Copied from aligntrial71.c
//GOOD WORKING VERSION,originmatrix is fixed
//A,B,C are not global anymore!@
//NO GLOBAL VARIABLEs
int getScore(char c1, char c2,int matchscore, int mismatchscore);
int max3(int i, int j, int k);
int **max3Ptr(int **A, int  **B, int **C, int i, int j, int k);
char *wrapper(int gapOpen,int gapExt,int matchscore,int mismatchscore,char *s,char *t,char *quality,int qthreshold);
void findSimilarity(int gapExt,int gapOpen,int matchscore, int mismatchscore,char *s,char *t,int **A,int **B,int **C);
int **originMatrix(int **currentMatrix,int i,int j,int gapExt,int gapOpen,int **A,int **B,int **C,int sLen,int tLen);
int align(int i, int j, int **matrix,int gapExt,int gapOpen,char *s,char *t,int **A,int **B,int **C,char **alignS,char **alignT,int sLen,int tLen);
char *log_pairs(char **s1, char **s2,char *quality,int qthreshold);
int main (int argc, char *argv[]){
  
  int matchscore,mismatchscore;
  int gapExt,gapOpen;
  int sLen,tLen;
  
  FILE *sFile;
  FILE *tFile;
  int MAXCHARS=5000;  
  char s[MAXCHARS];
  char t[MAXCHARS];
  char *ALIGNED;
  char quality[20000];
  int qthreshold=30;

  ALIGNED = (char *) malloc(4*MAXCHARS*sizeof(char));
 //quality = (char *) malloc(4*MAXCHARS*sizeof(char));
  printf("Argc is %d \n",argc);
  /* Check number of arguments */
 if (argc!=7){
    printf("\nusage: affine_align <gapstart penalty> <gapExt>  <matchscore> <mismatchscore> <1st file> <2ndfile>\n");
   exit(1);
 }


  strcpy(quality,"4 4 8 8 8 8 9 9 9 8 8 13 17 14 13 13 9 9 9 20 14 13 10 22 20 25
25 29 18 24 13 10 9 11 7 10 15 8 8 8 11 8 12 10 19 13 18 18 29
29 40 40 40 46 46 46 39 35 35 35 39 39 48 48 40 40 39 33 33 27
23 19 19 23 27 33 40 39 40 40 40 40 40 40 40 40 40 40 40 34 34
34 34 34 35 45 45 45 45 39 34 35 35 35 35 40 40 51 45 51 56 56
56 56 56 56 56 56 51 51 51 51 51 51 56 51 51 51 51 45 45 45 45
45 45 45 45 45 45 45 45 51 51 51 45 45 45 45 45 45 45 45 45 45
45 45 45 45 45 45 45 40 40 45 45 51 51 51 45 40 40 40 40 40 40
45 45 51 51 51 56 56 56 51 51 51 51 51 51 51 51 51 51 51 51 56
56 51 51 51 45 45 45 51 51 51 56 56 56 51 51 51 51 45 45 51 51
51 51 51 43 43 43 43 43 43 56 51 51 51 45 45 45 45 45 45 51 51
51 51 51 45 45 45 45 45 45 45 45 45 45 51 51 51 51 51 51 51 56
56 56 43 43 43 43 43 43 56 56 56 56 56 56 56 56 56 56 56 56 56
56 56 51 51 51 51 51 51 56 56 56 56 56 51 51 51 51 51 51 56 56
56 56 56 56 56 56 56 51 51 51 51 51 51 51 51 51 56 56 56 56 56
56 56 51 51 51 51 51 51 51 51 56 56 56 56 56 56 56 51 51 51 51
51 51 51 51 51 51 56 56 56 56 56 56 56 56 56 56 56 56 56 56 51
51 51 51 51 51 51 56 56 56 56 56 56 56 56 56 56 56 56 56 56 56
56 56 56 56 56 56 56 56 51 51 51 45 45 45 51 51 51 45 45 45 45
45 45 45 41 41 45 56 56 56 56 56 56 56 56 56 51 51 51 51 51 45
51 51 51 51 51 56 56 56 56 51 51 51 51 51 51 56 56 56 56 56 56
56 56 56 56 51 51 51 51 51 51 51 51 51 51 51 51 51 51 56 56 56
51 51 51 51 51 51 56 56 56 56 56 56 51 51 51 51 51 51 56 56 56
56 56 56 51 51 51 51 51 51 56 56 56 56 56 56 56 56 56 56 56 56
56 56 56 51 51 51 51 51 51 51 51 51 51 56 56 51 51 51 51 51 51
56 56 56 56 56 56 56 56 56 56 51 51 51 40 40 40 40 40 40 40 40
40 40 40 40 46 48 48 48 44 40 40 32 29 25 29 29 40 48 48 48 48
48 48 48 56 56 56 40 40 40 40 40 37 56 51 51 42 42 42 51 45 45
45 45 45 45 40 40 37 40 37 40 51 56 56 46 46 33 33 33 40 44 40
48 40 32 32 25 29 32 40 32 34 39 39 39 40 40 46 46 46 56 56 56
56 56 56 56 46 46 40 40 40 40 28 31 21 16 16 27 27 32 40 48 28
27 22 29 48 48 48 48 48 48 48 48 48 56 56 56 56 56 48 40 40 40
34 32 32 32 32 25 34 48 40 40 40 44 40 46 48 48 56 56 48 48 48
48 40 40 40 48 37 34 34 29 31 21 18 18 29 32 48 40 44 40 40 33
33 30 48 48 56 56 56 56 48 48 48 48 48 28 17 17 10 9 10 19 17 22
22 20 29 40 48 40 44 44 32 32 32 23 18 18 23 18 19 29 29 32 29
32 24 26 32 25 32 32 32 34 40 48 34 34 25 25 24 24 13 12 10 19
19 25 29 34 48 48 37 28 28 28 33 28 29 27 13 11 11 19 27 19 16
21 16 15 21 19 19 20 26 29 25 19 15 19 10 10 12 21 13 17 10 13
13 18 14 13 11 8 8 9 8 8 12 22 27 23 10 10 10 9 9 9 14 14 9 13
11 7 6 6 6 9 9 9 9 6 6 6 10 29 18 15 15 13 9 6 6 6 6 9 8 7 8 9
9 9 9 9 9 8 8 12 9 8 9 8 6 6 8 6 6 6 6 6 6 6 9 6 6 6 6 6 6 8 9
7 8 11 10 11 9 12 12 10 10 14 12 17 14 14 15 20 14 10 11 8 6 8
9 7 7 7 8 8 9 7 7 7 8 6 6 8 8 7 9 9 14 14 8 11 8 8 12 11 15 21
9 7 7 7 7 7 7 7 7 7 7 7 6 7 14 7 9 18 10 10 6 6 8 8 6");

 
  /* Read data files */
  gapOpen = atoi(argv[1]);
  gapExt = atoi(argv[2]);
  matchscore = atoi(argv[3]);
  mismatchscore = atoi(argv[4]);
  
  sFile = fopen(argv[5],"r");
  tFile = fopen(argv[6],"r");
  fscanf(tFile,"%s",t);
  fscanf(sFile,"%s",s);

  sLen=strlen(s);
  tLen=strlen(t);
  

//printf("%see\n",quality);
  //printf("%s and %s and %d %d\n",s,t,gapOpen,gapExt);
  /* Algorithm */
  ALIGNED=wrapper(gapOpen,gapExt,matchscore,mismatchscore,s,t,quality,qthreshold);
  
  printf("%s\n",ALIGNED);
 
  
  exit(0);
}

/* Fill out the 3 matrices
   C[i,j] Maximum score of alignment between s[1..i] and t[1..j] ending in s[i] matching t[j]
   B[i,j] Maximum score of alignment between s[1..i] and t[1..j] ending in a space matching t[j]
   C[i,j] Maximum score of alignment between s[1..i] and t[1..j] ending in s[i] matching a space
 */
void findSimilarity(int gapExt,int gapOpen,int matchscore, int mismatchscore,char *s,char *t,int **A,int **B,int **C) {

  int i,j, compare1, compare2, compare3, compare4, compare5, compare6;
  
  
  

  int sLen=strlen(s);
  int tLen=strlen(t);
  
  A[0][0] = 0;
  B[0][0] = INT_MIN;
  C[0][0] = INT_MIN;
  
  for (i=1; i<=sLen; i++) {
    A[i][0] = INT_MIN;
    B[i][0] = INT_MIN;
    //C[i][0] = -(gapOpen+gapExt*i); //FOR TERMINAL GAPS
    C[i][0] = 0; 
  }
   
  for (j=1; j<=tLen; j++) {
    A[0][j] = INT_MIN;
    //B[0][j] = -(gapOpen+gapExt*j); //FOR TERMINAL GAPS
    B[0][j] = 0;
    C[0][j] = INT_MIN;
  }
 
  for (i=1; i<=sLen; i++) {
    for (j=1; j<=tLen; j++) {
      A[i][j] = getScore(toupper(s[i-1]), toupper(t[j-1]),matchscore,mismatchscore) + max3(A[i-1][j-1], B[i-1][j-1], C[i-1][j-1]);
      //printf("Match mismatch score \t %d\n",getScore(scoreMatrix, alpha, s[i-1], t[j-1]));
      /* Overflow checks. Necessary because of -inf values */
	  
           			
		if ((-(gapOpen+gapExt)+A[i][j-1] < A[i][j-1]) && (i!=sLen)) //For terminal Gaps
		
		  compare1 = -(gapOpen+gapExt)+A[i][j-1];
		else
		  compare1 = A[i][j-1];

		if ((-(gapOpen+gapExt)+A[i-1][j] < A[i-1][j]) && (j!=tLen)) //For terminal Gaps
		  compare2 = -(gapOpen+gapExt)+A[i-1][j];
		else
		  compare2 = A[i-1][j];

		if ((-(gapOpen+gapExt)+B[i-1][j] < B[i-1][j]) && (j!=tLen)) //For terminal Gaps
		  compare3 = -(gapOpen+gapExt)+B[i-1][j];
		else
		  compare3 = B[i-1][j];

		if ((-(gapOpen+gapExt)+C[i][j-1] < C[i][j-1])  && (i!=sLen)) //For terminal Gaps
		
		  compare4 = -(gapOpen+gapExt)+C[i][j-1];
		else
		  compare4 = C[i][j-1];
       
    	
		if (((-gapExt+B[i][j-1]) < B[i][j-1])  && (i!=sLen)) //For terminal Gaps
		
		  compare5 = -gapExt+B[i][j-1];
		else
		  compare5 = B[i][j-1];

		if (((-gapExt+C[i-1][j]) < C[i-1][j]) && (j!=tLen)) //For terminal Gaps
		  compare6 = -gapExt+C[i-1][j];
		else
		  compare6 = C[i-1][j];
 

      	B[i][j] = max3(compare1, compare5, compare4);
      	C[i][j] = max3(compare2, compare3, compare6);
    }
  }

  return;
}
    

/* Find which matrix containes the previous value of the alignment path */
int **originMatrix(int **currentMatrix,int i,int j,int gapExt,int gapOpen,int **A,int **B,int **C,int sLen,int tLen) {
  
  if (i==0 && j!=0)
    return B;
  if (i!=0 && j==0)
    return C;
  
  if (currentMatrix == A)
    return max3Ptr(A,B,C,A[i-1][j-1], B[i-1][j-1], C[i-1][j-1]);
  else if (currentMatrix == B) {
  	if (i!=sLen)
  	
    	return max3Ptr(A,B,C,-(gapOpen+gapExt)+A[i][j-1], -gapExt+B[i][j-1], -(gapOpen+gapExt)+C[i][j-1]);
    else
    	return max3Ptr(A,B,C,A[i][j-1], B[i][j-1], C[i][j-1]);
  }
  else if (currentMatrix == C)
  {
  	if (j!=tLen)
  	  return max3Ptr(A,B,C,-(gapOpen+gapExt)+A[i-1][j], -(gapOpen+gapExt)+B[i-1][j], -gapExt+C[i-1][j]);
  	else
  		return max3Ptr(A,B,C,A[i-1][j], B[i-1][j], C[i-1][j]);
  }
  exit(1);
}


/* Find optimal alignment from similarity matrices */
int align(int i, int j, int **matrix,int gapExt,int gapOpen,char *s,char *t,int **A,int **B,int **C,char **alignS,char **alignT,int sLen,int tLen){
  int len=0;
  int newi, newj;

  if (i<=0 && j<=0) {
    return 0;
  }

//printf("Eray\n");
  /* New indices */
  if (matrix == A) {
    newi = i-1;
    newj = j-1;
  }
  else if(matrix == B) {
    newi = i;
    newj = j-1;
  }
  else {
    newi = i-1;
    newj = j;
  }
		     
  /*Recursive calls*/
  if (i>0 && originMatrix(matrix,i,j,gapExt,gapOpen,A,B,C,sLen,tLen) == C) {
    len = align(newi, newj, C,gapExt,gapOpen,s,t,A,B,C,alignS,alignT,sLen,tLen);
  }
  else if (j>0 && originMatrix(matrix,i,j,gapExt,gapOpen,A,B,C,sLen,tLen) == B) {
    len = align(newi, newj, B,gapExt,gapOpen,s,t,A,B,C,alignS,alignT,sLen,tLen);
  }
  else {
    len = align(newi,newj, A,gapExt,gapOpen,s,t,A,B,C,alignS,alignT,sLen,tLen);
  }

  /* Character insertion */
  if (matrix == A) {
    (*alignS)[len] = s[i-1];
    (*alignT)[len] = t[j-1];
   // printf("%c and %c \n",s[i-1],t[j-1]);
  }
  else if(matrix == B) {
    (*alignS)[len] = '-';
    (*alignT)[len] = t[j-1];
   //   printf("%c and %c \n",'-',t[j-1]);
  }
  else {
    (*alignS)[len] = s[i-1];
    (*alignT)[len] = '-';
   //  printf("%c and %c \n",s[i-1],'-');
  }
  len++;
   (*alignS)[len] = 0;
    (*alignT)[len] = 0;
  
  
  return len;
}




char *wrapper(int gapOpen,int gapExt,int matchscore,int mismatchscore,char *s,char *t,char *quality,int qthreshold)
{
	int **A,**B,**C;
	int i,len;
	static char *ALIGNED;
	
	
  	int sLen=strlen(s);
    int tLen=strlen(t);	
	
	
	char *alignS;
	char *alignT;

	
     /* Allocate memory */
    

    
      A = (int **) malloc((sLen+1)*sizeof(int *));
      B = (int **) malloc((sLen+1)*sizeof(int *));
      C = (int **) malloc((sLen+1)*sizeof(int *));
      for (i=0; i<=sLen; i++)
      {
        A[i] = (int *) malloc((tLen+1)*sizeof(int));
        B[i] = (int *) malloc((tLen+1)*sizeof(int));
        C[i] = (int *) malloc((tLen+1)*sizeof(int));
	  }  
  
  if (ALIGNED==NULL)
   // ALIGNED = (char *) malloc(5*(sLen+tLen+3)*sizeof(char)); //Memory allocation is the same as RESULT, they have to be same size
   ALIGNED = (char *) malloc(10000*sizeof(char)); 
  else 														//With size maybe it is a safer idea to use max(blabla,and some constant number)
   	sprintf(ALIGNED,"%s","");	 //Initialize the thing, since it's static it's a good idea to initialize all the time

    alignS = (char *) malloc((sLen+tLen+3)*sizeof(char));
    alignT = (char *) malloc((sLen+tLen+3)*sizeof(char));
  

  
  //printf("\n seq1 is INSIDE THE PROGRAM %s\n",s);
  findSimilarity(gapExt,gapOpen,matchscore,mismatchscore,s,t,A,B,C);
 //printf("The three matrices %d and %d and %d and slen %d tlen %d\n",A[sLen][tLen], B[sLen][tLen], C[sLen][tLen],sLen,tLen);
  //printf( "Align is starting \n");
  len = align(sLen, tLen, max3Ptr(A,B,C,A[sLen][tLen], B[sLen][tLen], C[sLen][tLen]),gapExt,gapOpen,s,t,A,B,C,&alignS,&alignT,sLen,tLen);
  

 /* Print result */
  //printf("; Similarity Score: %d\n",max3(A[sLen][tLen], B[sLen][tLen], C[sLen][tLen]));  
  //printf(">seq1\n%s\n",alignS);
  //printf(">seq2\n%s\n",alignT);
 
 // printf("Laylay2\n");
 
  // printf( "Log pair is starting\n");
   ALIGNED=log_pairs(&alignS,&alignT,quality,qthreshold);
    
    
    


  //FREEING UP MEMORYYYY  
  
    
    free(alignS);
   free(alignT);  
 // printf("Slen is %d\n and tlen is %d \n",sLen,tLen);
    for (i=0;i<=sLen;i++){
      free(A[i]);
      free(B[i]);
      free(C[i]);
     
   } // for
 //  printf("Finished3\n");
// printf( "Program is finished\n");  
  free(A);
  free(B);
  free(C);
 //printf("Finished\n");
 
 

  //printf("Finished2\n");
 //printf( "Program is finished\n");  
  return ALIGNED;

}
int getScore(char c1, char c2,int matchscore, int mismatchscore){
//Checks if c1 and c2 are equal, if yes returns matchscore if not returns mismatchscore
       
   if (c1==c2)
  	return matchscore;

   else
	return mismatchscore;

}

int max2(int i, int j){
 // Returns the maximum of two numbers given

  if (i>=j) return i;
  else return j;
}

int max3(int i, int j, int k){
  // Returns the maximum of three numbers given
  if (i>=j && i>=k) {
    return i;
  }
  if (j>=i && j>=k) {
    return j;
  }
  return k;
}

int **max3Ptr(int **A, int **B, int **C, int i, int j, int k) {
//compares the three numbers and returns a pointer, i-->A, j-->B, k-->C

 
  if (k>=j && k>=i) {
    return C;
  }
  if (j>=i && j>=k) {
    return B;
  }
  return A;
}

/* 

   INPUT: ALIGNMENT in
   >seq1
   ACACATTTT
   >seq2
   ACACACA
   
   format( Make sure you handle the cases if the sequence is not in one line)
   
   OUTPUTS a string of the statistics

  
*/

char *log_pairs(char **s1, char **s2,char *quality,int qthreshold){

//Inputs: aligned sequence 1, aligned sequence 2, quality file associated to sequence2, quality threshold
  /* data for distance computation */
  int indel_number = 0;
  int indel_spaces = 0;
  int base_spaces = 0; //spaces 
  int samebases = 0; // matches
  int base_mismatches = 0; //mismatches
  int transversions = 0; // purine-pyrimidine
  int transitions = 0; // pyrimidine-pyrimidine || purine-purine
  int s1a, s1c, s1g, s1t;
  int s2a, s2c, s2g, s2t;
  int ag,ac,at,aa,an;
  int gc,gt, ga, gg,gn;
  int ca,cc,cg,ct,cn;
  int ta,tc,tg,tt,tn;
  int na,nc,ng,nt,nn;
  int first=1;  //if it is the first occurence I need to start tokenizer.
  /* variables for loops */
  int i,j;

  /* variables for distances */
  double percent_sim;
  double SE_percent_sim;
  double percent_sim_windel;
  double SE_percent_sim_windel;
  double p; // temp variable
  double q; // temp variable
  double a; // temp variable
  double b; // temp variable
 
  int total_aligned_bases=0; //Total aligned bases without considering the quality if none of the sequences have strings have -
 
  double k_jukes_cantor;
  double SE_k_jukes_cantor;
  double k_kimura;
  double SE_k_kimura;
  static char *RESULT;
 
 
  //New Variables,
  int sLen=strlen(*s1);
  int tLen=strlen(*s2);
  int qualityarray[tLen+1]; //Quality array that holds the quality for each base
  int MaxQuality=999; //if the alignment corresponds to a gap we maximize its quality
  char *myqualitystring;
  
  FILE *errorlog;
  
  
  if (sLen!=tLen)
  {
  	printf("The alignment lengths don't match first sequence \n%s\nHas length %d, the second sequence \n%s\nHas length %d",*s1,sLen,*s2,tLen);
  
  	exit(0);
  }
//printf("USELESS INFORMATIOn Slen: %d and Tlen: %d \n",sLen,tLen);  


 if (myqualitystring==NULL)
  	myqualitystring = (char *) malloc((5)*sizeof(char)); //5 is kind of adhoc number
 else
  	sprintf(myqualitystring,"%s",""); //initalize the myqualitystring
 
 if (RESULT==NULL)
      // RESULT = (char *) malloc(5*(sLen+tLen+3)*sizeof(char)); //The sizes of RESULT and ALIGNED Has to be same and no realloc
      RESULT = (char *) malloc(10000*sizeof(char));
 else
       sprintf(RESULT,"%s",""); //initalize the result
    

  
  j=0; //It is going to be used in quality array

  for(i=0;i<tLen;i++)
  {
  	
  
   		 
   		if ((*s2)[i]=='-') //You have a space no corresponding quality exists
   		{
   	
   			(*s1)[i]=toupper((*s1)[i]); // Capatilize the second sequence
   			qualityarray[j++]=MaxQuality; //If no corresponding quality available since it is a gap, give the max quality.
   			
   			
   		}
   		else if ((*s2)[i]=='\n')
              printf("Something wrong I wasn't expecting a <return> character in the sequence\n"); 
        else //Means I have a normal character
        {
        
        	//Make both of the sequences upper case.
        	
        	(*s1)[i]=toupper((*s1)[i]);
        	(*s2)[i]=toupper((*s2)[i]);
        	
        	//scanf("%c\n",&ch);
        	if (first==1) 
        	{
        		(myqualitystring) = (strtok(quality," "));
        		if (myqualitystring != NULL)
        		//printf("Quality %s %c %d\n",myqualitystring,s2[i],i);
        			qualityarray[j++]=atoi(myqualitystring);
        		else
        		
        		{
        			printf("Quality file is missing \n");
        			exit(0);
        		}
        		first=2;
        	}
			else
			{
				myqualitystring = strtok(NULL," ");
				//printf("Started\n");
				if(myqualitystring != NULL)
				{
					if (strcmp(myqualitystring,"\n")!=0)
					{
             	
             		//	printf("Quality %s %c %d\n",myqualitystring,s2[i],i);
             			qualityarray[j++]=atoi(myqualitystring);
             		}
             		
             	}
             	else
             	{
				     printf("Quality file is missing \n");
				 	 printf("%s is my quality string\n",myqualitystring);
				     printf("string is %s \nString is %s\n quality is%s%d\n and tlen is %d and its the %dth iteration previus qualityarrayvalue:%d Qual size is %d\n",*s1,*s2,quality,111,tLen,i,qualityarray[j-1],j-1);
				     exit(0);
        		}
             		
            }  
            
            
            if ((*s1)[i]!='-')   //position i is gap free in the alignment
            	total_aligned_bases++;
            	
        }
          
   
   
   }
   
   errorlog=fopen("log.out","w");
   myqualitystring = strtok(NULL," ");
   while(myqualitystring !=NULL)
   {
		
	
		fprintf(errorlog,"Sequence %s\n have \n",*s2);
		//if (strcmp(myqualitystring,"\n")!=0)
		//{   		
   			fprintf(errorlog,"Sequence and quality file lengths don't match extra %s\n",myqualitystring);
   		
      	//}
      	myqualitystring = strtok(NULL," ");
   
   	}

		fclose(errorlog);
 
//TEST STUFF

//for (i=0;i<tLen;i++)
//	printf("%d\t",qualityarray[i]);
//printf("\n");
  

// CHECK FOR ERRORS



  s1a = s1c = s1g = s1t = 0;
  s2a = s2c = s2g = s2t = 0;
  ag = 0; ac = 0; at = 0; aa = 0; an = 0; 
  gc = 0; gt = 0; gg = 0; ga = 0; gn = 0; 
  ca = 0; cc = 0; cg = 0; ct = 0; cn = 0; 
  ta = 0; tc = 0; tg = 0; tt = 0; tn = 0; 
  na = 0; nc = 0; ng = 0; nt = 0; nn = 0; 
  
  
  


  // collect data
  for (i = 0; i<sLen; i++){
  
  	//UPPER CASE THE BOTH SEQUENCES
  	
  	if (qualityarray[i]<qthreshold)
    {
    	
    	//If they are below threshold i should make them lowercase
    	//s1[i]=tolower(s1[i]); Only the second one not the first one
    	(*s2)[i]=tolower((*s2)[i]);
    	
    	
  		//lowercase mask the sequence that comes with the quality
  		 //IF it is smaller than given threshold, we don't want to consider this base
  		//printf("Positionn %d has lower quality\n",i);
  
    }
    else
    {
    	switch (toupper((*s1)[i])){
    		case 'A':
     
    		  if ((*s2)[i] == 'T') at++;
    		  else if ((*s2)[i] == 'G') ag++;
    		  else if ((*s2)[i] == 'C') ac++;
    		  else if ((*s2)[i] == 'A') aa++;
    		  else if ((*s2)[i] == 'N') an++;
    		  break;
    		case 'C':
      
    		  if ((*s2)[i] == 'T') ct++;
    		  else if ((*s2)[i] == 'G') cg++;
    		  else if ((*s2)[i] == 'A') ca++;
    		  else if ((*s2)[i] == 'C') cc++;
    		  else if ((*s2)[i] == 'N') cn++;
    		  break;
    		case 'G':
     
    		  if ((*s2)[i] == 'T') gt++;
    		  else if ((*s2)[i] == 'A') ga++;
    		  else if ((*s2)[i] == 'C') gc++;
    		  else if ((*s2)[i] == 'G') gg++;
    		  else if ((*s2)[i] == 'N') gn++;
    		  break;
    		case 'T':
     
    		  if ((*s2)[i] == 'G') tg++;
    		  else if ((*s2)[i] == 'A') ta++;
    		  else if ((*s2)[i] == 'C') tc++;
    		  else if ((*s2)[i] == 'T') tt++;
    		  else if ((*s2)[i] == 'N') tn++;
     		  break;
    
    		case 'N':
    		  if ((*s2)[i] == 'G') ng++;
    		  else if ((*s2)[i] == 'A') na++;
    		  else if ((*s2)[i] == 'C') nc++;
    		  else if ((*s2)[i] == 'T') nt++;
    		  else if ((*s2)[i] == 'N') nn++;
    		  break;
    	default:
    		  break;
    	}

    }  //else
    if ((*s1)[i] == '-'){
      indel_spaces++;
      if (i == 0)
	indel_number++;
      else if ((*s1)[i-1] != '-')
	indel_number++;
    }
    else if ((*s2)[i] == '-'){
      indel_spaces++;
      if (i == 0)
	indel_number++;
      else if ((*s2)[i-1] != '-')
	indel_number++;
    }
  }
  
  
  s1a = aa+ag+ac+at;
  s1g = ga+gg+gc+gt;
  s1c = ca+cg+cc+ct;
  s1t = ta+tg+tc+tt;

  s2a = aa+ga+ca+ta;
  s2g = ag+gg+cg+tg;
  s2c = ac+gc+cc+tc;
  s2t = at+gt+ct+tt;

  base_mismatches = ag+ac+at+ga+gc+gt+ca+cg+ct+ta+tg+tc;
  transversions = at+ta+ac+ca+gc+cg+gt+tg;
  transitions = ag+ga+ct+tc;
  samebases = aa+gg+cc+tt;
  base_spaces = aa+ag+ac+at+ga+gc+gg+gt+ta+tc+tt+tg+ca+cc+cg+ct;

 // printf("indelnumber %d\nindelspaces %d\nbasespaces %d\nbasematches %d\nbasemismatches %d\ntransversions %d\ntransitions %d\n", indel_number, indel_spaces, base_spaces, samebases, base_mismatches, transversions, transitions);

  /* compute distances */
  
  percent_sim = (double)samebases / (double)base_spaces;
  SE_percent_sim = sqrt(((double)samebases/(double)base_spaces) * (1-(double)samebases/(double)base_spaces)/(double)base_spaces);
  //printf("%2.15f -- %2.15f\n", percent_sim, SE_percent_sim);
  
  percent_sim_windel = (double)samebases / ((double)base_spaces + (double)indel_number);
  SE_percent_sim_windel = sqrt(percent_sim_windel * (1-percent_sim_windel) / ((double)(base_spaces+indel_number)));

  //printf("%2.15f -- %2.15f\n", percent_sim_windel, SE_percent_sim_windel);

  p = (double)base_mismatches / (double)base_spaces;
  k_jukes_cantor = -0.75 * log(1-4*p/3);
  SE_k_jukes_cantor = sqrt((1-p)*p/((double)base_spaces * pow((1-4*p/3),2)));

  //printf("%2.15f -- %2.15f\n", k_jukes_cantor, SE_k_jukes_cantor);
  
  p = (double)transitions / (double)base_spaces;
  q = (double)transversions / (double)base_spaces;
  a = 1 / (1 - 2*p - q);
  b = 1 / (1 - 2*q);
  k_kimura = 0.5 * log(a) + 0.25 * log(b);
  SE_k_kimura = sqrt((a*a*p + pow((a+b)/2,2)*q - pow(a*p +(((a+b)/2*q)), 2)) / (double)base_spaces);

  //printf("%2.15f -- %2.15f\n", k_kimura, SE_k_kimura);



 
 //sprintf(RESULT,"%s",""); //NECESSARY INITILIAZING
  //SULT[0]='\0';
  //  printf("j: %f a %f b %f lake %20.15f\n", h,a,b,d_lake);
  

 //sprintf(RESULT, "######## jqscorer1 #######\n");
 //sprintf(RESULT, "%s###Seq1 (>seq1) \n",RESULT);
  //sprintf(RESULT, "%s###Seq2 (>seq2) \n",RESULT);
  //sprintf(RESULT, "%s###Seq1aln (%d) \n",RESULT,sLen);
  //sprintf(RESULT, "%s###Seq2aln (%d) \n",RESULT,tLen);
  //sprintf(RESULT, "%s###NA \n",RESULT); //I don't need to calculate those
  //sprintf(RESULT, "%s###NA \n",RESULT);
 //sprintf(RESULT, "%s###Seq1qualbase (0) \n",RESULT);
  //sprintf(RESULT, "%s###Seq2qualbase (%d) \n",RESULT,tLen);
 sprintf(RESULT,">seq1\n%s\n>seq2\n%s\n",*s1,*s2);
 // sprintf(RESULT, "%s###fraction :%5f\n",RESULT,percent_sim);
 // sprintf(RESULT, "%s###		SE :%5f\n",RESULT,SE_percent_sim);
 // sprintf(RESULT, "%s###Gaps :%d\n",RESULT,indel_number);
 // sprintf(RESULT, "%s###Dashes :%d\n",RESULT,indel_spaces);
 // sprintf(RESULT, "%s###AlignedBases with Ns:%d:NA\n",RESULT,total_aligned_bases);
 // sprintf(RESULT, "%s###### MASKING BELOW\n",RESULT);
 // sprintf(RESULT, "%s###aligned bases %d###   matches:%d\n",RESULT,samebases+base_mismatches,samebases);
 // sprintf(RESULT, "%s###	mismatches:%d\n",RESULT,base_mismatches);
 // sprintf(RESULT, "%s###		transversions:%d\n",RESULT,transversions);
 // sprintf(RESULT, "%s###		transitions:%d\n",RESULT,transitions);
 // sprintf(RESULT, "%s###k_jukes_cantor :%5f\n",RESULT,k_jukes_cantor);
//sprintf(RESULT, "%s###SE_k_jukes_cantor:%5f\n",RESULT,SE_k_jukes_cantor);

 sprintf(RESULT,"%s*HEADER\tfractionbp\tfractionbpSE\ttotal_gaps\ttotal_dashes\ttotal_aligned_bases\taligned_bases\tmatches\tmismatches\ttransversions\ttransitions\tjc\tjcSE\tk2\tk2SE\n",RESULT);
 
 sprintf(RESULT,"%s*DATA:\t%f\t%f\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\n",RESULT,percent_sim,SE_percent_sim,indel_number,indel_spaces,total_aligned_bases,samebases+base_mismatches,samebases,base_mismatches,transversions,transitions,k_jukes_cantor,SE_k_jukes_cantor,k_kimura,SE_k_kimura);
 
 
  return RESULT;
}
