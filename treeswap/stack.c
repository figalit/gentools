#include "stack.h"
STACK  *createstack(int size){
  STACK *temp;
  temp=(STACK *)malloc(sizeof(struct STCK));
  temp->left=calloc(size,sizeof(int));
  temp->right=calloc(size,sizeof(int));
  temp->start=calloc(size,sizeof(int));
  temp->marked=calloc(size,sizeof(int));
  temp->end=calloc(size,sizeof(int));
  temp->top=0;
  temp->size=size;
  return temp;
} // create

void clear(STACK *pS){
  pS->top=-1;
} // clear

boolean isEmpty(STACK *pS){
  return (pS->top<=0);
} // isEmpty

boolean pop(STACK *pS,int *left, int *start, int *end){
  if (isEmpty(pS)) 
    return FALSE;
  else {
    (*left)=pS->left[pS->top];
    (*start)=pS->start[pS->top];
    (*end)=pS->end[(pS->top)--];
    return TRUE;
  } // else
} // pop

void push(int left, int right, int start, int marked, STACK *pS){
  pS->left[++(pS->top)]=left;
  pS->right[pS->top]=right;
  pS->start[pS->top]=start;
  pS->marked[pS->top]=marked;
} // push

void mark(STACK *pS){
  pS->marked[pS->top]=1;
} // mark

boolean isMarked(STACK *pS){
  if (pS->top >= 2)
    return (pS->marked[pS->top-1]);
  return 0;
} // isMarked

boolean incRight(STACK *pS, int end){
  (pS->right[pS->top])++;
  pS->end[pS->top] = end; 
  if (pS->left[pS->top] == pS->right[pS->top])
    return TRUE;
  return FALSE;
} // incRight

void incTop(int left1, int left2, STACK *pS){
  pS->left[pS->top] += left1 + left2;
  pS->right[pS->top] += left1 + left2;
} // incTop
