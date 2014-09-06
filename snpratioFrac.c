#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define FASTQ_OFFSET 33

typedef struct SNP{
char SNPnew;
char snpOld;
int position;
}SNP;


int main(int argc, char **argv){
  int i;
  char fname[500];
  char *ref;
  FILE *reffile;
  int reflen;
  char seqname[40];
  char dummy[500];
  char ch;

  float *acnt, *ccnt, *gcnt, *tcnt;

  int start, end, mm;
  char chr[30];
  char orient[2];
  char name[100];
  char nameTemp[100]; // for checking if the name of the read changed or not
  char seqqual[40];
  char type1, type2;
  char snp1[5], snp2[5];
  int loc1, loc2;
  char q1, q2;
  int offset;
  int pass;
  int doperfect=0;
  float total;
  int wasperfect;
  int  QUAL_CUTOFF=25;
  
  SNP SNPrecord[1000000];
  int SNPrecordCount=0;  


  seqname[0] = 0;
  fname[0] = 0;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-f"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      reflen = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-p"))
      doperfect = 1;
    else if (!strcmp(argv[i], "-q"))
      QUAL_CUTOFF = atoi(argv[i+1]);
    /*
    else if (!strcmp(argv[i], "-s"))    
      strcpy(seqname, argv[i+1]);
    */
  }

  reffile = fopen(fname, "r");

  fscanf(reffile, "%s", dummy);
  strcpy(seqname, dummy+1);
  fgets(dummy, 100, reffile);
  ref = (char *) malloc(sizeof(char)*(reflen+1));
  
  acnt = (float *) malloc(sizeof(float)*reflen);
  ccnt = (float *) malloc(sizeof(float)*reflen);
  gcnt = (float *) malloc(sizeof(float)*reflen);
  tcnt = (float *) malloc(sizeof(float)*reflen);

  memset(acnt, 0, reflen*sizeof(float));
  memset(ccnt, 0, reflen*sizeof(float));
  memset(gcnt, 0, reflen*sizeof(float));
  memset(tcnt, 0, reflen*sizeof(float));

  i=0;

  while(fscanf(reffile, "%c", &ch) > 0){
    if (isalpha(ch))
      ref[i++]=toupper(ch);
  }

  ref[i]=0;
  fclose(reffile);
  strcpy(nameTemp,"NOOOO");

  //scanf("%s", dummy);
  //  printf("dum:%s\n", dummy);

  while (scanf("%s\t%d\t%d\t%d\t%s\t%s", name, &start, &end, &mm, orient, chr) > 0){
    //printf("%s\t%s\n", name, chr);
    if (start > reflen  || end > reflen)
      continue;

    int count; 
   if (strcmp(name, nameTemp)!=0)
	{
	  for (count=0; count<SNPrecordCount; count++)
	    {
	      if (SNPrecord[count].SNPnew=='A')
		{
		  acnt[SNPrecord[count].position]=acnt[SNPrecord[count].position]+(float)1/(float)SNPrecordCount;
		}
	      if (SNPrecord[count].SNPnew=='C')
		{
		  ccnt[SNPrecord[count].position]=ccnt[SNPrecord[count].position]+(float)1/(float)SNPrecordCount;
		}
	      if (SNPrecord[count].SNPnew=='G')
		{
		  gcnt[SNPrecord[count].position]=gcnt[SNPrecord[count].position]+(float)1/(float)SNPrecordCount;
		}
	      if (SNPrecord[count].SNPnew=='T')
		{
		  tcnt[SNPrecord[count].position]=tcnt[SNPrecord[count].position]+(float)1/(float)SNPrecordCount;
		}
	    }
	SNPrecordCount=0;
	}

    if (strcmp(chr, seqname))
      fgets(dummy, 500, stdin);
    else{
      if (mm>=1){
	scanf("\t%c\t%d\t%s\t", &type1, &loc1, &snp1);
	if (type1 != 'D')
	  scanf("%c\t", &q1);  
      }
      if (mm==2){
	scanf("%c\t%d\t%s\t", &type2, &loc2, &snp2);
	if (type2 != 'D')
	  scanf("%c\t", &q2);  
      }
      scanf("%s\n", seqqual);

      //printf("%s\t%s\n", name, seqqual);


      offset = 0;

      /*
      if (mm==2 && (type1=='S' && type2=='I' && loc2<loc1))
	offset = -1;
      else if (mm==2 && (type1=='S' && type2=='D' && loc2<loc1))
	offset = 1;	  
      else if (mm==2 && (type2=='S' && type1=='I' && loc1<loc2))
	offset = -1;
      else if (mm==2 && (type2=='S' && type1=='D' && loc1<loc2))
	offset = 1;	  
      */
      
      for (i=0;i<strlen(seqqual);i++){
	pass = 0;
	/*
	if (loc1 == 14442608 || loc2 == 14442608){
	  fprintf(stderr, "%s\t
	  }*/
	if (mm>=1 && i+start == loc1){
	  if (type1 == 'S' && q1-FASTQ_OFFSET >= QUAL_CUTOFF){
	    if (mm==2 && type2=='I' && loc2<loc1)
	      offset = -1;
	    if (mm==2 && type2=='D' && loc2<loc1)
	      offset = 1;	  
	    if (snp1[2] == 'A')
	      //acnt[loc1 - 1 + offset]++;
	      {
		SNPrecord[SNPrecordCount].SNPnew='A';
		SNPrecord[SNPrecordCount].position=loc1-1+offset;
		SNPrecordCount++;
	      }
	    else if (snp1[2] == 'C')
	      {
		SNPrecord[SNPrecordCount].SNPnew='C';
		SNPrecord[SNPrecordCount].position=loc1-1+offset;
		SNPrecordCount++;

		//ccnt[loc1 - 1 + offset]++;
	      }
	    else if (snp1[2] == 'G')
	      {
		SNPrecord[SNPrecordCount].SNPnew='G';
		SNPrecord[SNPrecordCount].position=loc1-1+offset;
		SNPrecordCount++;

	      }
	      //	      gcnt[loc1 - 1 + offset]++;
	    else if (snp1[2] == 'T')
	      {
		SNPrecord[SNPrecordCount].SNPnew='T';
		SNPrecord[SNPrecordCount].position=loc1-1+offset;
		SNPrecordCount++;
	      //	      tcnt[loc1 - 1 + offset]++;
	      }
	  }
	  pass = 1;
	}
	if (mm==2 && i+start == loc2){
	  if (type2 == 'S' && q2-FASTQ_OFFSET >= QUAL_CUTOFF){
	    if (type1 == 'I' && loc1<loc2)
	      offset = -1;
	    if (type1=='D' && loc1<loc2)
	      offset = 1;	  
	    if (snp2[2] == 'A')
	      {
		SNPrecord[SNPrecordCount].SNPnew='A';
		SNPrecord[SNPrecordCount].position=loc2-1+offset;
		SNPrecordCount++;

	      }
	      //acnt[loc2 - 1 + offset]++;
	    else if (snp2[2] == 'C')
	      {
		SNPrecord[SNPrecordCount].SNPnew='C';
		SNPrecord[SNPrecordCount].position=loc2-1+offset;
		SNPrecordCount++;

		//ccnt[loc2 - 1 + offset]++;
	      }
	    else if (snp2[2] == 'G')
	      {
		SNPrecord[SNPrecordCount].SNPnew='G';
		SNPrecord[SNPrecordCount].position=loc2-1+offset;
		SNPrecordCount++;

		//gcnt[loc2 - 1 + offset]++;
	      }
	    else if (snp2[2] == 'T')
	      {
		SNPrecord[SNPrecordCount].SNPnew='T';
		SNPrecord[SNPrecordCount].position=loc2-1+offset;
		SNPrecordCount++;

		//tcnt[loc2 - 1 + offset]++;
	      }
	  }
	  pass = 1;
	}
	
	if (!pass && seqqual[i]-FASTQ_OFFSET >= QUAL_CUTOFF){
	  if (mm==2 && (type1=='S' && type2=='I' && loc2<i+start))
	    offset = -1;
	  else if (mm==2 && (type1=='S' && type2=='D' && loc2<i+start))
	    offset = 1;	  
	  else if (mm==2 && (type2=='S' && type1=='I' && loc1<i+start))
	    offset = -1;
	  else if (mm==2 && (type2=='S' && type1=='D' && loc1<i+start))
	    offset = 1;	  
	  else if (mm==1 && (type1=='I' && loc1<i+start))
	    offset = -1;
	  else if (mm==1 && (type1=='D' && loc1<i+start))
	    offset = 1;	  
	  else
	    offset = 0;

	  if (ref[i+start-1+offset] == 'A')
	    {
	      SNPrecord[SNPrecordCount].SNPnew='A';
	      SNPrecord[SNPrecordCount].position=i+start-1+offset;
	      SNPrecordCount++;

	      //	    acnt[i+start - 1 + offset]++;
	    }
	  else if (ref[i+start-1+offset] == 'C')
	    {
	      SNPrecord[SNPrecordCount].SNPnew='C';
	      SNPrecord[SNPrecordCount].position=i+start-1+offset;
	      SNPrecordCount++;

	      // ccnt[i+start - 1 + offset]++;
	    }
	  else if (ref[i+start-1+offset] == 'G')
	    {
	      SNPrecord[SNPrecordCount].SNPnew='G';
	      SNPrecord[SNPrecordCount].position=i+start-1+offset;
	      SNPrecordCount++;

	    //gcnt[i+start - 1 + offset]++;
	    }
	  else if (ref[i+start-1+offset] == 'T')
	    {
	      SNPrecord[SNPrecordCount].SNPnew='T';
	      SNPrecord[SNPrecordCount].position=i+start-1+offset;
	      SNPrecordCount++;

	      // tcnt[i+start - 1 + offset]++;
	    }
	}

      }

    }
  }


  for (i=0; i < reflen; i++){
    if (ref[i]=='N')
      continue;

    wasperfect = 1;

    total = acnt[i] + ccnt[i] + gcnt[i] + tcnt[i];
    
    if  ((doperfect && ((acnt[i]!= 0 && ref[i] =='A') || (ccnt[i]!= 0 && ref[i] =='C') || (gcnt[i]!= 0 && ref[i] =='G') || (tcnt[i]!= 0 && ref[i] =='T')) ) || ((acnt[i]!= 0 && ref[i] !='A') || (ccnt[i]!= 0 && ref[i] !='C') || (gcnt[i]!= 0 && ref[i] !='G') || (tcnt[i]!= 0 && ref[i] !='T')))
	printf("%s\t%d\t%f\t%c", seqname, (i+1), total, ref[i]);

    else
      continue;

  


    if (acnt[i]!= 0 && ref[i] !='A'){
      //printf("%s\t%d\t%c/%c\t%d\t%d\n", seqname, (i+1), ref[i], 'A', acnt[i], (ccnt[i]+gcnt[i]+tcnt[i]));
      //      printf("%s\t%d\t%c/%c\t%d\t%d\t%d\t%d\n", seqname, (i+1), ref[i], 'A', acnt[i], ccnt[i], gcnt[i], tcnt[i]);
      printf("/A");
      wasperfect = 0;
    }
    if (ccnt[i]!= 0 && ref[i] !='C'){
      printf("/C");
    //printf("%s\t%d\t%c/%c\t%d\t%d\t%d\t%d\n", seqname, (i+1), ref[i], 'C', acnt[i], ccnt[i], gcnt[i], tcnt[i]);
    //printf("%s\t%d\t%c/%c\t%d\t%d\n", seqname, (i+1), ref[i], 'C', ccnt[i], (acnt[i]+gcnt[i]+tcnt[i]));
      wasperfect = 0;
    }
    if (gcnt[i]!= 0 && ref[i] !='G'){
      printf("/G");
      //      printf("%s\t%d\t%c/%c\t%d\t%d\t%d\t%d\n", seqname, (i+1), ref[i], 'G', acnt[i], ccnt[i], gcnt[i], tcnt[i]);
      //printf("%s\t%d\t%c/%c\t%d\t%d\n", seqname, (i+1), ref[i], 'G', gcnt[i], (acnt[i]+ccnt[i]+tcnt[i]));
      wasperfect = 0;
    }
    if (tcnt[i]!= 0 && ref[i] !='T'){
      printf("/T");
      wasperfect = 0;
    }
    //      printf("%s\t%d\t%c/%c\t%d\t%d\t%d\t%d\n", seqname, (i+1), ref[i], 'T', acnt[i], ccnt[i], gcnt[i], tcnt[i]);
    //printf("%s\t%d\t%c/%c\t%d\t%d\n", seqname, (i+1), ref[i], 'T', tcnt[i], (acnt[i]+ccnt[i]+gcnt[i]));
    
    
    printf("\t%f\t%f\t%f\t%f", acnt[i], ccnt[i], gcnt[i], tcnt[i]);

    if (doperfect && wasperfect)
      printf("\tP");
    
    if (!wasperfect) // ((acnt[i]!= 0 && ref[i] !='A') || (ccnt[i]!= 0 && ref[i] !='C') || (gcnt[i]!= 0 && ref[i] !='G') || (tcnt[i]!= 0 && ref[i] !='T'))
      printf("\tS");

    printf("\n");

  }

}
