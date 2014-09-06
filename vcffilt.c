#include <stdio.h>
#include <string.h>

#define MAXLINE 2000 
#define MAXCALL 30000

#define MAXDELBP 13000000
#define MAXINVCALL 50
#define MAXDELCALL 2400

typedef struct call{
  char line[MAXLINE];
  int sup;
  char type;
  char skip;
  int size;
}_call;

int parsesup(char *);
int parsesize(char *);

struct call allcalls[MAXCALL];

int main(int argc, char **argv){
  char line[MAXLINE];

  int callcnt=0;
  int i;

  int totdel;
  int totinv;
  int totins;
  int delcnt;
  int invcnt;
  int inscnt;

  int converge;
  int delcut=2;
  int invcut=2;
  
  while (1){
    fgets(line, MAXLINE, stdin);
    if (feof(stdin)) break;
    if (line[0] == '#') printf("%s", line);
    else{
      allcalls[callcnt].skip = 0;
      strcpy(allcalls[callcnt].line, line);
      allcalls[callcnt].sup = parsesup(line);
      allcalls[callcnt].size = parsesize(line);
      if (strstr(line, "<DEL>")) 
	allcalls[callcnt].type = 'D';    
      else if (strstr(line, "<INS>")) 
	allcalls[callcnt].type = 'I';
      else if (strstr(line, "<INV>")) 
	allcalls[callcnt].type = 'V';
      callcnt++;
    }
  }

  converge = 0;
  
  while (!converge){
    converge = 1;
    totdel = 0; totinv=0; delcnt=0;
    for (i=0;i<callcnt;i++){
      if (allcalls[i].skip == 0){
	if (allcalls[i].type == 'D') { totdel+=allcalls[i].size; delcnt++; }
	else if (allcalls[i].type=='V') totinv++;
      }
    }
    
    if (totinv > MAXINVCALL) {converge = 0; invcut++;}
    if (totdel > MAXDELBP || delcnt > MAXDELCALL) {converge = 0; delcut++; }
    
    for (i=0;i<callcnt;i++){
      if (allcalls[i].type == 'D' && allcalls[i].sup < delcut) 
	allcalls[i].skip = 1;
      if (allcalls[i].type == 'I' && allcalls[i].sup < delcut) 
	allcalls[i].skip = 1;
      else if (allcalls[i].type == 'V' && allcalls[i].sup < invcut) 
	allcalls[i].skip = 1;
    }

  }


  totdel = 0; totins = 0; delcnt=0; inscnt=0; invcnt=0;
  for (i=0;i<callcnt;i++){
    if (allcalls[i].skip == 0){
      
      if (allcalls[i].type == 'D'){
	delcnt++; totdel += allcalls[i].size;
      }
      
      else if (allcalls[i].type == 'I'){
	inscnt++; totins += allcalls[i].size;
      }
      
      if (allcalls[i].type == 'V'){
	invcnt++; 
      }
      
      printf("%s", allcalls[i].line);
    }
  }


  fprintf(stderr, "Remaining del cut:%d, %d calls = %d bp\nInv cut:%d, %d calls\nIns cut:%d, %d calls = %d bp\n", delcut, delcnt, totdel, invcut, invcnt, delcut, inscnt, totins);
}


int parsesup(char *line){
  
  int state=0;
  int i;
  char buf[20];
  char ch;
  int len = strlen(line);
  
  i = 0;

  while (state!=4 && i<len){
    ch = line[i++];
    switch(state){
    case 0:
      if (ch=='S') state = 1;
      else state = 0;
      break;
    case 1:
      if (ch=='U') state = 2;
      else state = 0;
      break;
    case 2:
      if (ch=='P') state = 3;
      else state = 0;
      break;
    case 3:
      if (ch=='=') state = 4;
      else state = 0;
      break;
    }
  }
  
  strcpy(buf, line+i);
  buf[strlen(buf)-1] = 0;
  
  return atoi(buf);
}



int parsesize(char *line){
  
  int state=0;
  int i, j;
  char buf[20];
  char ch;
  int len = strlen(line);
  
  i = 0;
  
  while (state!=6 && i<len){
    ch = line[i++];
    switch(state){
    case 0:
      if (ch=='S') state = 1;
      else state = 0;
      break;
    case 1:
      if (ch=='V') state = 2;
      else state = 0;
      break;
    case 2:
      if (ch=='L') state = 3;
      else state = 0;
      break;
    case 3:
      if (ch=='E') state = 4;
      else state = 0;
      break;
    case 4:
      if (ch=='N') state = 5;
      else state = 0;
      break;
    case 5:
      if (ch=='=') state = 6;
      else state = 0;
      break;
    }
  }
  
  j = 0;

  while (line[i] != ';')
    buf[j++] = line[i++];
    
  buf[j] = 0;
  
  return atoi(buf);
}

