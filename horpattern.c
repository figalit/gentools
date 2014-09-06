/*

As of Dec 14, 2005
TODO:
dump the hor-match patterns into seperate files.
    
*/


#define HORPATTERN

#include "pimp.h"


char **hornames;
char **horseqs;
char **names;
char **seqs;
int  *clustered;


void readFasta(FILE *, FILE *, int *, int *);

int main(int argc, char **argv){
  char str[1000];
  FILE *fasta;
  FILE *hor;
  FILE *log;
  FILE *histogram;
  FILE *plotfile;
  int i,j;
  int count;
  int nseq, nhor;
  int *freqs;
  int swapfreq;
  int mismatch;

  /* pattern match stuff */

  FILE *clusterFile;
  char cfbase[100];
  struct cluster *allclusters;
  struct cluster *prevcluster;
  struct cluster *current;
  struct cmember *cm;
  struct wgsread *allreads;
  struct wgsread *wgs1, *wgs2;
  struct asatseq *asat;
  struct asatseq *patternasat;
  char *wgsname;
  int wgsstart, wgsend;
  int iflag;
  int isInserted;
  int cluster_id;
  int pattern_id;
  int occurance;
  int isFINALHOR;

  /* pattern match stuff ends here */

  if (argc != 4 && argc != 5){
    fprintf(stderr, "Matches sequences to (exact or with given threshold) highorders given a two fasta-hor files.\nThen finds the patterns\n");
    fprintf(stderr, "%s [fasta file][hor fasta][threshold] <-align>\n", argv[0]);
    exit(0);
  }
  
  fasta = fopen(argv[1], "r");
  hor = fopen(argv[2], "r");
  
  lemming = 0;
  if (argc==5 && !strcmp(argv[4], "-align"))
    lemming=1;

  if (fasta==NULL||hor==NULL){
    fprintf(stderr, "Non existent file.\n");
    return 0;
  }
  
  fprintf(stderr, "MIN CLUSTER SIZE: %d\n", CLUSTERCUTOFF);

  makevectors();
  sprintf(str, "%s-%s.m%s.log", argv[1],argv[2], argv[3]);
  log = fopen(str, "w");
  sprintf(str, "%s-%s.m%s.histogram", argv[1],argv[2],argv[3]);
  histogram = fopen(str, "w");
  mismatch = atoi(argv[3]);

  /* PMS BEGIN */
  strcpy(cfbase, argv[1]);
  for (i=0;i<strlen(cfbase);i++){
    if (cfbase[i] == '.'){
      cfbase[i] = 0;
      break;
    }
  }
  /* PMS END */


  readFasta(fasta, hor, &nseq, &nhor);
  fprintf(stderr, "%d monomers, %d highorders are read into memory.\n", nseq,nhor);

  freqs = (int *) malloc(sizeof(int)*nhor);
  for (i=0;i<nhor;i++){
    freqs[i] = 0;
  }

  allclusters = NULL;

  for (i=0;i<nhor;i++){

    
    /* pattern match stuff */
    if (allclusters == NULL){ 
      alloc_cluster(&allclusters);
      allclusters->nmember++;
      alloc_cmember(&(allclusters->members));
      allclusters->members->seqno=i;
      current = allclusters;
    }
    else{
      current = allclusters;
      while (current->next != NULL){
	current = current->next;
      }
      alloc_cluster(&(current->next));
      current->next->nmember++;
      alloc_cmember(&(current->next->members));
      current->next->members->seqno=i;
      current = current->next;
    }
    /* pattern match stuff ends here */
    
    count = 0;
    fprintf(log, "HIGH ORDER: %s\n----------\n", hornames[i]);
    for (j=0;j<nseq;j++){
      if (!my_strcmp(horseqs[i], seqs[j], mismatch)){
	//if (count == 0)
	fprintf(log, "%s\n", names[j]);
	count++;
	clustered[j]++;
	insert_cmember(&current,j);
      }
    }
    //    if (count != 0)
    fprintf(log, "%s MATCHES: %d\n\n", hornames[i], count);
    ///fprintf(histogram, "%d\n",count);
    freqs[i] = count;
    printf("\r%f%%", (float)(i+1)/(float)(nhor)*100);
    fflush(stdout);
  }
  
  /* PMS BEGIN */
  
  fprintf(stderr, "\n");
  current = allclusters->next;
  prevcluster = allclusters;
  
  
  /* remove small clusters */

  fprintf(stderr, "Contracting clusters.\n");

  while (current != NULL){
    if (current->nmember < CLUSTERCUTOFF){
      //printf("Deleting Cluster with %d members\n", current->nmember);
      prevcluster->next = current->next;
      free_cluster(current);
      current = prevcluster->next;
      continue;
    }
    prevcluster = current;
    current = current->next;
  }



  if (allclusters->nmember < CLUSTERCUTOFF){
    current = allclusters;
    allclusters = allclusters->next;
    free_cluster(current);
  }

  fprintf(stderr, "ID'ing clusters and creating read sets.\n");

  /* log the clusters, and ID them, put to allreads */

  cluster_id = 1;
  current = allclusters;
  allreads = NULL;

  wgsname = (char *) malloc (NAMELENGTH * sizeof(char));

  while (current != NULL){
    if (current->nmember >= CLUSTERCUTOFF){
      current->id = cluster_id;
      fprintf(log, "\nCluster size: %d\nCluster ID:%d\nFINALHOR name: %s\n------------\n", current->nmember,cluster_id, hornames[current->members->seqno]);
      cm = current->members;
      isFINALHOR=1;
      sprintf(str, "%s.cluster.%d.fa", cfbase, cluster_id);
      clusterFile = fopen(str, "w");
      while (cm != NULL){
	if (!isFINALHOR){
	  fprintf(log, "Member %5d -=- %s\n", cm->seqno, names[cm->seqno]);
	  
	  fprintf(clusterFile, ">%s\n", names[cm->seqno]);
	  for (i=0;i<strlen(seqs[cm->seqno]);i++){
	    if (i!=0 && i%60==0)
	      fprintf(clusterFile, "\n");
	    fprintf(clusterFile, "%c", seqs[cm->seqno][i]);
	  }
	  fprintf(clusterFile, "\n");
	  breakname(names[cm->seqno], &wgsname, &wgsstart, &wgsend);

	  /* now, try to insert */
	  
	  if (allreads == NULL){
	    alloc_wgsread(&allreads);
	    strcpy(allreads->name, wgsname);
	    allreads->hasBigBrother = FALSE;
	  }
	  
	  insert_asat(&allreads, wgsname, wgsstart, wgsend, cluster_id);
	  
	}

	else{
	  fprintf(clusterFile, ">%s\n", hornames[cm->seqno]);
	  for (i=0;i<strlen(horseqs[cm->seqno]);i++){
	    if (i!=0 && i%60==0)
	      fprintf(clusterFile, "\n");
	    fprintf(clusterFile, "%c", horseqs[cm->seqno][i]);
	  }
	  fprintf(clusterFile, "\n");
	  isFINALHOR = 0;
	}
	
	
	cm = cm->next;
      }
      cluster_id++;
      fclose(clusterFile);
    }
    current = current->next;
  }

  /* print reads */

  fprintf(stderr, "Logging the reads/clusters info.\n");
  wgs1 = allreads;
  fprintf(log, "\nREADS/CLUSTERS ARE HERE: \n----\n\n");
  while (wgs1 != NULL){
    if (wgs1->orientation != INVALID){
      
      fprintf(log, "pmatch: read1: %s clusters: ", wgs1->name);
      print_clusterid(wgs1->members, log);
    }
    wgs1 = wgs1->next;
  }


  /* remove INVALIDs are single-asatseq members */

  
  fprintf(stderr, "Removing INVALID read sets, and sets with one members.\n");
  wgs1 = allreads->next;
  wgs2 = allreads;

  while (wgs1 != NULL){
    if (wgs1->orientation == INVALID || wgs1->members == NULL || wgs1->members->next == NULL){
      wgs2->next = wgs1->next;
      free_wgsread(wgs1);
      wgs1 = wgs2->next;
      continue;
    }
    wgs2 = wgs1;
    wgs1 = wgs1->next;
  }
    
  if (allreads->orientation == INVALID || allreads->members == NULL  || allreads->members->next == NULL){
    wgs1 = allreads;
    allreads = allreads->next;
    free_wgsread(wgs1);
  }
  

  wgs1=allreads;
  while (wgs1 != NULL){
    copy_members(&wgs1);

    if (checkPattern2(wgs1->members, wgs1->amembers) == FALSE){
      fprintf(log, "***** CORRUPT MEMBERS/AMEMBERS in wgs: %s\n", wgs1->name);
    }

    wgs1 = wgs1->next;
  }
  
  


  /* pattern matching */

  fprintf(stderr, "Checking for patterns, so help me Turing.\n");

  if (allreads == NULL || allreads->next == NULL){
    fprintf(stderr, "Either allreads or allreads->next, or even both are NULL.\nYou've got to be kidding me.\n");
    return FALSE;
  }

  fprintf(log, "\n\nPairwise Pattern Match Here\n\n");
  fprintf(log, "\n\nPass #1\n\n");

  fprintf(stderr, "Pass #1\n");

  wgs1 = allreads;


  while (wgs1 != NULL){
    if (wgs1->hasBigBrother == TRUE){
      wgs1 = wgs1->next;
      continue;
    }
    wgs2 = wgs1->next;
    //wgs2 = allreads;
    
    while (wgs2!=NULL){
      if (strcmp(wgs1->name, wgs2->name) && checkPattern3(wgs1->amembers, wgs2->amembers)){
	add_sibling(&wgs1, wgs2);
	wgs2->hasBigBrother = TRUE;
	fprintf(log, "pmatch: read1: %s\tpattern: ", wgs1->name);
	print_clusterid(wgs1->amembers, log);
	fprintf(log, "\tread2: %s\tpattern: ", wgs2->name);	
	print_clusterid(wgs2->amembers, log);
	fflush(log);
      }
      wgs2 = wgs2->next;
    }
    wgs1 = wgs1->next;
  }


  wgs1 = allreads;

  fprintf(stderr, "Pass #2\n");
  fprintf(log, "\n\nPass #2\n\n");

  while (wgs1 != NULL){
    if (wgs1->hasBigBrother == TRUE){
      wgs1 = wgs1->next;
      continue;
    }
    //wgs2 = wgs1->next;
    wgs2 = allreads;
    
    while (wgs2 != NULL){
      if (wgs2->hasBigBrother == TRUE){
	//fprintf(stderr, "pass..%s\n", wgs2->name);
	wgs2 = wgs2->next;
	continue;
      }
      if (strcmp(wgs1->name, wgs2->name) && checkPattern(&(wgs1->amembers), &(wgs2->amembers), log)){
	//add_sibling(&wgs1, wgs2); --> calkan dec 13
	merge_sibling(&wgs1, wgs2);
	//merge_sibling(&wgs2, wgs1);
	wgs2->hasBigBrother = TRUE;
	fprintf(log, "pmatch: read1: %s\tpattern: ", wgs1->name);
	print_clusterid(wgs1->amembers, log);
	fprintf(log, "\tread2: %s\tpattern: ", wgs2->name);	
	print_clusterid(wgs2->amembers, log);
	fflush(log);
      }
      wgs2 = wgs2->next;
    }
    wgs1 = wgs1->next;
  }


  fprintf(stderr, "Collapsing patterns.\n");


  wgs1 = allreads;

  while (wgs1 != NULL){
    if (wgs1->hasBigBrother == TRUE){
      wgs1 = wgs1->next;
      continue;
    }
    //wgs2 = wgs1->next;
    wgs2 = allreads;

    while (wgs2!=NULL){
      if (strcmp(wgs1->name, wgs2->name) && checkPattern3(wgs1->amembers, wgs2->amembers)){
	
	merge_sibling(&wgs1, wgs2);
	wgs2->hasBigBrother = TRUE;
      }
      wgs2 = wgs2->next;
    }
    wgs1 = wgs1->next;
  } 


  fprintf(stderr, "Dumping patterns.\n");
  fprintf(log, "\n\nPATTERNS ARE HERE: \n----\n\n");

  wgs1 = allreads;

  pattern_id = 1;

  while (wgs1 != NULL){
    if ((wgs1->hasBigBrother == FALSE) && (wgs1->sibling != NULL)){

      fprintf(log, "\nPattern ID: %d", pattern_id);
      fprintf(log, "\nOrder: ");
      sprintf(str, "%s.patternset.%d.txt", cfbase, pattern_id);
      pattern_id++;
      clusterFile = fopen(str, "w");
      occurance=1;
      wgs2 = wgs1->sibling;
      while (wgs2 != NULL){
	if (wgs2->members != NULL)
	  occurance++;
	wgs2 = wgs2->sibling;
      }
      print_clusterid(wgs1->amembers, log);
      fprintf(log, "Occurance: %d\nMembers: \n%s -=- %c\t", occurance, wgs1->name, wgs1->orientation==FORWARD?'F':'R');
      fprintf(clusterFile, "%s -=- %c ", wgs1->name, wgs1->orientation==FORWARD?'F':'R');

      asat = wgs1->members;
      patternasat = wgs1->amembers;

      /*
	while(asat->cluster_id != patternasat->cluster_id)
	asat = asat->next;
      */
      fprintf(clusterFile, "%d ", asat->start);
      /*
	while (patternasat->next != NULL)
	patternasat = patternasat->next;
      */

      /*
	while (asat->next != NULL && asat->cluster_id != patternasat->cluster_id)
	asat = asat->next;
      */

      while (asat->next != NULL){
	fprintf(log, "%d\t", asat->cluster_id);
	asat = asat->next;
      }

      fprintf(log, "%d", asat->cluster_id);

      fprintf(log, "\t-\t");
      
      while (patternasat != NULL){
	fprintf(log, "%d\t", patternasat->cluster_id);
	patternasat = patternasat->next;
      }       
      
      fprintf(log, "\n");
      
      fprintf(clusterFile, "%d\n", asat->end);
      
      wgs2 = wgs1->sibling;
      while (wgs2 != NULL){

	if (wgs2->members == NULL){
	  wgs2 = wgs2->next;
	  continue;
	}

	fprintf(log, "%s -=- %c\t", wgs2->name, wgs2->orientation==FORWARD?'F':'R');
	fprintf(clusterFile, "%s -=- %c ", wgs2->name, wgs2->orientation==FORWARD?'F':'R');

	asat = wgs2->members;

	patternasat = wgs2->amembers;
	
	fprintf(clusterFile, "%d ", asat->start);

	while (asat->next != NULL){
	  fprintf(log, "%d\t", asat->cluster_id);
	  asat = asat->next; 
	}

	fprintf(log, "%d", asat->cluster_id);	  

	fprintf(log, "\t-\t");

	while (patternasat != NULL){
	  fprintf(log, "%d\t", patternasat->cluster_id);
	  patternasat = patternasat->next;
	}       


	fprintf(log, "\n");

	fprintf(clusterFile, "%d\n", asat->end);
	
	wgs2 = wgs2->sibling;
      }
      fclose(clusterFile);
    }
    wgs1 = wgs1->next;
  }


  
  fprintf(stderr, "\nPlotting data.\n");





  /* PMS END */

  /*
  
  for (i=0;i<nseq;i++)
    if (clustered[i]>1){
      printf("yes there are some invaders around: %s.\n", names[i]);
      //return 1;
    }
  */

  // sort freq, reorder names
  
  for (i=0;i<nhor;i++){
    for (j=1;j<nhor;j++){
      if (i<j && freqs[i]<freqs[j]){
	swapfreq = freqs[i];
	freqs[i] = freqs[j];
	freqs[j] = swapfreq;
	strcpy(str, hornames[i]);
	strcpy(hornames[i], hornames[j]);
	strcpy(hornames[j], str);
      }
    }
  }

  plotfile=fopen("plot", "w");
  fprintf(plotfile, "set terminal postscript eps enhanced\n");
  fprintf(plotfile, "set output \"%s-%s.m%s.histogram.eps\"\n",argv[1],argv[2],argv[3]);
  fprintf(plotfile, "set size 2,2\n");
  fprintf(plotfile, "set bmargin 15\nset tmargin -5\n");
  fprintf(plotfile, "unset xtics\n");
  for (i=0;i<nhor;i++){
    fprintf(plotfile, "set label \"%s\" at first %d,0 rotate by -90 font \"Times,8\" \n", hornames[i], i);
    fprintf(histogram, "%d\n", freqs[i]);
  }
  fprintf(plotfile, "plot \"%s-%s.m%s.histogram\" with boxes\n", argv[1], argv[2],argv[3]);
  fclose(histogram);
  fclose(plotfile);
  system("gnuplot plot");
  sprintf(str, "%s-%s.m%s.plotfile", argv[1], argv[2],argv[3]);
  rename("plot", str);
  printf("\n");

  fprintf(stderr, "\nUnless I'm hallucinating, I'm done.\n");

  return 1;
}



void readFasta(FILE *fastaFile, FILE *horFile, int *nseq, int *nhor){
  int cnt;
  char ch; 
  int i;
  int seqcnt=0, seqlen=0;
  int horseqcnt=0, horseqlen=0;

  cnt = 0; i=0;

  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }

  seqcnt = cnt;
  seqlen = ALPHASATLEN;

  cnt = 0; i=0;

  rewind(horFile);

  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }


  horseqlen = ALPHASATLEN;
  horseqcnt = cnt;
  
  rewind(fastaFile);
  fprintf(stderr, "seqcnt: %d seqlen: %d\n", seqcnt, seqlen);
  fprintf(stderr, "horseqcnt: %d horseqlen: %d\n", horseqcnt, horseqlen);

  horseqs = (char **) malloc((horseqcnt+1) * sizeof(char *));
  
  for (i=0; i<=horseqcnt; i++)
    horseqs[i] = (char *) malloc(horseqlen);
  
  hornames = (char **) malloc((horseqcnt+1) * sizeof(char *));

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen);

  clustered = (int *) malloc((seqcnt+1) * sizeof(int));
  

  for (i=0; i<=horseqcnt; i++)
    hornames[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<horseqcnt; i++){
    horseqs[i][0] = 0;
    hornames[i][0] = 0;
  }

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
    clustered[i] = 0;
  }

  cnt = -1; 
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(names[cnt], SEQ_LENGTH, fastaFile);
      names[cnt][strlen(names[cnt])-1] = 0;
      //printf("seq-%d: %s\n", cnt, names[0]);
    }
    i = 0;
    if (cnt != 0)
      seqs[cnt][i++] = ch;
    do{
      if (!(fscanf(fastaFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	seqs[cnt][i++] = ch;
    } while (ch != '>');
    seqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(names[cnt], SEQ_LENGTH, fastaFile);
	names[cnt][strlen(names[cnt])-1] = 0;
	//printf("seq-%d: %s\n", cnt, names[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read from fasta\n",seqcnt);


  // hor file
  cnt = 0; i=0;

  rewind(horFile);

  cnt = -1; 
  while (fscanf(horFile, "%c", &ch) > 0){
    if (ch == '>'){
      cnt++;
      fgets(hornames[cnt], SEQ_LENGTH, horFile);
      trim(hornames[cnt]);
      //      hornames[cnt][strlen(hornames[cnt])-1] = 0;
      //printf("seq-%d: %s\n", cnt, hornames[0]);
    }
    i = 0;
    if (cnt != 0)
      horseqs[cnt][i++] = ch;
    do{
      if (!(fscanf(horFile, "%c", &ch) > 0))
	break;
      if (ch!='>' && ch!='\r' && ch!='\n')
	horseqs[cnt][i++] = ch;
    } while (ch != '>');
    horseqs[cnt][i] = 0;
    if (ch == '>'){
      cnt++;
      if (cnt != seqcnt){
	fgets(hornames[cnt], SEQ_LENGTH, horFile);
	trim(hornames[cnt]);
	//hornames[cnt][strlen(hornames[cnt])-1] = 0;
	//printf("horseq-%d: %s\n", cnt, hornames[cnt]);
      }
    } // if
  } // while
	    

  printf("[OK] %d sequences read\n",horseqcnt);

  *nseq = seqcnt;
  *nhor = horseqcnt;
} 

