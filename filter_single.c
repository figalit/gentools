#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


#define _NULL 0
#define _S 1
#define _R 2
#define _G 3
#define EOL -1

/* 
   takes a -final- RNApair product and checks the distance metric
*/

main(int argc, char **argv){
  FILE *fp;
  char line[20];
  int *S;
  int *R;
  int size_s;
  int size_r;
  int index;
  int i,j;
  int is_s_1;
  int is_s_2;
  int is_r_1;
  int is_r_2;
  int epsilon;
  int breakflag;

  char one[2], two[4], three[2], four[4];
  if (argc != 5){
    fprintf(stderr, "Usage: %s <final file> <size_s> <size_r> <epsilon>\n", argv[0]);
    exit(0);
  }
  fp = fopen(argv[1], "r");
  if (fp == NULL){
    fprintf(stderr, "Unable to open file %s\n", argv[1]);
    exit(0);
  }

  size_s = atoi(argv[2]);
  size_r = atoi(argv[3]);
  epsilon = atoi(argv[4]);

  S = (int *)malloc((size_s+1) * sizeof(int));
  R = (int *)malloc((size_r+1) * sizeof(int));
  S[0] = _NULL;
  R[0] = _NULL;

  while(fgets(line, 20, fp) > 0){
    line[strlen(line)-1] = 0;
    if (line[0] == 0)
      continue;
    sscanf(line, "%s%s %s%s",one, two, three, four);
    if (!strcmp(one, "S")){
      if (!strcmp(two, "Gap")){  // S Gap R index
	index = atoi(four);
	R[index] = _G;
      }
      else{
	if(!strcmp(four, "Gap")){ // S index R Gap
	  index = atoi(two);
	  S[index] = _G;	  
	}
	else if (!strcmp(three, "S")){  // S index S index
	  index = atoi(four);
	  S[index] = _S;	
	  index = atoi(two);
	  S[index] = _S;
	}
	else{ // three=R  S index R index
	  index = atoi(four);
	  R[index] = _S;	
	  index = atoi(two);
	  S[index] = _R;
	}
      }
    }
    else { // if first char is R  - R index R index
      // then three should also be R 
      index = atoi(four);
      R[index] = _R;	
      index = atoi(two);
      R[index] = _R;
    }
  }

  i=1;j=1;
  is_s_1 = is_s_2 = is_r_1 = is_r_2 = _NULL;
  
  // sorun burada, is_s_1i once bulacagini zannedio
  while (i<=size_s && j<=size_r){

    breakflag=0;
    
    for(;i<=size_s;i++){
      if (S[i] == _R)
	is_s_1 = i;
      else if (is_s_1 != 0 && S[i] == _S){
	//printf("find is_s_1 %d\n", is_s_1);
	for(;i<=size_s;i++){
	  is_s_2 = EOL;
	  if (S[i] == _R){
	    is_s_2 = i;
	    //  printf("find is_s_2 %d\n", is_s_2);
	    breakflag=1;
	    break;
	  }
	}
      }
      if (breakflag)
	break;
    }
    
    breakflag=0;

    for(;j<=size_r;j++){
      if (R[j] == _S)
	is_r_1 = j;
      else if (is_r_1 != 0 &&  R[j] == _R){
	//	printf("find is_r_1 %d\n", is_r_1);
	for(;j<=size_r;j++){
	  is_r_2 = EOL;
	  if (R[j] == _S){
	    is_r_2 = j;
	    //  printf("find is_r_2 %d\n", is_r_2);
	    breakflag=1;
	    break;
	  }
	}
      }
      if (breakflag)
	break;
    }

    if(is_s_2 == EOL || is_r_2==EOL){
      printf("Nope, one interaction\n");
      exit(0);
    }
    
    if (is_s_2 != EOL && is_r_2 != EOL && is_s_1 != size_s && is_r_1 != size_r){
      //      printf("%d %d %d %d\n", is_s_1,is_s_2,is_r_1,is_r_2);
      if (!(abs((is_s_2-is_s_1)- (is_r_2-is_r_1))<=epsilon)){
	//	printf("Yep [Hai].\n");
	//else{
	printf("Nope.\n");
	return;
      }	
    }
    
  }
  
  printf("Yabadabadu!\n");

}

