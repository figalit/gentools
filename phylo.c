//Eray Tuzun
//Can Alkan

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <alloca.h>
#define MAX 10000
#define MAXSTRINGS 10

typedef struct _scores *scoreList;
struct _scores{
  scoreList next;
  scoreList prev;
  float value;
  int strset1[MAXSTRINGS];
  int strset2[MAXSTRINGS];
  int numstr1;
  int numstr2;
};

typedef struct _scores scores;

typedef struct phylo *hp;
struct phylo{
  hp left, right;
  int str;
  int name[MAXSTRINGS];
  int size;
};

int sig(char, char);
int max3(int, int, int);
float pairwise(char *, char *);
int strcnt;
int match=0;
int  mismatch=-1;

void addScore(float, int[], int[], int, int);
scoreList allScores=NULL;

int *np;
hp heap[MAXSTRINGS]; // holds the pointers to structure phylo
hp tree;
void insert(hp [], hp, int *);
hp MakeNode(int, int[], int);
void  StrToHeap();
hp HeapToTree(hp [],int *);
void swap(hp [], int, int);
void deletemax (hp [],int *, hp *, hp *);
void reconstScores(void);
void combine(scoreList, scoreList);
int checkSet(int [], int [], int, int);
void printtree(hp, char **);

main(int argc, char **argv){
  char S[MAXSTRINGS][MAX]; // source strings
  char *center;
  int i;
  int j;
  int maxlen=0;
  FILE *sFile;
  float score;
  char fname[10];
  int tempStrList[3];
  int tempStrList2[3];
  scoreList temp;
  strcnt = argc-1;
  for (i=0;i<strcnt;i++){
    sFile = fopen(argv[i+1],"r");
    fscanf(sFile,"%s",S[i]);
    printf("Sequence: %s Loaded. Size: %d\n",argv[i+1],strlen(S[i]));
    if (strlen(S[i]) > maxlen)
      maxlen = strlen(S[i]);
    fclose(sFile);
  } // for
  printf("Starting pairwise alignment ...\n\n");
  for (i=0;i<strcnt;i++){
    for (j=i+1;j<strcnt;j++){
      score = pairwise(S[i], S[j]);
      tempStrList[0]=i;
      tempStrList2[0]=j;
      addScore(score, tempStrList, tempStrList2, 1, 1);
      printf("Sequences [%d:%d] Aligned. Score: %f\n",(i+1),(j+1),score);
    } // for j
  } // for i

  np=(int *)malloc(sizeof(int));
  *np=0;
  StrToHeap();
  tree=HeapToTree(heap, np);
  printf("\nThe Phylogeny Tree Is:\n");
  printtree(tree, argv);
  printf("\n");
} // main

void printtree(hp tree, char **argv){
  if (tree != NULL){
    printf("( ");
    printtree(tree->left, argv);
    if (tree->left == NULL){
      printf("%s ",argv[tree->name[0]+1]);
    } // if
    printtree(tree->right, argv);
    printf(") ");
  }
} // printtree

float pairwise(char *S, char *T){
  int **V; // scoring table
  int tSize, sSize; // textSize, sourceSize
  int i, j, retVal;
  int op1, op2, op3;
  tSize = strlen(T);
  sSize = strlen(S);
  /* create vectors */
  V = (int **)malloc(sizeof(int)*(sSize+1));
  for (i=0;i<=sSize;i++)
    V[i]=(int *)malloc(sizeof(int)*(tSize+1));    
  /* initialize vectors */
  V[0][0] = 0;
  for (i=1;i<=sSize;i++)
    V[i][0] = V[i-1][0] + sig(S[i-1],'-');

  for (j=1;j<=tSize;j++)
    V[0][j] = V[0][j-1] + sig(T[i-1],'-');

  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      op1 = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      op2 = V[i-1][j] + sig(S[i-1], '-');
      op3 = V[i][j-1] + sig('-', T[j-1]);
     
      V[i][j] = max3(op1, op2, op3);
    } // for j

  retVal =  V[sSize][tSize];
  
  for (i=0;i<=sSize;i++)
   free(V[i]);
  free(V);
  
  return retVal;
} // pairwise

int sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t)){ 
    if (s!='-')
      return match;
    else
      return 0;
  } // if
  else
    return mismatch;
} // sig

int max3(int a, int b, int c){
  if (a>=b){
    if (a>=c)
      return a;
    else
      return c;
  }
  else{
    if (b>=c)
      return b;
    else
      return c;
  } 
} // max3

/* scorelist operations */

void addScore(float value, int strset1[], int strset2[], int numstr1, int numstr2){
  scoreList insert;
  scoreList temp;
  int i, j;
  int tmp;

  for (i=0;i<numstr1;i++)
    for (j=1;j<numstr1;j++)
      if ((strset1[i] > strset1[j]) && i<j){
	tmp = strset1[i];
	strset1[i] = strset1[j];
	strset1[j] = tmp;
      } // if
  for (i=0;i<numstr2;i++)
    for (j=1;j<numstr2;j++)
      if ((strset2[i] > strset2[j]) && i<j){
	tmp = strset2[i];
	strset2[i] = strset2[j];
	strset2[j] = tmp;
      } // if
  if (allScores==NULL){
    allScores = (scoreList) malloc(sizeof(struct _scores));
    allScores->next = NULL;
    allScores->prev = NULL;
    allScores->numstr1=numstr1;
    allScores->numstr2=numstr2;
    allScores->value=value;
    for (i=0;i<numstr1;i++)
      allScores->strset1[i]=strset1[i];
    for (i=0;i<numstr2;i++)
      allScores->strset2[i]=strset2[i];
  } // if
  else {
    insert = (scoreList) malloc(sizeof(struct _scores));
    insert->numstr1=numstr1;
    insert->numstr2=numstr2;
    insert->value=value;
    for (i=0;i<numstr1;i++)
      insert->strset1[i]=strset1[i];
    for (i=0;i<numstr2;i++)
      insert->strset2[i]=strset2[i];
    if (value > allScores->value){
      insert->prev=NULL;
      insert->next=allScores;
      allScores->prev=insert;
      allScores = insert;
    }
    else{
      temp=allScores;
      while (temp->next!=NULL){
	if (value > temp->value)
	  break;
	else
	  temp = temp->next;
      } // while
      
      if (value > temp->value){
	insert->next=temp;
	insert->prev=temp->prev;
	if (temp->prev == NULL)
	  allScores = insert;
	else{
	  if (insert->prev != NULL)
	    insert->prev->next=insert;
	  insert->next->prev=insert; 
	}    
      }
      else{
	temp->next=insert;
	insert->next=NULL;
	if (temp->prev == NULL){
	  insert->prev = allScores;
	  allScores->next=insert;
	}
	else
	  insert->prev=temp;
      }
    }
  } // else
} // addscore

void combine(scoreList s, scoreList t){
  int num1, num2;
  float score;
  int snum, tnum;
  int numset1[MAXSTRINGS], numset2[MAXSTRINGS];
  int i, j;
  snum = s->numstr1 + s->numstr2;
  tnum = t->numstr1 + t->numstr2;
  if (checkSet(s->strset1 , t->strset1, s->numstr1, t->numstr1)){
    num1 = s->numstr1;
    num2 = s->numstr2 + t->numstr2;
    for (i=0;i<num1;i++)
      numset1[i]=s->strset1[i];
    for (i=0;i<s->numstr2;i++)
      numset2[i]=s->strset2[i];    
    for (j=0;j<t->numstr2;j++)
      numset2[i++]=t->strset2[j];    
  } // if
  else if (checkSet(s->strset2 , t->strset1, s->numstr2, t->numstr1)){
    num1 = s->numstr2;
    num2 = s->numstr1 + t->numstr2;
    for (i=0;i<num1;i++)
      numset1[i]=s->strset2[i];
    for (i=0;i<s->numstr1;i++)
      numset2[i]=s->strset1[i];    
    for (j=0;j<t->numstr2;j++)
      numset2[i++]=t->strset2[j];    
  } // if
  else if (checkSet(s->strset1 , t->strset2, s->numstr1, t->numstr2)){
    num1 = s->numstr1;
    num2 = s->numstr2 + t->numstr1;
    for (i=0;i<num1;i++)
      numset1[i]=s->strset1[i];
    for (i=0;i<s->numstr2;i++)
      numset2[i]=s->strset2[i];    
    for (j=0;j<t->numstr1;j++)
      numset2[i++]=t->strset1[j];    
  } // if
  else if (checkSet(s->strset2 , t->strset2, s->numstr2, t->numstr2)){
    num1 = s->numstr2;
    num2 = s->numstr1 + t->numstr1;
    for (i=0;i<num1;i++)
      numset1[i]=s->strset2[i];
    for (i=0;i<s->numstr1;i++)
      numset2[i]=s->strset1[i];    
    for (j=0;j<t->numstr1;j++)
      numset2[i++]=t->strset1[j];    
  } // if

  score = (s->value * num1 / snum) + (t->value * num1 / tnum);
  if (s == allScores){
    if (allScores->next != NULL)
      allScores->next->prev = NULL;
    allScores = allScores->next;
  }
  if (t == allScores){
    if (allScores->next != NULL)
      allScores->next->prev = NULL;
    allScores = allScores->next;
  }
  if (s != allScores && t != allScores){
    if (s->prev != NULL)
      s->prev->next = s->next;
    if (s->next != NULL)
      s->next->prev = s->prev; 
    if (t->prev != NULL)
      t->prev->next = t->next;
    if (t->next != NULL)
      t->next->prev = t->prev;
  }
  addScore(score, numset1, numset2, num1, num2);
} // combine

void reconstScores(void){
  int numset1[MAXSTRINGS];
  int numset2[MAXSTRINGS];
  int tempnumset[MAXSTRINGS];
  int i;
  int num1, num2, tempnum;

  scoreList temp, temp2;
  for (i=0;i<allScores->numstr1;i++)
    numset1[i] = allScores->strset1[i];

  num1=i;

  for (i=0;i<allScores->numstr2;i++)
    numset2[i] = allScores->strset2[i];

  num2=i;

  allScores=allScores->next;

  if (allScores != NULL)
    allScores->prev=NULL;
  temp=allScores;

  while (temp!=NULL){
    if (checkSet(temp->strset1 , numset1, temp->numstr1, num1))
      for (i=0;i<temp->numstr2;i++)
	tempnumset[i] = temp->strset2[i];

    else if (checkSet(temp->strset2 , numset1, temp->numstr2, num1))
      for (i=0;i<temp->numstr1;i++)
	tempnumset[i] = temp->strset1[i];
      
    else if (checkSet(temp->strset1 , numset2, temp->numstr1, num2))
      for (i=0;i<temp->numstr2;i++)
	tempnumset[i] = temp->strset2[i];
      
    else if (checkSet(temp->strset2 , numset2, temp->numstr2, num2))
      for (i=0;i<temp->numstr1;i++)
	tempnumset[i] = temp->strset1[i];

    tempnum=i;
    temp2 = temp->next;

    while (temp2 != NULL){
      if (checkSet(temp2->strset1 , numset2, temp2->numstr1, num2)){
	if (checkSet(temp2->strset2 , tempnumset, temp2->numstr2, tempnum))
	  combine(temp, temp2);
      } // if 
      else if (checkSet(temp2->strset2 , numset1, temp2->numstr2, num1)){
	if (checkSet(temp2->strset1 , tempnumset, temp2->numstr1, tempnum))
	  combine(temp, temp2);
      } // else if
      else if (checkSet(temp2->strset1 , numset1, temp2->numstr1, num1)){
	if (checkSet(temp2->strset2 , tempnumset, temp2->numstr2, tempnum))
	  combine(temp, temp2);
      } // else if
      else if (checkSet(temp2->strset2 , numset2, temp2->numstr2, num2)){
	if (checkSet(temp2->strset1 , tempnumset, temp2->numstr1, tempnum))
	  combine(temp, temp2);
      } // else if
      temp2 = temp2->next;
    } // while temp2
    temp = temp->next;
  } // while temp
} // reconstScores

int checkSet(int set[], int numset[], int numstr, int numstr2){
  int i;
  if (numstr != numstr2)
    return 0;
  for (i=0;i<numstr;i++)
    if (set[i] != numset[i])
      return 0;
  
  return 1;
} // checkSet


/* tree operations */

void insert(hp A[],hp x,int *np){
  (*np)++;       // increment the size of the heap
  A[(*np)]=x;    // place the pointer
} // insert

hp MakeNode(int str, int name[], int size){   /* creates nodes of letters */
  hp temp;
  int i;
  temp=(hp)malloc(sizeof(struct phylo));
  temp->str=str;
  temp->size=size;
  for (i=0;i<size;i++)
    temp->name[i]=name[i];
  temp->left=NULL;
  temp->right=NULL;
  return temp;
} // MakeNode

void  StrToHeap(){ /*puts phylo structures into the heap*/  
  int i;
  int name[MAXSTRINGS];
  for (i=0;i<strcnt;i++){
    name[0]=i;
    insert(heap, MakeNode(i,name,1), np);
  } // for
} // StrToHeap

hp HeapToTree(hp A[],int *np){ /*Converts heap to tree */
  hp x,y,z;
  int sum;
  int i,j;
  int name[MAXSTRINGS];
  x=(hp)malloc(sizeof(struct phylo));
  y=(hp)malloc(sizeof(struct phylo));
  z=(hp)malloc(sizeof(struct phylo));
  while ((*np)>1){ // until one node remains in the heap
    deletemax(A, np, &x, &y); // get the max scored x&y pair
    for (i=0;i<x->size;i++)
      name[i]=x->name[i];
    for (j=0;j<y->size;j++)
      name[i++]=y->name[j];
    z=MakeNode(-1,name,x->size+y->size);   // create a new element and put it into the heap
    z->left=x;
    z->right=y;
    insert(heap,z,np);
  } // while
  return z; // return the root of the phylogeny tree
} // HeapToTree

void deletemax (hp A[],int *pn, hp *x, hp *y){ /* finds the */
  int i;
  int xi, yi;

  for(i=1;i<=(*pn);i++){      
    if (mystrcmp(A[i]->name, allScores->strset1, A[i]->size,allScores->numstr1)){
      xi=i;
      *x=A[i];
    } // if
  } // for
  swap(A,xi,*pn);//put the minimum weighted node to the end of the heap
  (--(*pn)); // decrement size of the heap
  
  for(i=1;i<=(*pn);i++){      
    if (mystrcmp(A[i]->name, allScores->strset2, A[i]->size, allScores->numstr2)){
      yi=i;
      *y=A[i];
    } // else if
  }
  swap(A,yi,*pn);//put the minimum weighted node to the end of the heap
  (--(*pn)); // decrement size of the heap
  reconstScores();
} // deletemax

void swap(hp A[],int i,int j){  /* changes the values in position i and j */
  hp temp;
  temp=A[i];
  A[i]=A[j];
  A[j]=temp;
} // swap


int mystrcmp(int s[], int t[], int ss, int ts){
  int i;
  int j;
  int tmp;
  if (ss != ts)
    return 0;

  for (i=0;i<ss;i++)
    for (j=1;j<ss;j++){
      if ((s[i] > s[j]) && i<j){
	tmp = s[i];
	s[i] = s[j];
	s[j] = tmp;
      }
      if ((t[i] > t[j]) && i<j){
	tmp = t[i];
	t[i] = t[j];
	t[j] = tmp;
      } // if
    }
  return (checkSet(s,t,ss,ss));
} // mystrcmp
