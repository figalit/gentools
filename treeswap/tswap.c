#include "stack.h"

char *swap(char *, int, int, int, int);
char next_paran(char *, int);

main(int argc, char **argv){
  char *tree;
  char *stree;
  int cnt=0;
  char ch;
  char namebuf[100];
  char lengthbuf[100];
  FILE *in;
  FILE *out;
  int i, j;
  int swapcnt=0;
  char outfname[100];
  int start1, start2, start3;
  int end1, end2;
  int left1, left2, left3;
  int left[3];
  int start[3];
  int end[3];
  int semicolon=0;
  int colon=0;
  int popcnt = 0;

  if (argc != 2){
    printf("Tree Swapper\nUsage: %s TreeFile\n",argv[0]);
    exit(0);
  } // if argc
  if ((in = fopen(argv[1],"r")) == NULL){
    printf("File %s cannot be opened\n",argv[1]);
    exit(0);
  } // if fopen
  sprintf(outfname,"%s.swapped",argv[1]);
  tree = (char *)malloc(MAX);  stree = (char *)malloc(MAX);
  while( fscanf(in, "%c", &ch) > 0 ){
    if (ch != '\n' && ch != ';')
      tree[cnt++] = ch;
    else if (ch  == ';')
      semicolon=1;
  } // while
  tree[cnt] = 0; // end of string
  if (tree[cnt-1] == ':'){
    colon = 1;
    tree[--cnt]=0; // delete last colon
  }
  tree = (char *)realloc(tree, cnt+2); 
  //printf("%s\n",tree);
  stk = createstack(cnt/2);
  strcpy(stree,tree);
  cnt = strlen(tree);
  i=0;
  while (i<cnt){
    if (tree[i]=='('){
      //printf("push\n");
      push(1, 0, i, 0, stk); // new node
    } // left parantheses
    else if (tree[i]==')'){
      // mark here, check for equality, check previous node
      if (incRight(stk, i)){
	mark(stk);
	//printf("mark\n");
      } // if incright
      if ( next_paran(tree, i) != '(' ) {
	popcnt=0;
	if (isMarked(stk)) {
	  while (isMarked(stk)) 
	    pop(stk, &(left[popcnt]), &(start[popcnt]), &(end[popcnt++]));
	  pop(stk, &(left[popcnt]), &(start[popcnt]), &(end[popcnt++]));
	  if ( popcnt == 2 ){
	    if (left[0] > left[1]){
	      tree = swap(tree, start[0], end[0], start[1], end[1]);
	      swapcnt++;
	      printf("Swapping Ratio: %f\n",((float)left[1]/(float)left[0]));
	    } // if left0>left1
	    incTop(left[0], left[1], stk);
	  } // if popcnt
	  else {
	    left1 = left[1] + left[2];
	    if (left[0] > left[1]) {
	      if (left[1] > left[2]) {
		tree = swap(tree, start[0], end[0], start[2], end[2]);
		swapcnt++;
	      } //if 0>1>2
	      else if (left[0] > left[2]) {
		tree = swap(tree, start[0], end[0], start[1], end[1]);
		swapcnt++;	      
		tree = swap(tree, start[1], (start[1]+end[0]-start[0]), start[1], end[1]);
		swapcnt++;	      
	      } // 0>2>1 
	      else {
		tree = swap(tree, start[0], end[0], start[1], end[1]);
		swapcnt++;	      
	      } // 2>0>1
	    } // if 0>1
	    else {
	      if (left[1] > left[2]){
		if (left[0] > left[2]){
		  tree = swap(tree, start[1], end[1], start[2], end[2]);
		  swapcnt++;	      		  
		  tree = swap(tree, start[0], end[0], start[1], (start[1]+end[2]-start[2]));
		  swapcnt++;	      		  
		} // 1>0>2
		else {
		  tree = swap(tree, start[1], end[1], start[2], end[2]);
		  swapcnt++;	      		  
		} // 1>2>0
	      } // 1>2
	      else{
	        ; // no swap
	      } // 2>1>0
	    } // 1>0 
	    incTop(left[0], left1, stk);
	  } // popcnt == 3
	} // if ismarked
      } // if next_paran
    } // right parantheses
    i++;
  } // while
  //printf("\n\n\n%s\n\n\n%s\n",stree,tree);
  if (!strcmp(stree,tree))
    printf("no change\n");
  out = fopen(outfname, "w");
  for (i=0; i<strlen(tree); i++)
    if (tree[i]=='(' || tree[i]==')' || tree[i]==',')
      fprintf(out,"%c\n",tree[i]);
    else
    fprintf(out,"%c",tree[i]);
  if (colon)  
    fprintf(out,":"); 
  if (semicolon)
    fprintf(out,";"); 
  printf("Total # of Swaps : %d\n",swapcnt);
} // main

char *swap(char *tree, int start1, int end1, int start2, int end2){
  char *ret;
  int i;
  int j;
  //printf("swwap: start1: %d end1: %d start2: %d end2: %d\n",start1,end1,start2,end2);
  ret = (char *)malloc(strlen(tree)+5);
  for (i=0;i<start2;i++){
    ret[i] = tree[i];
    //printf("%c",tree[i]);
  }
  for(j=start1;j<=end1;j++){
    ret[i++] = tree[j];
    //printf("%c",tree[i]);
  }
  while (tree[j] != '(' && tree[j] !=')' && j<strlen(tree)){
    ret[i++]  = tree[j++];
    //printf("%c",tree[j-1]);
  }
  if (ret[i-1] != ',')
    ret[i++] = ',';
  for(j=start2;j<=end2;j++){
    ret[i++] = tree[j];
    //printf("%c",tree[j]);
  }
  while (tree[j] != '(' && tree[j] !=')' && j<strlen(tree)){
    ret[i++]  = tree[j++];
    //printf("%c",tree[j-1]);
  }
  i--;
  //printf("here i is: %d\n",i);
  while (i<strlen(tree)){
    ret[i] = tree[i++];
    //printf("%c",tree[i-1]);
  }
  ret[i]=0;
  printf("\n");
  if (ret[strlen(ret)-1] == ',' || ret[strlen(ret)-1]==':')
    ret[strlen(ret)-1] = 0;
  if (ret[strlen(ret)-1] == ',' || ret[strlen(ret)-1]==':')
    ret[strlen(ret)-1] = 0;
  return ret;
} // swap

char next_paran(char *tree, int i){
  int j=i+1;
  while (tree[j] != '(' && tree[j] != ')' && j<strlen(tree))
    j++;
  return tree[j];
} // next_paran
