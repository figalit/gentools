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
  char fname[50];
  char s_size[10];
  char r_size[10];
  char eps[10];
  int *S;
  int *R;
  int *Sl;
  int *Rl;
  int size_s;
  int size_r;
  int index;
  int i,j, ii,jj;
  int is_s_1;
  int is_s_2;
  int is_r_1;
  int is_r_2;
  int epsilon;
  int breakflag;
  int decision;
  int hairpin_s;
  int hairpin_r;
  int hairflag_s;
  int hairflag_r;
  int kisscnt;
  int isEight_s;
  int isEight_r;
  int preIs;
  int preIr;

  char one[2], two[5], three[2], four[5];

  while(scanf("%s%s%s%s", fname, s_size, r_size, eps) > 0){

  kisscnt = 0;
  isEight_s = 0;
  isEight_r = 0;
  preIs = 0;    
  preIr = 0;    
  fp = fopen(fname, "r");

  size_s = atoi(s_size);
  size_r = atoi(r_size);
  epsilon = atoi(eps);

  S = (int *)malloc((size_s+1) * sizeof(int));
  R = (int *)malloc((size_r+1) * sizeof(int));
  S[0] = _NULL;
  R[0] = _NULL;

  Sl = (int *)malloc((size_s+1) * sizeof(int));
  Rl = (int *)malloc((size_r+1) * sizeof(int));
  Sl[0] = _NULL;
  Rl[0] = _NULL;

  while(fgets(line, 20, fp) > 0){
    line[strlen(line)-1] = 0;
    if (line[0] == 0)
      continue;
    sscanf(line, "%s%s%s%s",one, two, three, four);
    if (!strcmp(one, "S")){
      if (!strcmp(two, "Gap")){  // S Gap R index
	index = atoi(four);
	R[index] = _G;
	Rl[index] = _NULL;
      }
      else{
	if(!strcmp(four, "Gap")){ // S index R Gap
	  index = atoi(two);
	  S[index] = _G;	
	  Sl[index] = _NULL;  
	}
	else if (!strcmp(three, "S")){  // S index S index
	  index = atoi(four);
	  S[index] = _S;
	  Sl[index] = atoi(two);
	  index = atoi(two);
	  S[index] = _S;
	  Sl[index] = atoi(four);
	}
	else{ // three=R  S index R index
	  index = atoi(four);
	  R[index] = _S;	
	  Rl[index] =  atoi(two);
	  index = atoi(two);
	  S[index] = _R;
	  Sl[index] =  atoi(four);
	}
      }
    }
    else { // if first char is R  - R index R index
      // then three should also be R 
      index = atoi(four);
      R[index] = _R;	
      Rl[index] =  atoi(two);
      index = atoi(two);
      R[index] = _R;
      Rl[index] =  atoi(four);
    }
  }
  fclose(fp);


  breakflag=0;
  for(i=1;i<=size_s;i++)
    if (S[i] == _R)
      breakflag=1;
  if (!breakflag){
    printf("Nope: %s\n", fname);
    return;
  }


  hairpin_s=0;
  hairpin_r=0;
  hairflag_s=0;
  hairflag_r=0;

  i=1;j=1;
  is_s_1 = is_s_2 = is_r_1 = is_r_2 = _NULL;
  decision = 1;

  while (i<=size_s && j<=size_r){

    breakflag=0;
    
    for(;i<=size_s;i++){
      if (S[i] == _R){
	is_s_1 = i;
	if (preIs == 0)
	  preIs = is_s_1;
      }
      else if (is_s_1 != 0 && S[i] == _S){
	printf("find is_s_1 %d preIs %d\n", is_s_1, preIs);
	if (is_s_1-preIs >= 8)
	  isEight_s=1;
	for(;i<=size_s;i++){
	  is_s_2 = EOL;	  
	  
	  if (S[i] == _R){
	    is_s_2 = i;
	    ii=is_s_1;
	    //for(ii=is_s_1;ii<is_s_2;ii++){
	    while(ii<is_s_2){
	      if (S[ii] == _S){ //check for hairpins
		if (hairflag_s == 0 && Sl[ii]>ii && Sl[ii]<is_s_2){
		  hairpin_s+=Sl[ii]-ii+1;
		  printf("hairpin_s btw %d %d | %d %d %d\n",is_s_1,is_s_2,Sl[ii],ii,hairpin_s);
		  hairflag_s=1;
		  ii=Sl[ii];
		}
		else if(ii>Sl[ii])
		  hairflag_s=0;
	      }
	      ii++;
	    }
	    kisscnt++;
	    printf("find is_s_2 %d\n", is_s_2);
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
      if (R[j] == _S){
	is_r_1 = j;
	if (preIr == 0)
	  preIr = is_r_1;
      }
      else if (is_r_1 != 0 &&  R[j] == _R){
	printf("find is_r_1 %d\n", is_r_1);
	if (is_r_1-preIr >= 8)
	  isEight_r=1;
	for(;j<=size_r;j++){
	  is_r_2 = EOL;
	  
	  if (R[j] == _S){
	    is_r_2 = j;
	    jj=is_r_1;
	    //for(jj=is_r_1;jj<is_r_2;jj++){
	    while(jj<is_r_2){
	      if (R[jj] == _R){ //check for hairpins
		if (hairflag_r == 0 && Rl[jj]>jj && Rl[jj]<is_r_2){
		  hairpin_r+=Rl[jj]-jj+1;
		  printf("hairpin_r btw %d %d | %d %d %d.\n",is_r_1,is_r_2,Rl[jj],jj,hairpin_r);
		  hairflag_r=1;
		  jj=Rl[jj];
		}
		else if(jj>Rl[jj])
		  hairflag_r=0;
	      }
	      jj++;
	    }
	    printf("find is_r_2 %d\n", is_r_2);
	    breakflag=1;
	    break;
	  }
	}
      }
      if (breakflag)
	break;
    }

    if (is_s_2 != EOL && is_r_2 != EOL && is_s_1 != size_s && is_r_1 != size_r){
      //      printf("%d %d %d %d\n", is_s_1,is_s_2,is_r_1,is_r_2);
      if (!(abs((is_s_2-is_s_1-hairpin_s)- (is_r_2-is_r_1-hairpin_r))<=epsilon)){
	//	printf("Yep [Hai].\n");
	//else{
	printf("Nope: %s %d %d %d %d, DISTANCE:%d \n", fname,is_s_1,is_s_2,is_r_1,is_r_2,(abs((is_s_2-is_s_1-hairpin_s) - (is_r_2-is_r_1-hairpin_r))));
	decision=0;
	break;
	//	return;
      }	
    }
       
  }
  
  if (isEight_s==0 || isEight_r==0)
    printf("[NOEIGHT] %s\n", fname);
  else if (kisscnt==0)
    printf("[SINGLE] %s\n", fname);
  else if (decision)
    printf("Yep: %s\n", fname);
    //    printf("Yep: %s %d %d %d %d\n", fname,is_s_1,is_s_2,is_r_1,is_r_2);
  free(S);
  free(R);
  free(Sl);
  free(Rl);
  }
}

