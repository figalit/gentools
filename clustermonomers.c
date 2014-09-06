

/*

  TODO
  as of Nov 22,2005 18:54
  delete all INVALIDs  done Nov 17
  delete single-cluster reads done Nov 17
  group pattern matches - done Nov 17

  output clusters - done Nov 22
  change checkpattern - done Nov 22

  Dec 14, 2005: read/alphasat begin/end locations are
  being logged
  
  March 20, 2006 - patternsets less thatn PATTSET_MIN are removed

*/

#define CLUSTERMONOMERS
#include "pimp.h"

char **names;
char **seqs;


int readSingleFasta(FILE *, int);


int main(int argc, char **argv){
  char str[1000];
  FILE *fasta;
  FILE *log;
  FILE *clusterFile;
  char cfbase[100];
  int i,j;
  int count;
  int nseq;
  int mismatch;
  int score;
  int threepass=0;
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

  if (argc != 3 && argc != 4 && argc != 5){
    fprintf(stderr, "Clusters duplicate sequences with some mismatch threshold.\n");
    fprintf(stderr, "Last update: Dec 14, 2005.\n");
    fprintf(stderr, "%s [fasta file] [mismatch threshold] <-3pass> <-align>\n", argv[0]);
    exit(0);
  }
  

  lemming = 0;
  if (argc > 3){
    for (i=3; i<argc; i++){
      if (!strcmp(argv[i], "-3pass"))
	threepass = 1;
      else if (!strcmp(argv[i], "-align"))
	lemming = 1;
    }
  }

  fasta = fopen(argv[1], "r");
  
  if (fasta == NULL){
    fprintf(stderr, "Unable to open file %s\n", argv[1]);
    return 0;
  }

  fprintf(stderr, "MIN CLUSTER SIZE: %d\n", CLUSTERCUTOFF);
  sprintf(str, "%s-m%s.clusters.log", argv[1],argv[2]);
  log = fopen(str, "w");
  mismatch = atoi(argv[2]);
  strcpy(cfbase, argv[1]);
  for (i=0;i<strlen(cfbase);i++){
    if (cfbase[i] == '.'){
      cfbase[i] = 0;
      break;
    }
  }


  //alloc_cluster(&allclusters);

  allclusters = NULL;
  nseq = readSingleFasta(fasta, ALPHASATLEN);
  fprintf(stderr, "%d monomers are read into memory.\n", nseq);


  makevectors();
  i = 0;

  //insert_cmember(&allclusters, 1); // first guy in


  for (i=0;i<nseq;i++){

    if (allclusters == NULL){ 
      alloc_cluster(&allclusters);
      allclusters->nmember++;
      alloc_cmember(&(allclusters->members));
      allclusters->members->seqno=i;
      continue;
    }
    
    isInserted = 0;
    current = allclusters;
    while (current != NULL){
      iflag = 0;
      cm = current->members;
      while (cm != NULL){
	score = my_strcmp(seqs[cm->seqno], seqs[i], mismatch);
	if (!score)
	  iflag = 1;
	else{
	  iflag = 0;
	  break; 
	}
	cm = cm->next;
      }
      if (iflag){ // insertable
	insert_cmember(&current, i);
	isInserted = 1;
	break;
      }
      prevcluster = current;
      current = current->next;
    }
    // otherwise, current is now NULL, i is not inserted to any
    // existing place, so create a new one
    
    if (!isInserted){
      alloc_cluster(&current);
      current->nmember++;
      alloc_cmember(&(current->members));
      current->members->seqno=i; 
      prevcluster->next = current;
    }
    fprintf(stderr, "\rDoing something %d%%", (int)((float)(i+1)/(float)(nseq)*100));
  }


  /*
  
  for (i=0;i<nseq;i++){
    for (j=i+1;j<nseq;j++){
      //score = my_strcmp(seqs[i], seqs[j], mismatch);
      score = strcmp(seqs[i], seqs[j]);
      if (score == mismatch){
	// if I can live with this
	// let's try to do this assuming mismatch = 0 
	// the other way will be a "little" messier.
	current = find_cluster(allclusters, i);
	insert_cmember(&current, j);
      }
    }
    printf("\rDoing something %d%%", (int)((float)(i+1)/(float)(nseq)*100));
  }

  */

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
      fprintf(log, "\nCluster size: %d\nCluster ID:%d\n------------\n", current->nmember,cluster_id);
      cm = current->members;
      sprintf(str, "%s.cluster.%d.fa", cfbase, cluster_id);
      clusterFile = fopen(str, "w");
      while (cm != NULL){
	fprintf(log, "Member %5d -=- %s\n", cm->seqno, names[cm->seqno]);
	
	fprintf(clusterFile, ">%s\n", names[cm->seqno]);
	for (i=0;i<strlen(seqs[cm->seqno]);i++){
	  if (i!=0 && i%60==0)
	    fprintf(clusterFile, "\n");
	  fprintf(clusterFile, "%c", seqs[cm->seqno][i]);
	}
	fprintf(clusterFile, "\n");
	
	/* break the name */
	breakname(names[cm->seqno], &wgsname, &wgsstart, &wgsend);
	/* now, try to insert */

	if (allreads == NULL){
	  alloc_wgsread(&allreads);
	  strcpy(allreads->name, wgsname);
	  allreads->hasBigBrother = FALSE;
	}

	insert_asat(&allreads, wgsname, wgsstart, wgsend, cluster_id);
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
      
      /*
      if (wgs1->orientation == FORWARD){
	asat = wgs1->members;
	while (asat != NULL){
	  if (asat->start>asat->end)
	    fprintf(stderr, "first pass invalid forward orientation wgs1 at %s\n", wgs1->name);	    
	  asat = asat->next;
	}
      }

      else if (wgs1->orientation == REVERSE){
	asat = wgs1->members;
	while (asat != NULL){
	  if (asat->start<asat->end)
	    fprintf(stderr, "first pass invalid reverse orientation wgs1 at %s\n", wgs1->name);	    
	  asat = asat->next;
	}
      }

      */


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
  

  /*
  fprintf(stderr, "After remove\n");
  
  */
  wgs1=allreads;
  while (wgs1 != NULL){
    copy_members(&wgs1);

    if (checkPattern2(wgs1->members, wgs1->amembers) == FALSE){
      fprintf(log, "***** CORRUPT MEMBERS/AMEMBERS in wgs: %s\n", wgs1->name);
    }
    /*
    if (wgs1->orientation == FORWARD){
      asat = wgs1->members;
      while (asat != NULL){
	if (asat->start>asat->end)
	  fprintf(stderr, "second pass invalid forward orientation wgs1 at %s\n", wgs1->name);	    
	asat = asat->next;
      }
    }
    
    else if (wgs1->orientation == REVERSE){
      asat = wgs1->members;
      while (asat != NULL){
	if (asat->start<asat->end)
	  fprintf(stderr, "second pass invalid reverse orientation wgs1 at %s\n", wgs1->name);	    
	asat = asat->next;
      }
    }
    */
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
	/*	fprintf(log, "pmatch: read1: %s\tpattern: ", wgs1->name);
	print_clusterid(wgs1->amembers, log);
	fprintf(log, "\tread2: %s\tpattern: ", wgs2->name);	
	print_clusterid(wgs2->amembers, log);*/
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

  if (threepass){
  
  
    fprintf(stderr, "Pass #3\n");
    
    wgs1 = allreads;
    
    
    while (wgs1 != NULL){
      if (wgs1->hasBigBrother == TRUE){
	wgs1 = wgs1->next;
	continue;
      }
      wgs2 = wgs1->next;
      //wgs2 = allreads;
      
      while (wgs2!=NULL){
	if (strcmp(wgs1->name, wgs2->name) && checkPattern4(wgs1->amembers, wgs2->amembers)){
	  merge_sibling(&wgs1, wgs2);
	  wgs2->hasBigBrother = TRUE;
	}
	wgs2 = wgs2->next;
      }
      wgs1 = wgs1->next;
    }
  }


  /*
  fprintf(stderr, "After add_sibling\n");

  wgs1=allreads;
  while (wgs1 != NULL){
    if (wgs1->orientation == FORWARD){
      asat = wgs1->members;
      while (asat != NULL){
	if (asat->start>asat->end)
	  fprintf(stderr, "third pass invalid forward orientation wgs1 at %s\n", wgs1->name);	    
	asat = asat->next;
      }
    }
    
    else if (wgs1->orientation == REVERSE){
      asat = wgs1->members;
      while (asat != NULL){
	if (asat->start < asat->end)
	  fprintf(stderr, "third pass invalid reverse orientation wgs1 at %s\n", wgs1->name);	    
	asat = asat->next;
      }
    }
    wgs1 = wgs1->next;
  }
  */

  
  /*
    TODO Here:
    MERGE wgs1 and wgs2; try to eliminate duplicates
    dump wgs's to cfbase.patternset%d.txt
   */

  

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

  /*

  fprintf(stderr, "After merge_sibling\n");

  wgs1=allreads;
  while (wgs1 != NULL){
    if (wgs1->orientation == FORWARD){
      asat = wgs1->members;
      while (asat != NULL){
	if (asat->start>asat->end)
	  fprintf(stderr, "second pass invalid forward orientation wgs1 at %s\n", wgs1->name);	    
	asat = asat->next;
      }
    }
    
    else if (wgs1->orientation == REVERSE){
      asat = wgs1->members;
      while (asat != NULL){
	if (asat->start<asat->end)
	  fprintf(stderr, "second pass invalid reverse orientation wgs1 at %s\n", wgs1->name);	    
	asat = asat->next;
      }
    }
    wgs1 = wgs1->next;
  }

  */

  fprintf(stderr, "Dumping patterns.\n");
  fprintf(log, "\n\nPATTERNS ARE HERE: \n----\n\n");

  wgs1 = allreads;

  pattern_id = 1;

  while (wgs1 != NULL){
    if ((wgs1->hasBigBrother == FALSE) && (wgs1->sibling != NULL) && wgs1->nsibs>=PATTSET_MIN){

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
	/*
	if (wgs2->orientation == FORWARD){
	  asat = wgs2->members;
	  while (asat != NULL){
	    if (asat->start>asat->end)
	      fprintf(stderr, "invalid forward orientation wgs2 at %s\n", wgs2->name);	    
	    asat = asat->next;
	  }
	}
	
	else if (wgs2->orientation == REVERSE){
	  asat = wgs2->members;
	  while (asat != NULL){
	    if (asat->start<asat->end)
	      fprintf(stderr, "invalid reverse orientation wgs2 at %s\n", wgs2->name);	    
	    asat = asat->next;
	  }
	}
	*/

	if (wgs2->members == NULL){
	  wgs2 = wgs2->next;
	  continue;
	}

	fprintf(log, "%s -=- %c\t", wgs2->name, wgs2->orientation==FORWARD?'F':'R');
	fprintf(clusterFile, "%s -=- %c ", wgs2->name, wgs2->orientation==FORWARD?'F':'R');
	/* hmm there's something wrong here, probably because I
	   merged the members
	   no, dumbass, I merged the siblings, not members
	   no, while merging the siblings I also merged the members
	   whatever
	*/
	
	asat = wgs2->members;

	patternasat = wgs1->amembers;
	
	/*
	while(asat->cluster_id != patternasat->cluster_id)
	  asat = asat->next;	
	fprintf(clusterFile, "%d ", asat->start);
	
	
	while (patternasat->next != NULL)
	  patternasat = patternasat->next;
	

	while (asat->next != NULL && asat->cluster_id != patternasat->cluster_id)
	  asat = asat->next;
	*/
	
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


  fprintf(stderr, "\nUnless I'm hallucinating, I'm done.\n");
  

  return 1;
}



int readSingleFasta(FILE *fastaFile, int maxlength){
  int cnt;
  char ch; 
  int i;
  int seqcnt=0, seqlen=0;

  cnt = 0; i=0;

  rewind(fastaFile);
  while (fscanf(fastaFile, "%c", &ch) > 0){
    if (ch == '>')
      cnt++;
  }

  seqcnt = cnt;
  seqlen = maxlength;

  cnt = 0; i=0;

  rewind(fastaFile);
  fprintf(stderr, "seqcnt: %d seqlen: %d\n", seqcnt, seqlen);

  seqs = (char **) malloc((seqcnt+1) * sizeof(char *));
  
  for (i=0; i<=seqcnt; i++)
    seqs[i] = (char *) malloc(seqlen);
  

  names = (char **) malloc((seqcnt+1) * sizeof(char *));

  for (i=0; i<=seqcnt; i++)
    names[i] = (char *) malloc(SEQ_LENGTH);

  for (i=0; i<seqcnt; i++){
    seqs[i][0] = 0;
    names[i][0] = 0;
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


  return seqcnt;

} 


