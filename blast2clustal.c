#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXSEQ 50
#define SEQLEN 200000
#define FORWARD 0
#define BACKWARD 1

struct _sequences{
  char name[100];
  int start;
  int end;
  int direction;
  char seq[SEQLEN];
}; // struct

typedef struct _sequences SEQS;
int debug = 0;
SEQS seqs[MAXSEQ];
void addSeq(char [], int, char [], int, char []);
void printOut(int, char *);

main(int argc, char **argv){
  int seqTot;
  int i, j, k;
  int cnt;
  FILE *in;
  char outfname[50];
  FILE *out;
  char temp[100];
  char *query;
  char portion[61];
  int start, end;
  int nlinecnt;
  char ch;
  char name[100];
  int pos;
  int startpos;
  if (argc != 3){
    printf("%s [BlastN File] [debug]\n", argv[0]);
    exit(0);
  } // if
  if ((in=fopen(argv[1], "r")) == NULL){
    printf("%s cannot be opened\n",argv[1]);
    exit(0);
  }
  debug = atoi(argv[2]);
  strcpy(temp, "temp");
  cnt = 0;
  seqTot = 0;
  query = (char *) malloc(100);
  for (i=0; i<MAXSEQ; i++){
    strcpy(seqs[i].name, "n");
    //for (j=0; j< SEQLEN-1; j++)
    //seqs[i].seq[j] = 'N';
    seqs[i].seq[0] = 'X';
    seqs[i].start = 0;
    seqs[i].end = 0;
    seqs[i].direction = FORWARD;    
  } // for initialization

  while (strcmp(temp,"QUERY"))
    fscanf(in, "%s", temp);

  fscanf(in, "%d %s %d", &start, query, &end);
  startpos = 16;
  while (fscanf(in, "%s", temp) > 0){
    //printf("%s\n",temp);
    if (!strcmp(temp, "QUERY")){
      fscanf(in, "%d %s %d", &start, query, &end);
      //query = cleanQuery(query);
    } // if query
    else if (!strcmp(temp, "\\")){ 
      nlinecnt = 0;
      while (nlinecnt != 3){  // skip insertions
	fscanf(in, "%c", &ch);
	if (ch == '\n')
	  nlinecnt++;
      } //  while nlinecnt!=3
    } // else if
    else if (!strcmp(temp, "Database:"))
      break; // end of file
    else {
      strcpy(name, temp);
      fscanf(in, "%d", &start);
      sprintf(temp,  "%d", start);
      pos = 8 + strlen(temp) + 1;
      //printf("startpos: %d\n", start);
      while (pos != startpos){
	fscanf(in, "%c", &ch);
	pos++;
      }
      for (k=0; k < strlen(query); k++)
	fscanf(in, "%c", &(portion[k]));
      portion[k] = 0;
      fscanf(in, "%d", &end);
      //printf("endpos: %d\n", end);
      addSeq(name, start, portion, end, query);
      if (debug)
	getchar();
      //return;
    } // else  sequence
  } // while
  printOut(21, argv[1]);
} // main

void addSeq(char name[], int start, char portion[], int end, char query[]){
  int i, j, k;
  int found;
  int pos;
  int pad;
  int normal = 1;
  int damn;
  int newcreation = 0;

  if (!strcmp(name,"AC005632") && start==24587){
    //   debug=1;
    printf("debug mode\n");
  }

  if (debug)
    printf("%s\n", portion);

  for (i=0; i<MAXSEQ; i++){
    printf("i: %d %s\n",i, seqs[i].name);
    if (!strcmp(seqs[i].name, "n")) { // if not allocated
      found = 0;
      break;
    } // if not allocated
    else if (!strcmp(seqs[i].name, name)){
      found = 1;
      if (seqs[i].direction == FORWARD){
	pos = start - seqs[i].start;
      } // if forward
      else {
	pos = seqs[i].start - start;
      } // if backward
      break;
    } // else if
  } // for
  
  if (found){
    //if ((start>end && start<seqs[i].end-1000) || (start<end && start>seqs[i].end+1000))
    if (abs(start-seqs[i].start) > 1500)
      return; // throw this portion away
  }
  
  if (found && ((start > end && seqs[i].direction==FORWARD) || (start<end && seqs[i].direction==BACKWARD))){
    strcat(name,".2");
    for (i=0; i<MAXSEQ; i++){
      if (!strcmp(seqs[i].name, "n")) { // if not allocated
	found = 0;
	break;
      } // if not allocated
      else if (!strcmp(seqs[i].name, name)){
	found = 1;
	if (seqs[i].direction == FORWARD){
	  pos = start - seqs[i].start;
	} // if forward
	else {
	  pos = seqs[i].start - start;
	} // if backward
	break;
      } // else if
    } // for
  } // if exception

  if (!found){
    strcpy(seqs[i].name, name);
    //seqs[i].start = start;
    if (start < end){
      seqs[i].direction = FORWARD;
      //pos = start - seqs[i].start;
    }
    else{
      seqs[i].direction = BACKWARD;
      //pos = seqs[i].start - start;
    }
    if (mystrlen(seqs[0].seq) > 0){
      printf("PADDING for %s size %d\n",seqs[i].name,(mystrlen(seqs[0].seq)-strlen(query)));
      for (j=0;j<(mystrlen(seqs[0].seq)-strlen(query));j++)
	seqs[i].seq[j] = 'N';
      seqs[i].seq[j] = 'X';
    }
    printf("created %s\n", name);
    newcreation = 1;
  } // if new sequence
  //  pos = start;
  
  if (found){
    damn = mystrlen(seqs[i].seq);
    if (mystrlen(seqs[0].seq) > 0){
      printf("PADDING for %s size %d\n",seqs[i].name,(mystrlen(seqs[0].seq)-strlen(query)));
      for (j=damn;j<(mystrlen(seqs[0].seq)-strlen(query));j++){
	seqs[i].seq[j] = 'N';
	newcreation = 1;
      } // pad with n's
      seqs[i].seq[j] = 'X';
    }
  }

  if (seqs[i].direction == BACKWARD){
    if (end > seqs[i].end && seqs[i].start!=0){
      printf("abnormal calculation strlen: %d ", mystrlen(seqs[i].seq));
      damn = mystrlen(seqs[i].seq);
      pos = mystrlen(seqs[i].seq) + seqs[i].end - start - 1;
      printf("postart: %d ",pos);
      for (j=0;j<strlen(portion);j++){ // portion copy
	if (portion[j] == '.'){ // if agreement
	  seqs[i].seq[pos++] = toupper(query[j]);
	} // if agreement
	else if (portion[j] != ' '){ // empty char
	  seqs[i].seq[pos++] = toupper(portion[j]);
	} // else if not empty
      } // for j
      normal = 0;
      printf("posend: %d ",pos);
      damn = pos+(start-seqs[i].end+1)-strlen(portion);
      printf("damn: %d \n",damn);
      
      if (damn > pos){
	printf("yandan aman da yandan\n");
	for (k=damn; k<mystrlen(seqs[i].seq); k++)
	  seqs[i].seq[pos++] = seqs[i].seq[k];
      } 
      seqs[i].seq[damn] = 'X';
      printf("name: %s portion: %d start: %d pos: %d startpos: %d endpos: %d end: %d\n",name,strlen(portion), start, pos, seqs[i].start, seqs[i].end, end);  
      
    } // if end>prev_end
  } // direction == backward
  
  if (seqs[i].direction == FORWARD){
    if (end < seqs[i].end && seqs[i].start!=0){
      printf("abnormal calculation strlen: %d ", mystrlen(seqs[i].seq));
      damn = mystrlen(seqs[i].seq);
      pos = mystrlen(seqs[i].seq) + seqs[i].end - start + 2;
      printf("postart: %d ",pos);
      for (j=0;j<strlen(portion);j++){ // portion copy
	if (portion[j] == '.'){ // if agreement
	  seqs[i].seq[pos++] = toupper(query[j]);
	} // if agreement
	else if (portion[j] != ' '){ // empty char
	  seqs[i].seq[pos++] = toupper(portion[j]);
	} // else if not empty
      } // for j
      normal = 0;
      printf("posend: %d ",pos);
      damn = pos+(start-seqs[i].end+1)-strlen(portion);
      printf("damn: %d \n",damn);
      
      if (damn > pos){
	printf("yandan aman da yandan\n");
	for (k=damn; k<mystrlen(seqs[i].seq); k++)
	  seqs[i].seq[pos++] = seqs[i].seq[k];
      } 
      seqs[i].seq[damn] = 'X';
      printf("name: %s portion: %d start: %d pos: %d startpos: %d endpos: %d end: %d\n",name,strlen(portion), start, pos, seqs[i].start, seqs[i].end, end);  
      
    } // if end>prev_end
  } // direction == backward
  
  if (normal){
    if (seqs[i].end != 0 && (portion[0]!=' ' ^ seqs[i].direction==BACKWARD) && query[0]!='n' && query[strlen(query)-1]!='n'){
      pad = abs(start - seqs[i].end) - 1;
      damn = mystrlen(seqs[i].seq);
      printf("pad: %d   damn: %d\n", pad, damn);
      for (j=0;j<pad;j++){
	seqs[i].seq[damn+j] = 'N';
      }
      seqs[i].seq[damn+j] = 'X';
    } 
    pos = mystrlen(seqs[i].seq);
    printf("normal calculation pos : %d\n", pos);
    for (j=0;j<strlen(portion);j++){ // portion copy
      if (query[j] == 'n' && portion[j] == ' '){
	if (portion[strlen(query)] == ' ')
	  end++;
	else
	  start--;
      }
      //else{
	if (portion[j] == '.'){ // if agreement
	  seqs[i].seq[pos++] = toupper(query[j]);
	} // if agreement
	else if (portion[j] != ' '){ // empty char
	  seqs[i].seq[pos++] = toupper(portion[j]);
	} // else if not empty
	else if (portion[j] == ' ' && newcreation){
	  seqs[i].seq[pos++] = 'N';
	  printf("N");
	}
	else if (portion[j] == ' ' && !newcreation && portion[0] != ' ')
	  seqs[i].seq[pos++] = 'N'; 
	else if (portion[j] == ' ' && query[j]=='n')
	  seqs[i].seq[pos++] = 'N'; 
	//}
    } // for j
    printf("\nname: %s portion: %d start: %d pos: %d startpos: %d endpos: %d end: %d\n",name,strlen(portion), start, pos, seqs[i].start, seqs[i].end, end);  
    seqs[i].seq[pos] = 'X';
    seqs[i].start = start;
    seqs[i].end = end;
  } // if normal
  if (pos >= SEQLEN)
    debug=1;
} // addSeq 



void printOut(int seqTot, char *inFile){
  int i, j, k, cnt;
  FILE *out;
  char fname[50];
  int nameSize=0;
  int endpoint;
  int maxlen;
  int damn;
  cnt = 0;
  sprintf(fname,"%s.out",inFile);
  out = fopen(fname, "w");

  for (i=0; i<seqTot; i++)
    if (strlen(seqs[i].name) > nameSize)
      nameSize = strlen(seqs[i].name);
  /*
  for (i=0; i<seqTot; i++){
    k = 0;
    j = 0;
    while (j<SEQLEN){
      while (seqs[i].seq[j] == 'N')
	j++;
      seqs[i].seq[k++] = seqs[i].seq[j++];
    } // while j
    seqs[i].seq[k++] = 0;
  } // for cleanup
  */
  maxlen = mystrlen(seqs[0].seq);
  for (i=1; i<seqTot; i++){
    if (mystrlen(seqs[0].seq) > maxlen)
      maxlen = mystrlen(seqs[0].seq);
  }

  for (i=0; i<seqTot; i++){
    if (mystrlen(seqs[i].seq) < maxlen){
      for (j=mystrlen(seqs[i].seq); j<maxlen; j++)
	seqs[i].seq[j] = 'N';
      seqs[i].seq[j] = 'X';
    }
  }
  for (i=0; i<seqTot; i++){
    printf("%s len: %d\n",seqs[i].name,mystrlen(seqs[i].seq));
    //fprintf(out,"%s: %s\n\n\n\n",seqs[i].name,seqs[i].seq);
  }
  //return;
  
  while (cnt < strlen(seqs[0].seq)){
    printf("cnt: %d of %d\n", cnt, strlen(seqs[0].seq));
    for (j=0; j<seqTot; j++){
      fprintf(out,"%s", seqs[j].name);
      for (k=0; k<=(nameSize-strlen(seqs[j].name)); k++)
	fprintf(out, " ");
      fprintf(out, "   ");
      for (i=cnt; i<cnt+60; i++){
	if (i >= strlen(seqs[0].seq))
	  break;
	fprintf(out, "%c", seqs[j].seq[i]);
      } // for i
      fprintf(out, "\n");
    } // for j
    fprintf(out, "\n");
    cnt = i;
    fflush(out);
  } // while
  
  
} // printOut

int mystrlen(char seq[]){
  int i=0;
  for (i=0; i<SEQLEN; i++)
    if (seq[i] == 'X')
      break;
  return i;
}

//char *cleanQuery(char *query)
