/* 
   fastadel.c
   Given a fasta file and a file of list of sequences to be removed
   it deletes those sequences from that fasta file
   Last update: Nov 7, 2005
   Can Alkan
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define NOOFDEL 200000


typedef struct dmember{
  char name[150];
  struct dmember *next;
}sdmember;



//int deletethis(char *, int);
int deletethis(char *);
char **toberemoved;
int ONCE;
void alloc_dmember(struct dmember **);
void my_fgets(char *, int , FILE *);
struct dmember *dlist;

int main(int argc, char **argv){

  FILE *fasta;
  FILE *del;
  FILE *clean;
  FILE *quarantine;
  char str[150];
  char str2[150];
  int delcnt;
  int i;
  int wflag;
  char ch;
  int deleted;
  int dumpclean;
  struct dmember *current;
  struct dmember *prev;
  int delete;
  int exact=0;

  ONCE = 0;
  dumpclean = 1;

  if (argc != 3 && argc!=4 && argc!=5){
    fprintf(stderr, "Given a fasta file and a file of list of sequences to be removed\nit deletes those sequences from that fasta file.\nLast update: Nov 7, 2005.\n");
    fprintf(stderr, "Usage: %s [fastafile] [listtoremove] <-noclean> <-once>\n", argv[0]);
    return 0;
  }

  if (argc == 4){
    if (!strcmp(argv[3],"-noclean"))
      dumpclean=0;
    else if (!strcmp(argv[3],"-once"))
      ONCE = 1;
    else if (!strcmp(argv[3],"-exact"))
      exact = 1;
    else
      dumpclean=1;
  }
  else if (argc == 5){
    if (!strcmp(argv[3],"-noclean"))
      dumpclean=0;
    else if (!strcmp(argv[3],"-once"))
      ONCE = 1;
    else if (!strcmp(argv[3],"-exact"))
      exact = 1;
    if (!strcmp(argv[4],"-noclean"))
      dumpclean=0;
    else if (!strcmp(argv[4],"-once"))
      ONCE = 1;
    else if (!strcmp(argv[4],"-exact"))
      exact = 1;
  }
  else if (argc == 6){
    if (!strcmp(argv[3],"-noclean"))
      dumpclean=0;
    else if (!strcmp(argv[3],"-once"))
      ONCE = 1;
    else if (!strcmp(argv[3],"-exact"))
      exact = 1;
    if (!strcmp(argv[4],"-noclean"))
      dumpclean=0;
    else if (!strcmp(argv[4],"-once"))
      ONCE = 1;
    else if (!strcmp(argv[4],"-exact"))
      exact = 1;
    if (!strcmp(argv[5],"-noclean"))
      dumpclean=0;
    else if (!strcmp(argv[5],"-once"))
      ONCE = 1;
    else if (!strcmp(argv[5],"-exact"))
      exact = 1;
  }
  else
    dumpclean=1;

  fasta = fopen(argv[1], "r");
  del   = fopen(argv[2], "r");

  dlist = NULL;

  if (fasta == NULL) {
    fprintf(stderr, "Fasta File opening error.\n");
    return 0;
  }

  if (del == NULL) {
    fprintf(stderr, "Delete File opening error.\n");
    return 0;
  }

  if (dumpclean){
    sprintf(str, "%s.%s.clean.fa", argv[1], argv[2]);
    clean = fopen(str, "w"); 
  }
  sprintf(str, "%s.%s.quarantine.fa", argv[1], argv[2]);

  /* check if exists */
  quarantine = fopen(str, "r");
  if (quarantine != NULL){
    fprintf(stderr, "%s already exists, skipping.\n", str);
    return 0;
  }
  quarantine = fopen(str, "w");
  
  toberemoved = (char **) malloc(NOOFDEL * sizeof(char *));
  for (i=0;i<NOOFDEL;i++){
    toberemoved[i] = (char *) malloc(150 * sizeof(char));
  }

  i=0; delcnt=0;
  while (fgets(str, 150, del) > 0){
    str[strlen(str)-1] = 0;
    if (str[0]==0)
      continue;
    delcnt++;
    if (dlist == NULL){
      alloc_dmember(&dlist);
      printf("to be del: %s\n", str);
      strcpy(dlist->name, str);
      continue;
    }
    // else
    
    prev = dlist;
    alloc_dmember(&current);
    strcpy(current->name, str);
    current->next = prev;
    dlist = current;
  }
  //delcnt = i;
  
  fprintf(stderr, "%d sequences will be deleted.\n\n", delcnt);
  
  wflag = 1;
  deleted = 0;
  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch == '>'){
      /*
	if (!dumpclean && deleted == delcnt)
	break;
      */
      //my_fgets(str, 150, fasta);
      fgets(str, 150, fasta);
      str[strlen(str)-1] = 0;
      delete=0;
      //if (deletethis(str)){
      current = dlist;
      prev = current;
      while (current != NULL){
	if (!exact){
	  if( strstr(str, current->name)){
	    //if (!strcmp(str, current->name)){
	    if (ONCE && current!=dlist){
	      prev->next = current->next;
	      free(current);
	    }
	    //printf("%s\t-- %s\n", str, current->name);
	    //getchar();
	    delete=1;
	    break;
	  }
	}
	else {
	  
	  //sscanf(str, "%s", str2);
	  if (!strcmp(str, current->name)){
	    if (ONCE && current!=dlist){
	      prev->next = current->next;
	      free(current);
	    }
	    delete=1;
	    break;
	  }
	}
	prev = current;
	current = current->next;
      }
      
      if (delete){
	deleted++;
	fprintf(stderr, "\rDeleted %d of %d", deleted, delcnt);
	fprintf(quarantine, ">%s\n", str);
	wflag = 0;
      }
      
      else{
	if (dumpclean)
	  fprintf(clean, ">%s\n", str);
	wflag = 1;
      }
      
    }
    if (wflag && ch != '>'){
      if (dumpclean)
	fprintf(clean, "%c", ch);
      else ;
    }
    else if (ch!='>')
      fprintf(quarantine, "%c", ch);
  }
  fprintf(stderr, "\n");
  return 1;    
}


/*
int deletethis(char *str, int delcnt){
  int i;
  
  for (i=0;i<delcnt;i++){
    if (strstr(str, toberemoved[i]))
      return 1;
  }
  
  return 0;
}
*/

/*
int deletethis(char *str, struct dmember **current){
  int i;
  struct dmember *prev;
  
  //  current = dlist;
  prev = *current;
  while ((*current) != NULL){
    if (strstr(str, (*current)->name)){
      if (ONCE && (*current)!=dlist){
	prev->next = current->next;
	free(current);
      }
      return 1;
    }
    prev = *current;
    current = current->next;
  }
 
  return 0;
}
*/

void alloc_dmember(struct dmember **dm){
  (*dm) = (struct dmember *) malloc (sizeof(struct dmember));
  (*dm)->next = NULL;
}

void my_fgets(char *str, int length, FILE *in){
  char ch;
  int i=0;
  while (i<length && fscanf(in, "%c", &ch)){
    if ((ch==' '  || ch=='\t')  && i!=0){
      if (str[i-1] == ' ' || str[i] == '\t')
	;
      else
	str[i++] = ch;
    }
    else
      str[i++] = ch;
    if (ch == '\n' || ch=='\r')
      break;
  }
  str[i] = 0;
}
