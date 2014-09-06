#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define MAX 150000
typedef int boolean;
typedef struct STCK{
  int *left, *right;
  int *start;
  int *marked;
  int *end;
  int top, size;
}STACK;
STACK *stk;
/*  functions */
STACK  *createstack(int );
void clear(STACK *);
boolean isEmpty(STACK *);
boolean pop(STACK *, int *, int *, int *);
void push(int, int, int, int, STACK *);
void mark(STACK *);
boolean isMarked(STACK *);
boolean incRight(STACK *pS, int end);
void incTop(int, int, STACK *);
