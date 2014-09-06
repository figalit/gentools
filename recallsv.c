#include <stdio.h>
#include <ctype.h>

#define FASTQ_OFFSET 33


int main(int argc, char **argv){
  int i;
  char fname[500];
  char *ref;
  FILE *reffile;
  int reflen;
  char seqname[40];
  char dummy[500];
  char ch;


  int start, end, mm;
  char chr[30];
  char orient[2];
  char name[100];
  char seqqual[40];
  char type1, type2;
  char snp1[5], snp2[5];
  int loc1, loc2;
  char q1[5], q2[5];
  int offset;
  int pass;
  int doperfect=0;
  int total;
  int wasperfect;
  int  QUAL_CUTOFF=25;
  int nptotal=0;
  int linecnt=0;
  int minsnp=1;
  char fwdname[100];
  char revname[100];
  int rstart, rend;
  char rorient[2];
  char rchr[30];
  
  int rmm;
  char rseqqual[40];
  char rtype1, rtype2;
  char rsnp1[5], rsnp2[5];
  int rloc1, rloc2;
  char rq1[5], rq2[5];
  char span[30];
  int tmm;
  char call[100];
  char tchr[30];
  char tstart[30], tend[30];

  seqname[0] = 0;
  fname[0] = 0;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
  }

  //reffile = fopen(fname, "r");

  while (scanf("%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%d\t%s\t%s\t%s\t%s", name, fwdname, chr, &start, &end, &mm, orient, revname, rchr, &rstart, &rend, &rmm, rorient, span, &tmm, call, tchr, tstart, tend) > 0){
    //printf("%s\t%s\n", name, chr);
    linecnt++;
    
    //printf("%s\t%s\n", name, call);
    

    if (!strcmp(call, "transchromosomal")){
      //printf("got trans\n");
      scanf("\n");
    }

    else{

    if (mm>=1){
      scanf("\t%c\t%d\t%s\t", &type1, &loc1, snp1);
      //if (type1 != 'D')
      scanf("%s\t", q1);  
    }
    if (mm==2){
      //if (type1=='I' && strlen(snp1)!=1)
      //	;
      //else{
      scanf("%c\t%d\t%s\t", &type2, &loc2, snp2);
      //if (type2 != 'D')
      scanf("%s\t", q2);  
	//}
    }
    
    scanf("%s", seqqual);

    if (rmm>=1){
      scanf("\t%c\t%d\t%s\t", &rtype1, &rloc1, rsnp1);
      //if (type1 != 'D')
      scanf("%s\t", rq1);  
    }
    if (rmm==2){
      //if (rtype1=='I' && strlen(rsnp1)!=1)
      //;
      // else{
      scanf("%c\t%d\t%s\t", &rtype2, &rloc2, rsnp2);
      //if (type2 != 'D')
      scanf("%s\t", rq2);  
	//}
    }
    
    scanf("%s\n", rseqqual);

    }
    
    //chr13:18,428,014-18,442,236
    if (!strcmp(chr, "chr13") && start>=18428014 && end <= 18442236){

      if (!(strcmp(rchr, "chr13") && rstart>=18428014 && rend <= 18442236)){
	//printf("%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%d\t%d", name, fwdname, "GGO_BLK", start, end, mm, orient, revname, rchr, rstart, rend, rmm, rorient, span, tmm, tchr, tstart, tend);
	printf("%s\t%s\t%d\t%d\t%d\t%s\n", revname, rchr, rstart, rend, rmm, rorient);
      }
    }

    else if (!(!strcmp(chr, "chr13") && start>=18428014 && end <= 18442236)){
      
      if (strcmp(rchr, "chr13") && rstart>=18428014 && rend <= 18442236){
	//printf("%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%d\t%d", name, fwdname, "GGO_BLK", start, end, mm, orient, revname, rchr, rstart, rend, rmm, rorient, span, tmm, tchr, tstart, tend);
	printf("%s\t%s\t%d\t%d\t%d\t%s\n", fwdname, chr, start, end, mm, orient);
      }
    }

    /*
    //chr10:19433918-19581617
    if (!strcmp(chr, "chr10") && start>=19433918 && end <= 19581617){

      if (!(strcmp(rchr, "chr10") && rstart>=19433918 && rend <= 19581617)){
	//printf("%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%d\t%d", name, fwdname, "GGO_BLK", start, end, mm, orient, revname, rchr, rstart, rend, rmm, rorient, span, tmm, tchr, tstart, tend);
	printf("%s\t%s\t%d\t%d\t%d\t%s\n", revname, rchr, rstart, rend, rmm, rorient);
      }
    }

    else if (!(!strcmp(chr, "chr10") && start>=19433918 && end <= 19581617)){
      
      if (strcmp(rchr, "chr10") && rstart>=19433918 && rend <= 19581617){
	//printf("%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%d\t%d", name, fwdname, "GGO_BLK", start, end, mm, orient, revname, rchr, rstart, rend, rmm, rorient, span, tmm, tchr, tstart, tend);
	printf("%s\t%s\t%d\t%d\t%d\t%s\n", fwdname, chr, start, end, mm, orient);
      }
      }*/

  }
    

  
}
