#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define REMOVED "REMOVED"
#define MAX 300

typedef struct sv{

  char chr[10];
  int s,e;
  char call [20];
  
  int span;
  int sup;
  
  float prob;

  struct sv *next;

}_sv;


void alloc_sv(struct sv **);
void insert_sv(struct sv **, char *, int, int, char *, int, int, float);
int merge(struct sv *, struct sv *);
int overlap(struct sv *, struct sv *);

int main(){

  char chr[50];
  int s, e;
  char call[20];
  int span;
  int sup;
  int nc;
  float prob;
  
  struct sv *root=NULL;
  struct sv *one=NULL;
  struct sv *two=NULL;

  int converged=0;
  long double score;

  nc = 0;
  while (fscanf(stdin, "%s\t%d\t%d\t%s\t%d\t%d\t%f\n", chr, &s, &e, call, &span, &sup, &prob) > 0){

    insert_sv(&root, chr, s, e, call, span, sup, prob);
    nc++;
  }


  fprintf(stderr, "%d sites loaded\n", nc);

  while (!converged){
    converged = 1;
    one = root;

    while (one != NULL){

      two = one->next;

      if (!strcmp(one->call, REMOVED)){
	one = one->next;
	continue;
      }

      while (two != NULL){
    
      if (!strcmp(two->call, REMOVED)){
	two = two->next;
	continue;
      }


	if (one==two || strcmp(one->call, two->call)){
	  two = two->next;
	  continue;
	}

	if (overlap(one, two)){
	  converged=0;
	  merge(one, two);
	  nc--;
	  fprintf(stderr, "%d remains.\n", nc);
	}

	two = two->next;
      }

      one = one->next;
    }
   
  }

  one = root;

  while (one!=NULL){
    
    if (strcmp(one->call, REMOVED) && one->sup>=2 && one->prob>1.5){
      score = 1 - pow((1-one->prob/one->sup), one->sup);
      fprintf(stdout, "%s\t%d\t%d\t%s\t%d\t%d\t%f\t%llf\n", one->chr, one->s, one->e, one->call, one->span, one->sup, one->prob, score);
    }
    one = one->next;
  }

}

void insert_sv(struct sv **root, char *chr, int s, int e, char *call, int span, int sup, float prob){

  struct sv *new;


  if ((*root) == NULL){
    alloc_sv(root);
    (*root)->s = s;
    (*root)->e = e;
    (*root)->sup = sup;
    (*root)->span = span;
    (*root)->prob = prob;
    //(*root)->span = e-s+1;
    strcpy((*root)->chr, chr);
    strcpy((*root)->call, call);
    
    (*root)->next = NULL;
    return;
  }

  alloc_sv(&new);
  new->s = s; 
  new->e = e; 
  new->sup = sup; 
  new->prob = prob; 
  //  new->span = e-s+1; 
  new->span = span; 
  strcpy(new->chr, chr);
  strcpy(new->call, call);


  new->next = *root;
  *root = new;
}


void alloc_sv(struct sv **loc){
  (*loc) = (struct sv *) malloc (sizeof(struct sv));
  (*loc)->next = NULL;
  (*loc)->s = 0;
  (*loc)->e = 0;
  (*loc)->chr[0] = 0;
  (*loc)->span = 0;
  (*loc)->sup = 0;
  (*loc)->prob = 0.0;
  (*loc)->call[0] = 0;
	
}



int overlap(struct sv *one, struct sv *two){
  long double fscore1, fscore2;



  if (strcmp(one->chr, two->chr))
    return 0;

  if (strcmp(one->call, two->call))
    return 0;


  /*
  if (one->span < 1000 && two->span < 1000 && abs(one->span - two->span) > MAX)
    return 0;
  */

  //os ts oe te
  //os ts te oe


  fscore1 = 1 / (one->span * ( 1 - pow ((1 - one->prob/one->sup), one->sup)));
  fscore2 = 1 / (two->span * ( 1 - pow ((1 - two->prob/two->sup), two->sup)));

  if (one->s <= two->s && one->e >= two->s){
    if (abs(one->span - two->span) > MAX){
      //if (one->prob > two->prob)
      if (fscore1 > fscore2)
	strcpy(two->call, REMOVED);
      else if (fscore1 < fscore2)
	//else if (one->prob < two->prob)
	strcpy(one->call, REMOVED);
      else {
	if (one->span < two->span)
	  strcpy(two->call, REMOVED);
	else
	  strcpy(one->call, REMOVED);
      }
    }
    return 1;
  }

  //ts os oe te
  //ts os te oe

  if (two->s <= one->s && two->e >= one->s){
    
    if (abs(one->span - two->span) > MAX){
      //      if (one->prob > two->prob)
      if (fscore1 > fscore2)
	strcpy(two->call, REMOVED);
      else if (fscore1 < fscore2)
	//      else if (one->prob < two->prob)
	strcpy(one->call, REMOVED);
      else {
	if (one->span < two->span)
	  strcpy(two->call, REMOVED);
	else
	  strcpy(one->call, REMOVED);
      }
    }
    return 1;
  }  

  
  /*  if (abs(one->s - two->s) <= MAX && abs(one->e - two->e) <= MAX)
    return 1;
  */
  return 0;

}


int merge(struct sv *one, struct sv *two){
  
  int s, e;
  int span;
  int sup;
  float prob;



  /*
    fprintf(stderr, "merge:\n");
    fprintf(stderr, "%s\t%d\t%d\t%s\t%d\t%d\n", one->chr, one->s, one->e, one->call, one->span, one->sup);
    fprintf(stderr, "%s\t%d\t%d\t%s\t%d\t%d\n", two->chr, two->s, two->e, two->call, two->span, two->sup);
  */

  if (!strcmp(two->call, REMOVED) || !strcmp(one->call, REMOVED))
    return;

  span = (one->span + two->span) / 2;
  sup = one->sup + two->sup;
  prob = one->prob + two->prob;
  
  

  // os ts oe te
  // os ts te oe
  if (one->s <= two->s){
    if (strcmp(one->call, "inversion"))
      s = two->s;
    else
      s = one->s;      
    if (one->e <= two->e){
      // os ts oe te
      if (strcmp(one->call, "inversion"))
	e = one->e;
      else
	e = two->e;
    }
    else{
      // os ts te oe
      if (strcmp(one->call, "inversion"))
	e = two->e;
      else
	e = one->e;
    }
  }

  // ts os te oe
  // ts os oe te
  else {
    if (strcmp(one->call, "inversion"))
      s = one->s;
    else
      s = two->s;
    if (two->e <= one->e){
      // ts os te oe
      if (strcmp(one->call, "inversion"))
	e = two->e;
      else
	e = one->e;
    }
    else{
      // ts os oe te
      if (strcmp(one->call, "inversion"))
	e = one->e;
      else	
	e = two->e;
    }
  }

  one->sup = sup;
  one->span = span;
  one->prob = prob;
  one->s = s;
  one->e = e;

  
  // dump two
  strcpy(two->call, REMOVED);
}
