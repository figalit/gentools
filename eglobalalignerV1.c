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
char *wrapper(int gapOpen,int gapExt,int matchscore,int mismatchscore,char *s,char *t);
void findSimilarity(int gapExt,int gapOpen,int matchscore, int mismatchscore,char *s,char *t,int **A,int **B,int **C);
int **originMatrix(int **currentMatrix,int i,int j,int gapExt,int gapOpen,int **A,int **B,int **C,int sLen,int tLen);
int align(int i, int j, int **matrix,int gapExt,int gapOpen,char *s,char *t,int **A,int **B,int **C,char **alignS,char **alignT,int sLen,int tLen);
char *log_pairs(char **s1, char **s2);
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

  ALIGNED = (char *) malloc(4*MAXCHARS*sizeof(char));
 //quality = (char *) malloc(4*MAXCHARS*sizeof(char));
  printf("Argc is %d \n",argc);
  /* Check number of arguments */
 if (argc!=7){
    printf("\nusage: affine_align <gapstart penalty> <gapExt>  <matchscore> <mismatchscore> <1st file> <2ndfile>\n");
   exit(1);
 }

 
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
  
  ALIGNED=wrapper(gapOpen,gapExt,matchscore,mismatchscore,s,t);
  
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




char *wrapper(int gapOpen,int gapExt,int matchscore,int mismatchscore,char *s,char *t)
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
  
  printf("; Similarity Score: %d\n",max3(A[sLen][tLen], B[sLen][tLen], C[sLen][tLen]));  
  printf(">seq1\n%s\n",alignS);
  printf(">seq2\n%s\n",alignT);
 

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

