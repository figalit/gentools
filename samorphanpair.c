#include <stdio.h>

#define MAX 5000
#define INSERT 400

char **orphans;
int *lengths;

int getlen(char *orphan, int numorp);

int main(int argc, char **argv){
  char name[100];
  int orient;
  char orphan[100];
  int pos;
  char line[1000];
  FILE *in; FILE *len;
  int i;
  char fname[100];
  char lenname[100];
  char thisorphan[100];
  int hasbegin, hasend;
  int hasfwd, hasrev;
  int beginor, endor;
  int numorp;
  int thislen;

  fname[0]=0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      strcpy(lenname, argv[i+1]);
  }

  if (fname[0]==0) return 0;
  in = fopen(fname, "r");
  if (in==NULL) return 0;
		     
  if (lenname[0]==0) return 0;
  len = fopen(lenname, "r");
  if (len==NULL) return 0;


  orphans = (char **) malloc(sizeof (char *) * MAX);
  for (i=0;i<MAX;i++){
    orphans[i] = (char *) malloc(sizeof(char) * 100);
    orphans[i][0]=0;
  }
  lengths = (int *) malloc(sizeof (int) * MAX);

  i=0;

  while (fscanf(len, "%s %d\n", orphan, &thislen) > 0){
    strcpy(orphans[i], orphan);
    //printf("read %s\t%d\n", orphan, i);
    lengths[i] = thislen;
    i++;
  }
  fclose(len);

  numorp = i;
		     
  thisorphan[0]=0; hasbegin=0; hasend=0; hasfwd=0; hasrev=0;
  // assumes input is sorted by orphan name
  while (fscanf(in, "%s\t%d\t%s\t%d", name, &orient, orphan, &pos) > 0){
    fgets(line, 1000, in);
    if (strcmp(orphan, thisorphan)){
      if (hasbegin && hasend && hasfwd && hasrev) {
	printf("%s\n", orphan);
      }
      hasbegin=0; hasend=0; strcpy(thisorphan, orphan); hasfwd=0; hasrev=0;
    }

    else{
      
      thislen = getlen(orphan, numorp);
      if (pos < INSERT)
	hasbegin = 1;
      else if ( pos > (thislen - INSERT)) 
	hasend = 1;
      if (orient==0) hasfwd=1;
      else if (orient==16) hasrev=1;
    }
    
  }
}


int getlen(char *orphan, int numorp){
  int i;
  for (i=0;i<numorp;i++){
    if (!strcmp(orphan, orphans[i]))
      return lengths[i];
  }
}
