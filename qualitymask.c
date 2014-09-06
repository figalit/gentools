

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SEQ_LENGTH 500
#define MASK_LOW 0
#define MASK_N 1
#define MASK_X 2
#define SEARCH 1
#define INSERT 0

char **names;
char **seqs;
int *length;
int **quality;

int readSingleFasta(FILE *, FILE *, char **);
int find_name(char *, int, int);
int hash(char *, int, int);
void my_fgets(char *, int, FILE *);

int main(int argc, char **argv){

  int qthresh;
  FILE *fasta;
  FILE *qual;
  FILE *masked;
  int nseq;
  int MASK_MODE;
  char fastaname[SEQ_LENGTH];
  char qualname[SEQ_LENGTH];
  char fname[SEQ_LENGTH];
  char fname2[SEQ_LENGTH];
  int i,j,w;
  int consec;
  int thisgood;
  int dumpthis;
  int windowmask;
  int internalstart;
  int internalend;
  int lowq;
  int cnt;

  if (argc < 5){
    fprintf(stderr, "\n\nGiven one fasta sequence file; and a corresponding quality file; it masks the bases\nwith quality value less than given threshold with the letter \'N\'.\n");
    fprintf(stderr, "Last Update: March 16, 2007\n");
    fprintf(stderr, "\nUsage:\n\t%s -f [fasta_file] -qf [quality_file] [-q quality_threshold] [-mask=MASK_MODE] <-consec=consecutive_good_qual>\n", argv[0]);
    fprintf(stderr, "\t\t-mask=low:\tLower Case Masking\n");
    fprintf(stderr, "\t\t-mask=N:\tN Masking\n");
    fprintf(stderr, "\t\t-mask=X:\tX Masking\n");
    return 0;
  }
  
  fastaname[0] = 0;
  qualname[0] = 0;
  qthresh = 0;
  consec = 0;
  windowmask  = 0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-f"))
      strcpy(fastaname, argv[i+1]);
    else if (!strcmp(argv[i], "-qf"))
      strcpy(qualname, argv[i+1]);
    else if (!strcmp(argv[i], "-q"))
      qthresh = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-mask")){
      if (!strcmp(argv[i+1], "N"))
	MASK_MODE=MASK_N;
      else if (!strcmp(argv[i+1], "X"))
	MASK_MODE=MASK_X;
      else if (!strcmp(argv[i+1], "low"))
	MASK_MODE=MASK_LOW;      
      else{
	fprintf(stderr, "Invalid Mask Mode. Choose from:\n");
	fprintf(stderr, "\t\t-mask low:\tLower Case Masking\n");
	fprintf(stderr, "\t\t-mask N:\tN Masking\n");
	fprintf(stderr, "\t\t-mask X:\tX Masking\n");
	return 0;
      }
    }
    else if (!strcmp(argv[i], "-consec"))
      consec = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-window"))
      windowmask = atoi(argv[i+1]);
    
  }

  if (qthresh <= 0){
    fprintf(stderr, "Invalid threshold value %s.\n",argv[3]);
    return 0;
  }


  if (fastaname[0] == 0 || qualname[0] == 0){
    printf("Wrong file name.\n");
    return 0;
  }

  consec = 0;

  fasta = fopen(fastaname, "r");
  qual  = fopen(qualname, "r");
  
  if (fasta == NULL){
    fprintf(stderr, "Unable to open sequence file %s.\n", argv[1]);
    return 0;
  }
  
  if (qual == NULL){
    fprintf(stderr, "Unable to open quality file %s.\n", argv[2]);
    return 0;
  }
  
  nseq = readSingleFasta(fasta, qual, argv);
  fclose(fasta);
  fclose(qual);
  
  
  /* here all the names, sequence and quality should have been read */
  strcpy(fname2, fastaname);
  for (i=strlen(fname2)-1;i>=0;i--){
    if (fname2[i] == '.'){
      fname2[i] = 0;
      break;
    }
  }
  
  if (consec == 0 && windowmask == 0)
    sprintf(fname, "%s.masked.q%d.fa", fname2, qthresh);
  else if (windowmask == 0)
    sprintf(fname, "%s.masked.q%d.consec%d.fa", fname2, qthresh, consec);
  else
    sprintf(fname, "%s.masked.q%d.window%d.fa", fname2, qthresh, windowmask);

  masked = fopen(fname, "w");
  fprintf(stderr, "\nMasking in progress.\n");
  dumpthis = 0;
  if (consec == 0)
    dumpthis = 1;

  cnt = 0;

  for (i=0;i<nseq;i++){
    cnt++;
    fprintf(stderr, "\r%d\tof\t%d", cnt, nseq );

    if (consec!=0){
      dumpthis = 0;
      thisgood = 0;
      for (j=0;j<length[i];j++){
	if (quality[i][j]>=qthresh){
	  thisgood++;
	}
	else if (thisgood >= consec){
	  dumpthis = 1;
	  break;
	}
	else
	  thisgood = 0;
      }
    }
    
    if (!dumpthis)
      continue;

    fprintf(masked, ">%s\n", names[i]);
    if (windowmask == 0){
      for (j=0;j<length[i];j++){
	if (j!=0 && j%60==0)
	  fprintf(masked, "\n");
	if (quality[i][j]<qthresh){
	  switch(MASK_MODE){
	  case MASK_LOW:
	    fprintf(masked, "%c", tolower(seqs[i][j]));
	    break;
	  case MASK_N:
	    fprintf(masked, "N");
	    break;
	  case MASK_X:
	    fprintf(masked, "X");
	    break;
	  default:
	    break;
	  }
	}
	else
	  fprintf(masked, "%c", seqs[i][j]);
      }
    }

    else{  // quality end trimming here

      internalstart = 0; internalend=length[i];

      j = 0;

      while (j<length[i]){
	lowq = 0;
	for (w=j;w<j+windowmask && w<length[i];w++){
	  if (quality[i][w]<qthresh)
	    lowq++;
	}
	//printf("j:%d lowq: %d\n", j, lowq);
	//getchar();
	if (lowq < windowmask/2){
	  internalstart = j;
	  break;
	}
	j++;
      }

      j = length[i]-1;

      while (j>=0){
	lowq = 0;
	for (w=j;w>j-windowmask && w>=0;w--){
	  if (quality[i][w]<qthresh)
	    lowq++;
	}
	if (lowq < windowmask/2){
	  internalend = j;
	  break;
	}
	j--;
      }

      //printf("name: %s\twm: %d\tis: %d\tie: %d\n", names[i], windowmask, internalstart, internalend);
      for (j=0;j<length[i];j++){
	if (j!=0 && j%60==0)
	  fprintf(masked, "\n");
	if (j<internalstart || j>internalend){
	  switch(MASK_MODE){
	  case MASK_LOW:
	    fprintf(masked, "%c", tolower(seqs[i][j]));
	    break;
	  case MASK_N:
	    fprintf(masked, "N");
	    break;
	  case MASK_X:
	    fprintf(masked, "X");
	    break;
	  default:
	    break;
	  }
	}
	else
	  fprintf(masked, "%c", seqs[i][j]);
      }


    }


    fprintf(masked, "\n");	
  }

  fclose(masked);

  fprintf(stderr, "\nMasked sequences are dumped to file: %s\n\n", fname);

  return 1;
}

      
int readSingleFasta(FILE *fastaFile, FILE *qualFile, char **argv){
  int cnt;
  char ch; 
  int i,j;
  int index;
  int seqcnt=0, seqlen=0;
  int maxlen;
  char dummy[SEQ_LENGTH];
  char str[SEQ_LENGTH];
  char *this_name;
  int hasWarned;
  int willHash;
  char firstseq[SEQ_LENGTH];
  char firstqual[SEQ_LENGTH];

  firstseq[0]=0;
  firstqual[0]=0;

  hasWarned = 0;
  
  cnt = 0; i=0;
  willHash = 0;
  fprintf(stderr, "Counting sequences in FASTA.\n");
  maxlen=0;
  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      if (seqlen>maxlen)
	maxlen=seqlen;
      cnt++;
      printf("\r%d", cnt);
      seqlen=0;
      my_fgets(dummy, SEQ_LENGTH, fastaFile);
      if (firstseq[0] == 0){
	strcpy(firstseq, dummy);
	i=strlen(firstseq)-1;
	while(isspace(firstseq[i]))
	  firstseq[i--]=0;
      }
	
    }
    else if (!isspace(ch))
      seqlen++;
  }

  printf("\n");
  if (seqlen>maxlen)
    maxlen=seqlen;

  seqcnt = cnt;

  cnt = 0;

  fprintf(stderr, "Counting sequences in QUALITY.\n");

  while (fscanf(qualFile, "%c", &ch) > 0){
    my_fgets(dummy, SEQ_LENGTH, qualFile);
    if (strchr(dummy, '>') || ch=='>')
      cnt++;
    printf("\r%d", cnt);
    if (firstqual[0] == 0){
      strcpy(firstqual, dummy);
      i=strlen(firstqual)-1;
      while(isspace(firstqual[i]))
	firstqual[i--]=0;
    }
  }

  printf("\n");

  if (seqcnt != cnt){
    fprintf(stderr, "Number of sequences in fasta and quality files do not match: %d vs. %d!\n", seqcnt, cnt);
    exit(0);
  }


  if (strcmp(firstseq, firstqual)){
    fprintf(stderr, "\n\n*****************************************************************\n");
    fprintf(stderr, "It looks like the sequence/quality orders do not match. This will take some time.\nEnabling hash.\n");
    hasWarned = 1;  
    willHash = 1;
    fprintf(stderr, "If this runs really slow (that means there are too many sequences, the following is recommended, prior to run:\n");
    fprintf(stderr, "\tfastasort.sh %s\n", argv[1]);
    fprintf(stderr, "\tfastasort.sh %s\n", argv[2]);
    fprintf(stderr, "*****************************************************************\n");
    /*
    fprintf(stderr, "\tfastatotab < %s | sort > %s.tab\n", argv[1], argv[1]);
    fprintf(stderr, "\tfastatotab < %s | sort > %s.tab\n", argv[2], argv[2]);
    fprintf(stderr, "\ttabtofasta %s.tab %s\n", argv[1], argv[1]);
    fprintf(stderr, "\ttabtofasta %s.tab %s\n", argv[2], argv[2]);
    fprintf(stderr, "rm -f %s.tab %s.tab\n\n", argv[1], argv[2]);    
    */
  }

  length = (int *) malloc((seqcnt) * sizeof(int));


  cnt = 0; i=0;

  fprintf(stderr, "Allocating memory for %d sequences with max length %d.\n", seqcnt, maxlen);
  
  this_name = (char *) malloc(SEQ_LENGTH * sizeof(char));
  
  seqs = (char **) malloc((seqcnt) * sizeof(char *));
  
  for (i=0; i<seqcnt; i++)
    seqs[i] = (char *) malloc(maxlen);
  

  names = (char **) malloc((seqcnt) * sizeof(char *));

  for (i=0; i<seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);
  
  
  
  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
  }
  

  fprintf(stderr, "Reading sequences.\n");
  rewind(fastaFile);
  cnt = -1; 
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", cnt, seqcnt );
      /*

      */
      if (willHash){
	my_fgets(this_name, SEQ_LENGTH, fastaFile);
	this_name[strlen(this_name)-1] = 0;
	index=hash(this_name, seqcnt, INSERT);
	strcpy(names[index], this_name);
      }
      else{
	my_fgets(names[cnt], SEQ_LENGTH, fastaFile);
	names[cnt][strlen(names[cnt])-1] = 0;
      }
    }
    i = 0;
    if (cnt != 0){
      if (willHash)
	seqs[index][i++] = ch;
      else
	seqs[cnt][i++] = ch;
    }
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n'){
	if (willHash)
	  seqs[index][i++] = ch;
	else
	  seqs[cnt][i++] = ch;
      }
    } while (ch != '>');

    if (willHash)
      seqs[index][i] = 0;
    else
      seqs[cnt][i] = 0;
    
    if (ch == '>'){
      cnt++;
      fprintf(stderr, "\r%d\tof\t%d", cnt, seqcnt );
      if (cnt != seqcnt){
	if (willHash){
	  my_fgets(this_name, SEQ_LENGTH, fastaFile);
	  this_name[strlen(this_name)-1] = 0;
	  index=hash(this_name, seqcnt, INSERT);
	  strcpy(names[index], this_name);
	}
	else{
	  my_fgets(names[cnt], SEQ_LENGTH, fastaFile);
	  names[cnt][strlen(names[cnt])-1] = 0;
	}

      }
    } // if
  } // while
	    

  fprintf(stderr, "\n[OK] %d sequences read from fasta file.\n",seqcnt);

  fprintf(stderr, "Allocating memory for the quality values.\n");
  quality = (int **) malloc((seqcnt) * sizeof(int *));

  for (i=0;i<seqcnt;i++){
    length[i] = strlen(seqs[i]);
    quality[i] = (int *) malloc(length[i] * sizeof(int));
    if (isspace(names[i][strlen(names[i])-1]))
      names[i][strlen(names[i])-1] = 0;
  }


  //fprintf(stderr, "name0: %s.\n", names[0]);


  fprintf(stderr, "Reading quality values.\n");
  rewind(qualFile);
  
  
  cnt = 0;
  i = 0; index=i;
  while(fscanf(qualFile, "%s", str) > 0){
    
    if (str[0]=='>'){
      my_fgets(dummy, SEQ_LENGTH, qualFile);
      dummy[strlen(dummy)-1] = 0;
      sprintf(this_name,"%s%s", str, dummy);
      j=strlen(this_name)-1;
      while(isspace(this_name[j]))
	this_name[j--]=0;
      index = i;
      cnt = 0;
      if (strcmp(names[i], (this_name+1))){
	if (!hasWarned){
	  fprintf(stderr, "\n\n*****************************************************************\n");
	  fprintf(stderr, "It looks like the sequence/quality orders do not match. This will take some time.\n");
	  hasWarned = 1;
	  fprintf(stderr, "If this runs really slow (that means there are too many sequences, the following is recommended, prior to run:\n");
	  fprintf(stderr, "\tfastasort.sh %s\n", argv[1]);
	  fprintf(stderr, "\tfastasort.sh %s\n", argv[2]);
	  fprintf(stderr, "*****************************************************************\n");
	  /*
	  fprintf(stderr, "\tfastatotab < %s | sort > %s.tab\n", argv[1], argv[1]);
	  fprintf(stderr, "\tfastatotab < %s | sort > %s.tab\n", argv[2], argv[2]);
	  fprintf(stderr, "\ttabtofasta %s.tab %s\n", argv[1], argv[1]);
	  fprintf(stderr, "\ttabtofasta %s.tab %s\n", argv[2], argv[2]);
	  fprintf(stderr, "rm -f %s.tab %s.tab\n\n", argv[1],
	  argv[2]);
	  */
	}
	index = find_name((this_name+1), seqcnt, willHash);
      }
      if (index == -1){
	fprintf(stderr, "No matching sequence name for the qual %s.\nNote that the names MUST BE EXACTLY the same.\n", (this_name+1));
	exit(0);
      }
      i++;
      fprintf(stderr, "\r%d\tof\t%d", i, seqcnt );
    }

    else{
      if (cnt == length[index]){
	fprintf(stderr, "Sequence and quality lengths do not match for : %s\n", names[index]);
	exit(0);
      }
      quality[index][cnt++] = atoi(str);
    }

  }
  


  fprintf(stderr, "\n[OK] %d sequences read from quality file.\n",seqcnt);


  return seqcnt;

}

	 
int find_name(char *this_name, int nseq, int willHash){
  int i;

  //fprintf(stderr, "search for %s\n", this_name);

  if (willHash){
    i = hash(this_name, nseq, SEARCH);
    if(!strcmp(names[i], this_name))
      return i;
  }
  else{
    for (i=0;i<nseq;i++)
      if(!strcmp(names[i], this_name))
	return i;
  }

  return -1;
}


int hash(char *this_name, int nseq, int mode){
  int i;
  long sum;
  //int prime=3160979;
  int index;
  unsigned int a;


  sum = 0;

  a=131;
  for (i=0;i<strlen(this_name);i++){
    sum=(sum*a)+this_name[i]; //bkdrhash                                                                                                                                                                                                          
  }

  index = sum % nseq;

  //  fprintf(stderr, "index %d\n", index);

  if (mode == INSERT){
    while (names[index][0] != 0){
      //  fprintf(stderr, "looking %s\n", this_name);
      index++;
      if (index == nseq)
	index = 0;
    }
  }
  else{
    i = index;
    //fprintf(stderr, "hash search for %s\n", this_name);
    while (strcmp(names[index], this_name)){
      index++;
      if (index == nseq)
	index = 0;
      if (index == i)
	return 0;
    }
  }

  return index;

}

  
void my_fgets(char *str, int length, FILE *in){
  char ch;
  int i=0;
  while (i<length && fscanf(in, "%c", &ch)){
    if ((ch==' '  || ch=='\t')  && i!=0){
      if (str[i-1] == ' ' || str[i] == '\t')
	;
      else
	str[i++] = ch;
    }
    else
      str[i++] = ch;
    if (ch == '\n' || ch=='\r')
      break;
  }
  str[i] = 0;
}
