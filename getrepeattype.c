#include <stdio.h>

/*
   SW  perc perc perc  query      position in query           matching       repeat              position in  repeat
score  div. del. ins.  sequence    begin     end    (left)    repeat         class/family         begin  end (left)   ID
1010   7.4  0.0  0.0  chr17     34141579 34141699 (44633043) C  5S             rRNA                   (0)  121      1    244
*/



main(){

  float score;
  float div;
  float del;
  float ins;
  char qseq[50];
  int begin, end;
  char left[100];
  char orient[10];
  char repeattype2[100];
  char repeattype[100];
  char line[10000];
  char ptype[100];

  float tscore;
  float tdiv;
  float tdel;
  float tins;
  long tlen;
  int repcount;

  int flag=1;

  repeattype[0]=0; ptype[0];
  repcount = 0;
  tlen=0;
  tdel=0; tins=0;
  
  div=0; del=0; ins=0; 
  
  printf("Repeat_type\tAvg_div\tAvg_del\tAvg_ins\total_bp\tcount\n");

  while (scanf("%f %f %f %f %s %d %d %s %s %s %s", &score, &div, &del, &ins, qseq, &begin, &end, left, orient, repeattype2, repeattype) > 0) {
    //fprintf(stderr, "this type %s\n", repeattype);
    fgets(line, 10000, stdin);
    if (strcmp(repeattype, ptype)){
      if (ptype[0]!=0){
	fprintf(stderr, "new type %s\n", repeattype);
	printf("%s\t%f\t%f\t%f\t%d\t%d\n", ptype, (tdiv/repcount), (tdel/repcount), (tins/repcount), tlen, repcount);
	repcount = 1;
	tlen=end-begin+1;
	tdel=del; tins=ins; tdiv = div;
	strcpy(ptype, repeattype); 
      }
      if (ptype[0]==0){
	fprintf(stderr, "new type %s\n", repeattype);
	tlen=end-begin+1;
	tdiv=div; tins=ins; repcount=1; tdel=del;
      }
      strcpy(ptype, repeattype); 
      //flag=0;
    }
    else{
      //flag=1;
      tlen+=end-begin+1;
      tdiv+=div; tins+=ins; repcount++; tdel+=del;
    }
  }

  if (ptype[0]!=0){
	printf("%s\t%f\t%f\t%f\t%d\t%d\n", ptype, (tdiv/repcount), (tdel/repcount), (tins/repcount), tlen, repcount);
	repcount = 0;
	tlen=0;
	tdel=0; tins=0;
  }


}
