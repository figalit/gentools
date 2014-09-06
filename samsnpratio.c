#include <stdio.h>
#include <ctype.h>

#define FASTQ_OFFSET 33
#define MAXSNP 2

int  mytok(char *str, char delim, char *save, int start);

int main(int argc, char **argv){
  int i, k;
  char fname[500];
  char *ref;
  FILE *reffile;
  int reflen;
  char seqname[40];
  char dummy[500];
  char ch;

  int *acnt, *ccnt, *gcnt, *tcnt;

  int start, end, mm;
  char chr[30];
  //char orient[2];
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

  int orient;
  int mapqual, mpos, isize;
  char cigar[100], mrnm[100], sequence[100], quality[100], dist[100], md[100];
  char token[30];
  int tokenpos;
  
  int snppos[MAXSNP];
  char buf[10];
  int bufk;

  seqname[0] = 0;
  fname[0] = 0;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-f"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      reflen = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-p"))
      doperfect = 1;
    else if (!strcmp(argv[i], "-npt"))
      nptotal = 1;
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
  
  acnt = (int *) malloc(sizeof(int)*reflen);
  ccnt = (int *) malloc(sizeof(int)*reflen);
  gcnt = (int *) malloc(sizeof(int)*reflen);
  tcnt = (int *) malloc(sizeof(int)*reflen);

  memset(acnt, 0, reflen*sizeof(int));
  memset(ccnt, 0, reflen*sizeof(int));
  memset(gcnt, 0, reflen*sizeof(int));
  memset(tcnt, 0, reflen*sizeof(int));

  i=0;

  while(fscanf(reffile, "%c", &ch) > 0){
    if (isalpha(ch))
      ref[i++]=toupper(ch);
  }

  ref[i]=0;
  fclose(reffile);

  fprintf(stderr, "%s loaded.\n", fname);

  //scanf("%s", dummy);
  //  printf("dum:%s\n", dummy);


  while (scanf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n", name, &orient, chr, &start, &mapqual, cigar, mrnm, &mpos, &isize, sequence, quality, dist, md) > 0){




    //printf("%s\t%s\n", name, chr);
    linecnt++;
    if (start > reflen)
      continue;

    if (strcmp(chr, seqname))
      continue;

    tokenpos = mytok(dist, ':', token, 0);    
    tokenpos = mytok(dist, ':', token, tokenpos);    
    tokenpos = mytok(dist, ':', token, tokenpos);    

    printf("dist: %s", token);
    mm = atoi(token);
    
    tokenpos = mytok(md, ':', token, 0);    
    tokenpos = mytok(md, ':', token, tokenpos);    
    tokenpos = mytok(md, ':', token, tokenpos);    

    

    printf("\tedit: %s\n", token);
    //    printf("dist: %d\tedit: %s\n", mm, token);

    //continue;


    if (mm == 0){

      for (i=0;i<strlen(sequence);i++){
	if (ref[i+start-1] == 'A')
	  acnt[i+start - 1]++;
	else if (ref[i+start-1] == 'C')
	  ccnt[i+start - 1]++;
	else if (ref[i+start-1] == 'G')
	  gcnt[i+start - 1]++;
	else if (ref[i+start-1] == 'T')
	  tcnt[i+start - 1]++;
      }

      continue;

    }



    i=0;
    k=0;
    while (isalpha(token[i])) {
      snppos[k++]=i;
      printf("\t%d:%d", k, snppos[k]);
      i++;
    }
    
    while (k<mm){
      
      snppos[k]=0;
      buf[0]=0;
      bufk=0;

      while(i<strlen(token)){
	if (isalpha(token[i]))
	  break;
	else
	  buf[bufk++]=token[i];
	i++;
      }

      buf[bufk]=0;

      snppos[k]=atoi(buf);

      printf("\t%d:%d", k, snppos[k]);
      k++;
    }

    printf("\n");

    continue;


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
	  if (type1 == 'S' && q1[0]-FASTQ_OFFSET >= QUAL_CUTOFF){
	    if (mm==2 && type2=='I' && loc2<loc1)
	      offset = -1;
	    if (mm==2 && type2=='D' && loc2<loc1)
	      offset = 1;	  
	    if (snp1[2] == 'A')
	      acnt[loc1 - 1 + offset]++;
	    else if (snp1[2] == 'C')
	      ccnt[loc1 - 1 + offset]++;
	    else if (snp1[2] == 'G')
	      gcnt[loc1 - 1 + offset]++;
	    else if (snp1[2] == 'T')
	      tcnt[loc1 - 1 + offset]++;
	  }
	  pass = 1;
	}
	if (mm==2 && i+start == loc2){
	  if (type2 == 'S' && q2[0]-FASTQ_OFFSET >= QUAL_CUTOFF){
	    if (type1 == 'I' && loc1<loc2)
	      offset = -1;
	    if (type1=='D' && loc1<loc2)
	      offset = 1;	  
	    if (snp2[2] == 'A')
	      acnt[loc2 - 1 + offset]++;
	    else if (snp2[2] == 'C')
	      ccnt[loc2 - 1 + offset]++;
	    else if (snp2[2] == 'G')
	      gcnt[loc2 - 1 + offset]++;
	    else if (snp2[2] == 'T')
	      tcnt[loc2 - 1 + offset]++;
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
	    acnt[i+start - 1 + offset]++;
	  else if (ref[i+start-1+offset] == 'C')
	    ccnt[i+start - 1 + offset]++;
	  else if (ref[i+start-1+offset] == 'G')
	    gcnt[i+start - 1 + offset]++;
	  else if (ref[i+start-1+offset] == 'T')
	    tcnt[i+start - 1 + offset]++;
	}

      }

    
  }


  for (i=0; i < reflen; i++){
    if (ref[i]=='N')
      continue;

    wasperfect = 1;

    total = acnt[i] + ccnt[i] + gcnt[i] + tcnt[i];

    if (nptotal){
      switch(ref[i]){
      case 'A':
	total-=acnt[i];
	break;
      case 'C':
	total-=ccnt[i];
	break;
      case 'G':
	total-=gcnt[i];
	break;
      case 'T':
	total-=tcnt[i];
	break;
      }
      
    }
    
    if  ((doperfect && ((acnt[i]!= 0 && ref[i] =='A') || (ccnt[i]!= 0 && ref[i] =='C') || (gcnt[i]!= 0 && ref[i] =='G') || (tcnt[i]!= 0 && ref[i] =='T')) ) || ((acnt[i]!= 0 && ref[i] !='A') || (ccnt[i]!= 0 && ref[i] !='C') || (gcnt[i]!= 0 && ref[i] !='G') || (tcnt[i]!= 0 && ref[i] !='T')))
	printf("%s\t%d\t%d\t%c", seqname, (i+1), total, ref[i]);

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
    
    
    printf("\t%d\t%d\t%d\t%d", acnt[i], ccnt[i], gcnt[i], tcnt[i]);

    if (doperfect && wasperfect)
      printf("\tP");
    
    if (!wasperfect) // ((acnt[i]!= 0 && ref[i] !='A') || (ccnt[i]!= 0 && ref[i] !='C') || (gcnt[i]!= 0 && ref[i] !='G') || (tcnt[i]!= 0 && ref[i] !='T'))
      printf("\tS");

    printf("\n");

  }

}


int  mytok(char *str, char delim, char *save, int start){
  int i,j;
  int len = strlen(str);
  save[0]=0; j=0;
  for (i=start; i<len; i++){
    if (delim == str[i]){
      save[j]=0; 
      //printf("tok2: %s\n", save); 
      break;
    }
    else
      save[j++] = str[i];
  }
  //  printf("tok: %s\ti:%d\tlen:%d\n", save, i, len);
  save[j]=0;
  return i+1;
}
