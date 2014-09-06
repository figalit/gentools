#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX 2000000
#define DELETED -1

typedef struct _set *Set;
struct _set{
  int val;
  Set next;
};

struct _node{
  Set set;
  int secondint;
};

typedef struct _node Node;

void addSet(Set *, int);
int isSame(Set, Set);
void sort(Node **, int);
int isIncluded(Set, int);
int getLast(Node);
int CountSet(Set);
int check(Node *, Set, int, int, int);
Node *elimSubset(Node *, int, int *);
int subset(Set, Set);
Node *consolidate(Node *, int, int *);

Node *FirstGraph;
int firstCount;

main(int argc, char **argv){
  FILE *in;
  FILE *out;
  char outfname[100];
  Node *graph;
  Node *pGraph;
  int i, j, k;
  int first, second;
  int count=0;
  int maxseq;
  int notFinished = 1;
  int current;
  int forbreak;
  int foundone;
  int foundk;
  int setCnt;
  Set printSet;
  int **resultTable;
  int rowCnt;
  int maxRow;
  int loop=1;
  if (argc != 3){
    printf("%s PairsFile NumOfSequences\n",argv[0]);
    exit(0);
  }
  sprintf(outfname, "%s.cliques", argv[1]);
  if ((graph = (struct _node *)malloc(sizeof(struct _node) * MAX)) == NULL)
    printf("graph\n");
  if ((FirstGraph = (struct _node *)malloc(sizeof(struct _node) * MAX))==NULL)
    printf("fgraph\n");
  //pGraph = (struct _node *)malloc(sizeof(struct _node) * MAX);
  in = fopen(argv[1], "r");
  maxseq = atoi(argv[2]);
  while(fscanf(in, "%d %d", &first, &second) > 0){
    addSet(&(graph[count]).set, first);
    graph[count].secondint = second;
    addSet(&(FirstGraph[count]).set, first);
    FirstGraph[count++].secondint = second;
  } // while
  
  //graph = (struct _node *)realloc(graph, sizeof(struct _node) * (count+1));
  //FirstGraph = (struct _node *)realloc(FirstGraph, sizeof(struct _node) * (count+1));
  
  printf("count: %d\n", count);
  firstCount = count;
  while (notFinished){  // start generating set
    printf("loop: %d\n",loop++);
    sort(&graph, count);
    notFinished = 0;
    for (i=0;i<count;i++){
      forbreak = 0;
      current = graph[i].set->val;
      setCnt = CountSet(graph[i].set);
      for (j=0/*graph[i].secondint+1*/; j<maxseq; j++){
	foundone = 0;
	if (check(FirstGraph, graph[i].set, j, count, graph[i].secondint)){
	  addSet(&(graph[i].set), graph[i].secondint);
	  addSet(&(graph[i].set), j);
	  graph[i].secondint=j;
	  forbreak=1;
	  notFinished=1;
	  break;
	} //  if check	    
	if (forbreak)
	  break;
      } // for j
    } // for i  
  } // while(1)
  sort(&graph, count);
  pGraph = elimSubset(graph, count, &setCnt);
  count = setCnt;
  free(graph);
  pGraph = consolidate(pGraph, count, &setCnt);
  free(FirstGraph);
  for (i=0;i<setCnt;i++){
    for(j=i+1;j<setCnt;j++){
      if (isSame(pGraph[i].set, pGraph[j].set))
	pGraph[i].secondint=DELETED;
    }
  }

  maxRow = 0;
  count = 0;
  out = fopen(outfname, "w");
  for (i=0;i<setCnt;i++){
    if(pGraph[i].secondint != DELETED){
      printSet = pGraph[i].set;
      rowCnt = 0;
      count++;
      if (printSet->next != NULL){
	while (printSet!=NULL){
	  rowCnt++;
	  fprintf(out, "%d ", printSet->val);
	  printSet = printSet->next;
	}// while
	fprintf(out,"\n");
      } // if printset
      if (rowCnt > maxRow)
	maxRow = rowCnt;
    } // if pgraph
  } // for
} // main

int isSame(Set s1, Set s2){
  int notfound = 1;
  Set t1 = s1;
  Set t2 = s2;
  while (t1 != NULL){
    t2 = s2;
    notfound = 1;
    while (t2 != NULL){
      if (t1->val == t2->val)
	notfound = 0;
      t2 = t2->next;
    }
    if (notfound)
      return 0;
    t1 = t1->next;
  }
  return 1;  
} // isSame

int isIncluded(Set s1, int val){
  Set t1 = s1;
  if (t1 == NULL)
    return 0;
  while (t1 != NULL){
    if (t1->val == val)
      return 1;
    t1 = t1->next;
  }
  return 0;
}

void addSet(Set *s, int val){
  Set t1;
  Set t2;
  t1 = *s;
  //if ((*s) != NULL)
  //if (isIncluded(t1, val))
  // return;
  /*  if ((t1 = (struct _set *) malloc(sizeof(struct _set))) == NULL){
    printf("mem\n");
    exit(0);
  }
  t1->val = val;
  t1->next = NULL; */
  if (*s == NULL){
    // *s = t1;
    (*s) = (struct _set *) malloc(sizeof(struct _set));
    (*s)->val = val;
    (*s)->next = NULL;
  }
  else{
    t2 = *s;
    while (t2->next != NULL && t2->next->val < val){
      if (t2->val==val)
	return;
      t2 = t2->next;
    }
    //t2->next = t1; // (*s)->next; 
    if (t2->val==val)
      return;

    if (t2->next == NULL){
      t2->next = (struct _set *) malloc(sizeof(struct _set));
      t2->next->val = val;
      t2->next->next = NULL;
    }  
    else {
      if (t2->next->val==val)
	return;
      t1 = t2->next;
      t2->next = (struct _set *) malloc(sizeof(struct _set));
      t2->next->val = val;
      t2->next->next = t1;      
    }
  } // else
} // addSet

void sort(Node **graph, int cnt){
  int i, j;
  Node temp;
  for (i=0; i<cnt; i++)
    for (j=0; j<cnt; j++){
      if (((((*graph)[i]).set)->val) < ((((*graph)[j]).set)->val) && i>j){
	temp = (*graph)[i];
	(*graph)[i] = (*graph)[j];
	(*graph)[j] = temp;
      } // if
    } // for j
  
} // sort

int getLast(Node node){
  Set tmp = node.set;
  while (tmp->next != NULL)
    tmp = tmp->next;
  return tmp->val;
} // getLast

int CountSet(Set set){
  Set tmp = set;
  int i=0;
  while (tmp!=NULL){
    i++;
    tmp = tmp->next;
  } // while
  return i;
} // CountSet

int check(Node *FirstGraph, Set set, int val, int count, int secondint){
  int temp = 0;
  int i;
  Set t1 = set;
  int small, big;
  int second=0;
  while (t1 != NULL){
    if (val > t1->val){
      small = t1->val;
      big = val;
    } // if
    else{
      small = val;
      big = t1->val;      
    } // else
    temp = 0;
    for (i=0;i<count;i++){
      if (FirstGraph[i].set->val==small && FirstGraph[i].secondint==big){
	temp = 1;
	break;
      }
    } // for
    if (!temp)
      return 0;
    t1 = t1->next;
  } // while
  if (secondint == val)
    return 1;
  for (i=0;i<count;i++){
    if (FirstGraph[i].set->val==secondint && FirstGraph[i].secondint==val){
      return 1;
    }
  }
  return 0;
} // check

Node *elimSubset(Node *graph, int count, int *setCnt){
  int i, j;
  int add=0;
  int subsetcheck;
  Node *ret = (struct _node *)malloc(sizeof(struct _node) * count); 
  for (i=0;i<count;i++){
    subsetcheck=0;
    for (j=0;j<count;j++){
      if (i!=j && subset(graph[i].set,graph[j].set)){
	subsetcheck=1;
	break;
      } // if
    } // for j
    if (!subsetcheck){
      ret[add].set = graph[i].set;
      ret[add++].secondint = -2; // value unimportant
    } // if subsetcheck
  } // for i
  *setCnt = add;
  ret = (struct _node *)realloc(ret, sizeof(struct _node) * add); 
  return ret;
} // elimSubset

/*
Node *consolidate(Node *graph, int count, int *setCnt){
  int i, j;
  Node *ret = (struct _node *)malloc(sizeof(struct _node) * MAX);
  int add=0;
  Set testSet;
  Set t1; 
  Set t2;
  Set t3;
  int combine;
  int found;
  for (i=0;i<count;i++){
    sort (&ret, add);
    for (j=0;j<count;j++){
      if (i != j){
	testSet = NULL;
	t1 = graph[i].set;
	while (t1 != NULL){
	  t2 = graph[j].set;
	  found = 0;
	  while (t2 != NULL){
	    if (t1->val == t2->val)
	      found = 1;
	    t2 = t2->next;
	  } // while t2
	  if (!found)
	    addSet(&testSet, t1->val);
	  t1 = t1->next;
	} // while t1
	t3 = testSet;
	combine = 1;
	while (t3 != NULL){
	  if (!check(FirstGraph, graph[j].set, t3->val, firstCount, t3->val))
	    combine = 0;  
	  t3 = t3->next;
	} // while
	t1 = t2 = t3 = NULL;
	//free(t1); free(t2); free(t3);
	if (combine){
	  while (testSet != NULL){
	    addSet(&(ret[add].set), testSet->val);
	    testSet = testSet->next;
	  } // while testSet;	  
	  t2 = graph[j].set;
	  while (t2 != NULL){
	    addSet(&(ret[add].set), t2->val);
	    t2 = t2->next;
	  } // while t2     
	  ret[add++].secondint = -2;
	} // if combine
      } // if i!=j
      //ret[add].set = graph[j].set;
    } // for j
  } // for i
  *setCnt = add;
  ret = (struct _node *)realloc(ret, sizeof(struct _node) * add); 
  return ret;
} // consolidate

*/

int subset(Set s1, Set s2){
  Set t1 = s1;
  Set t2 = s2;
  
  while (t1 != NULL){
    if (!isIncluded(t2, t1->val))
      return 0;
    t1 = t1->next;
  } // while t1
  return 1;
} // subset


Node *consolidate(Node *graph, int count, int *setCnt){
  int notFinished = 1;
  int i, j;
  int add=0;
  Set t1;
  Set t2;
  int forbreak;
  while (notFinished){  // start generating set
    sort(&graph, count);
    notFinished = 0;
    for (i=0;i<count;i++){
      forbreak = 0;
      for (j=0;j<count;j++){
	if (i != j){
	  t2 = graph[j].set;
	  while (t2 != NULL){
	    if (!isIncluded(graph[i].set,t2->val)){
	      if (check(FirstGraph, graph[i].set, t2->val, firstCount, t2->val)){
		addSet(&(graph[i].set), t2->val);
		addSet(&(graph[i].set), j);
		forbreak=1;
		notFinished=1;
		break;
	      } //  if check	    
	    } // if !isIncluded
	    t2 = t2->next;
	  }// while t2 != NULL
	} // if i!=j
	if (forbreak)
	  break;
      } // for j
    } // for i
  } //while notfinished
  return graph;
}
