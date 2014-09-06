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
#define NOOFDEL 300000

int deletethis(char *, int);
char **toberemoved;

int main(int argc, char **argv){

  FILE *fasta;
  FILE *del;
  FILE *clean;
  FILE *quarantine;
  char str[150];
  int delcnt;
  int i;
  int wflag;
  char ch;
  int deleted;
  int dumpclean;


  if (argc != 3 && argc!=4){
    fprintf(stderr, "Given a fasta file and a file of list of sequences to be removed\nit deletes those sequences from that fasta file.\nLast update: Nov 7, 2005.\n");
    fprintf(stderr, "Usage: %s [fastafile] [listtoremove] <-noclean>\n", argv[0]);
    return 0;
  }

  if (argc == 4){
    if (!strcmp(argv[3],"-noclean"))
      dumpclean=0;
    else
      dumpclean=1;
  }
  else
    dumpclean=1;

  fasta = fopen(argv[1], "r");
  del   = fopen(argv[2], "r");

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

  i=0;
  while (fgets(str, 150, del) > 0){
    str[strlen(str)-1] = 0;
    strcpy(toberemoved[i++], str);
  }
  delcnt = i;
  
  fprintf(stderr, "%d sequences will be deleted.\n", delcnt);
  
  wflag = 1;
  deleted = 0;
  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch == '>'){
      /*
	if (!dumpclean && deleted == delcnt)
	break;
      */
      fgets(str, 150, fasta);
      str[strlen(str)-1] = 0;
      if (deletethis(str, delcnt)){
	deleted++;
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
  return 1;    
}


int deletethis(char *str, int delcnt){
  int i;
  
  for (i=0;i<delcnt;i++){
    if (strstr(str, toberemoved[i]))
      return 1;
  }
  
  return 0;
}
