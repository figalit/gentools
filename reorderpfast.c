/* 
   given a pfast results.out.txt file
   reorder hits using their paired ends
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct f_end{
  char chr[100];
  char name[100];
  int cstart;
  int cend;
  int fstart;
  int fend;
  int wins;
  char orient;
  struct f_end *next;
}_f_end;

typedef struct fosmid{
  char clonename[100];
  struct f_end *forward;
  struct f_end *reverse;  
  struct fosmid *next;
}_fosmid;



void getclonename(char cn[], char n[]);
void *mymalloc(size_t size);
void alloc_fend(struct f_end **fend);
void alloc_fosmid(struct fosmid **fos);
void insert_clone(struct fosmid **allfosmids, char name[], int cstart, int cend, 
		  int fstart, int fend, int wins, char orient, char chr[]);
int isforward(char n[]);
void reorder(struct fosmid *allfosmids, FILE *);

int main(int argc, char **argv){
  char name[100];
  int cstart, cend, fstart, fend, wins;
  int pwins;
  int windiff;
  int thisdiff;
  char orient;
  char chr[100];
  char prev[100];
  int cnt;
  int i;
  FILE *in;
  FILE *out;
  char clonename[100];
  struct fosmid *allfosmids;

  prev[0] = 0;
  cnt = 1;

  if (argc != 3)
    return 0;

  in = fopen(argv[1], "r");
  out = fopen(argv[2], "w");
  pwins = 0;
  windiff = 0;
  
  if (in == NULL || out == NULL)
    return 0;

  allfosmids = NULL;

  while(fscanf(in, "%s %d %d %d %d %d %c %s", name, &cstart, &cend, &fstart, &fend, 
	      &wins, &orient, chr) > 0){
    fprintf(stderr, "\r%d", cnt++);
    insert_clone(&allfosmids, name, cstart, cend, fstart, fend, wins, orient, chr);
  }

  fprintf(stderr, "\n");

  reorder(allfosmids, out);

  return 1;
  
  
}

void alloc_fend(struct f_end **fend){
  (*fend) = (struct f_end *) mymalloc(sizeof(struct f_end));
  (*fend)->next = NULL;
  (*fend)->cstart = 0; 
  (*fend)->cend = 0;
  (*fend)->fstart = 0;
  (*fend)->fend = 0;
  (*fend)->wins = 0;
  (*fend)->orient = '\0';
  (*fend)->chr[0] = '\0';
  (*fend)->name[0] = '\0';
}

void alloc_fosmid(struct fosmid **fos){
  (*fos) = (struct fosmid *) mymalloc(sizeof(struct fosmid));
  (*fos)->next = NULL;
  (*fos)->forward = NULL;
  (*fos)->reverse = NULL;
  (*fos)->clonename[0] = '\0';
}


void *mymalloc(size_t size){
  void *ret;
  ret = malloc(size);
  
  if (ret == NULL){
    fprintf(stderr, "Insufficient memory.\n");
    exit(0);
  }
  return ret;
}

void getclonename(char cn[], char n[]){
  int i,j;

  i=0;
  j=0;
  while(n[i] != 'F' && n[i] != 'R'){
    cn[j++] = n[i++];
  }

  i++; //pass F or R

  while(n[i] != '.'){
    cn[j++] = n[i++];
  }  

  cn[j] = 0;
  
}


int isforward(char n[]){
  int i;
  
  i=0;

  while (n[i] != 'F' && n[i] != 'R')
    i++;

  if (n[i] == 'F')
    return 1;

  return 0;

}

void insert_clone(struct fosmid **allfosmids, char name[], int cstart, int cend, 
		  int fstart, int fend, int wins, char orient, char chr[]){
  char clonename[100];
  int isForward;
  struct fosmid *current;
  struct fosmid *new;
  struct f_end *newend;

  getclonename(clonename, name);
  
  isForward = isforward(name);

  if (*allfosmids == NULL){
    alloc_fosmid(allfosmids);
    strcpy((*allfosmids)->clonename, clonename);
    if (isForward){
      alloc_fend(&((*allfosmids)->forward));
      (*allfosmids)->forward->cstart = cstart;
      (*allfosmids)->forward->cend = cend;
      (*allfosmids)->forward->fstart = fstart;
      (*allfosmids)->forward->fend = fend;
      (*allfosmids)->forward->orient = orient;
      (*allfosmids)->forward->wins = wins;
      strcpy((*allfosmids)->forward->chr, chr);
      strcpy((*allfosmids)->forward->name, name);
    }
    else{
      alloc_fend(&((*allfosmids)->reverse));
      (*allfosmids)->reverse->cstart = cstart;
      (*allfosmids)->reverse->cend = cend;
      (*allfosmids)->reverse->fstart = fstart;
      (*allfosmids)->reverse->fend = fend;
      (*allfosmids)->reverse->orient = orient;
      (*allfosmids)->reverse->wins = wins;
      strcpy((*allfosmids)->reverse->chr, chr);
      strcpy((*allfosmids)->reverse->name, name);
    }
    return;
  }

  current = (*allfosmids);

  while (current!=NULL){
    if (!strcmp(current->clonename, clonename))
      break;
    current = current->next;
  }

  alloc_fend(&newend);
  newend->cstart = cstart;
  newend->fstart = fstart;
  newend->cend = cend;
  newend->fend = fend;
  newend->orient = orient;
  newend->wins = wins;
  strcpy(newend->chr, chr);
  strcpy(newend->name, name);

  if (current != NULL){
    if (isForward){
      newend->next = current->forward;
      current->forward = newend;
    }
    else{
      newend->next = current->reverse;
      current->reverse = newend;
    }

    return;

  }
  

  alloc_fosmid(&new);
  if (isForward)
    new->forward = newend;
  
  else
    new->reverse = newend;
    
  new->next = (*allfosmids);
  (*allfosmids) = new;
  

}

void reorder(struct fosmid *allfosmids, FILE *out){
  struct fosmid *current = allfosmids;
  struct f_end *forward;
  struct f_end *reverse;
 
  while (current != NULL){
    forward = current->forward;
    while (forward != NULL){
      reverse = current->reverse;
      while (reverse != NULL){
	if (!strcmp(forward->chr, reverse->chr) && 
	    abs(forward->cstart-reverse->cstart) < 100000 &&
	    abs(forward->cstart-reverse->cstart) > 10000 )
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", forward->name, forward->cstart, forward->cend, forward->fstart, forward->fend, forward->wins, forward->orient, forward->chr);
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", reverse->name, reverse->cstart, reverse->cend, reverse->fstart, reverse->fend, reverse->wins, reverse->orient, reverse->chr);
	reverse = reverse->next;
      }
      forward = forward->next;
    }
    current = current->next;
  }

 
}
