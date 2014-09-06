#include "pimp.h"

void trim(char *name){
  int i=0;
  int dash=0;
  while (!isspace(name[i++])){
    if (name[i-1] == '-') dash++;
    if (dash == 2) break;
  }
   
  name[i-1]=0;
}

int my_strcmp(char *s1, char *s2, int mismatch){
  if (lemming)
    return my_strcmpAlign(s1, s2, mismatch);
  else
    return my_strcmpHamming(s1, s2, mismatch);
}

int my_strcmpAlign(char *s1, char *s2, int mismatch){
  // return 0 if match is ok
  // 1 if not ok enough
  // backward compability thingy
  
  int mcnt=0;
  int i, len;

  align(s1, s2);

  len = strlen(Sp);

  for (i=0;i<len;i++){
    if (Sp[i] != Tp[i])
      if (Sp[i]!='-' && Tp[i]!='-') 
	mcnt++;
    if (mcnt > mismatch) return 1;
  }

  return 0;

}


int my_strcmpHamming(char *s1, char *s2, int mismatch){
  // return 0 if match is ok
  // 1 if not ok enough
  // backward compability thingy
  
  int mcnt=0;
  int i, len;

  if (mismatch == 0)
    return strcmp(s1,s2);

  len = strlen(s1);
  if (strlen(s2) != len)
    return 1;

  for (i=0;i<len;i++){
    if (s1[i] != s2[i])
      if (s1[i]!='-' && s2[i]!='-') 
	mcnt++;
    if (mcnt > mismatch) return 1;
  }

  return 0;

}




void alloc_cmember(struct cmember **cm){
  (*cm) = (struct cmember *) malloc (sizeof(struct cmember));
  (*cm)->next = NULL;
  (*cm)->seqno=-1;
}

void alloc_cluster(struct cluster **cl){
  (*cl) = (struct cluster *) malloc (sizeof(struct cluster));
  (*cl)->members = NULL;
  (*cl)->next = NULL;
  (*cl)->nmember = 0;
  (*cl)->id = 0;
}

void insert_cmember(struct cluster **cl, int seqno){
  struct cmember *cm2;
  struct cmember *new;

  if ((*cl)->members == NULL){
    alloc_cmember(&((*cl)->members));
    (*cl)->members->seqno = seqno;
    ((*cl)->nmember)++;
    return;
  }
  cm2 = (*cl)->members;

  while(cm2->next != NULL)   
    cm2 = cm2->next;
  
  alloc_cmember(&new);
  new->seqno = seqno;
  cm2->next = new;

  ((*cl)->nmember)++;
  //printf("inserting %d to cluster, nmem: %d\n", seqno, (*cl)->nmember);
}


struct cluster * find_cluster(struct cluster *allclusters, int seqno){
  struct cluster *current;
  struct cluster *prev;
  struct cmember *cm;
  current = allclusters;

  /*
    if (current->nmember == 0)
    return current; // then this is the first guy

    else{ 
  */

  while (current != NULL){
    if (current->nmember != 0){
      cm = current->members;
      while (cm!=NULL){
	if (cm->seqno == seqno){
	  //printf("FOUND SEQNO %d IN CLUSTER NMEMBER %d\n", seqno, current->nmember);
	  return current;
	}
	cm = cm->next;
      }
    }
    prev = current;
    current = current->next;
  }

  // not found, and current is NULL and prev keeps the last one
  // create new cluster, link to prev and return it
  alloc_cluster(&current);
  current->nmember++;
  alloc_cmember(&(current->members));
  current->members->seqno=seqno;
  prev->next = current;
  
  //  }


  return current;

}

void breakname(char *name, char **readname, int *start, int *end){
  int i=0;
  int gaps;
  char dummy[100];
  char fromstr[100];
  char tostr[100];
  char fnumstr[100];
  char tnumstr[100];
  char namestr[100];
  int j;
  
  /*
    while (!isspace(name[i])){
    if (name[i] == ' ')
    break;
    i++;
    }
 
    (*readname) = (char *) malloc((i+1)*sizeof(char));
  */

  /*
  gaps=0;
  j=0;
  
  for (i=0;i<strlen(name);i++){
    if (name[i] != ' '){
      dummy[j++] = name[i];
    }
    else {
      dummy[j]=0; j=0;
      if (gaps == 0)
	strcpy((*readname), dummy);
      else if (gaps == 2) // if gaps==1 dummy=from
	(*start) = atoi(dummy);
      else if (gaps == 4) // if gaps==3 dummy=to
	(*end) = atoi(dummy);
      gaps++;
    }
    } */
  sscanf(name, "%s %s %s %s %s %s", namestr, fromstr, fnumstr, tostr, tnumstr, dummy);
  (*start) = atoi(fnumstr);
  (*end) = atoi(tnumstr);
  strcpy((*readname), namestr);

}

void free_cmember(struct cmember *cm){
  if (cm->next != NULL)
    free_cmember(cm->next);
  free(cm);
}

void free_cluster(struct cluster *cl){
  
  //printf("FREE Cluster with %d members\n", cl->nmember);
  free_cmember(cl->members);  
  free(cl);

}


void alloc_asatseq(struct asatseq **asat){
  (*asat) = (struct asatseq *) malloc (sizeof(struct asatseq));
  (*asat)->next       = NULL;
  (*asat)->start      = 0;
  (*asat)->end        = 0;
  (*asat)->cluster_id = 0;
  (*asat)->isMerged = 0;
}

void alloc_wgsread(struct wgsread **wgs){
  (*wgs) = (struct wgsread *) malloc (sizeof(struct wgsread));
  (*wgs)->members = NULL;
  (*wgs)->amembers = NULL;
  (*wgs)->sibling = NULL;
  (*wgs)->next    = NULL;
  (*wgs)->name[0] = 0;
  (*wgs)->orientation = ORIENT_INITIALIZE;
  (*wgs)->hasBigBrother = FALSE;
  (*wgs)->nsibs = 0;
}


void free_asatseq(struct asatseq *asat){
  if (asat->next != NULL)
    free_asatseq(asat->next);
  free(asat);
}

void free_wgsread(struct wgsread *wgs){
  
  free_asatseq(wgs->members);  
  free(wgs);

}


void add_sibling(struct wgsread **wgs, struct wgsread *sibling){
  struct wgsread *sib2;
  struct asatseq *asat1;
  struct asatseq *asat2;
  int includes1, includes2;
  int flag;
  
  (*wgs)->nsibs++;

  if ((*wgs)->sibling == NULL){
    (*wgs)->sibling = sibling;
    (*wgs)->sibling->sibling = NULL;
    return;
  }

  sib2 = (*wgs)->sibling;

  while(sib2->sibling != NULL)   
    sib2 = sib2->sibling;
  
  sibling->hasBigBrother = TRUE;
  sib2->sibling = sibling;
  sib2->sibling->sibling = NULL;

 
  //printf("inserting %d to cluster, nmem: %d\n", seqno, (*cl)->nmember);
}


void merge_sibling(struct wgsread **wgs, struct wgsread *sibling){
  struct wgsread *sib1;
  struct wgsread *sib2;
  int isInserted;

  (*wgs)->nsibs++;

  if ((*wgs)->sibling == NULL){
    (*wgs)->sibling = sibling;
    (*wgs)->members = sibling->members;
    (*wgs)->amembers = sibling->amembers;
    (*wgs)->sibling->sibling = NULL;
    return;
  }

  sib2 = sibling;

  while(sib2 != NULL){
    sib1 = (*wgs)->sibling;
    while (sib1->sibling != NULL){
      if (!strcmp(sib1->name, sib2->name))
	break;
      sib1 = sib1->sibling;
    }
    if (strcmp(sib1->name, sib2->name)){
      alloc_wgsread(&(sib1->sibling));
      strcpy(sib1->sibling->name, sib2->name);
      sib1->sibling->hasBigBrother = TRUE;
      sib1->sibling->sibling = NULL;
      sib1->sibling->members = sib2->members;
      sib1->sibling->amembers = sib2->amembers;
      sib1->sibling->next = NULL;
      sib1->sibling->orientation = sib2->orientation;
      //      sib1->sibling = sib2;
    }
    sib2 = sib2->sibling;
  }

  

}


void insert_asat(struct wgsread **wgsroot, char *name, int start, int end, int cluster_id){

  struct asatseq *asat;
  struct asatseq *new;
  struct asatseq *prev;
  struct wgsread *wgs;
  

  
  wgs = (*wgsroot);

  if (wgs == NULL){
    printf("WHAT THE HELL?\n");
    exit(0);
  }
  
  while (strcmp(wgs->name, name)){
    if (wgs->next == NULL){
      alloc_wgsread(&(wgs->next));
      strcpy(wgs->next->name, name);
      wgs->next->hasBigBrother = FALSE;
    }
    wgs = wgs->next;
  }

  if (wgs->orientation == INVALID)
    return;


  if (wgs->members == NULL){
    alloc_asatseq(&(wgs->members));
    strcpy(wgs->name, name);
    wgs->members->start = start;
    wgs->members->end = end;
    wgs->members->cluster_id = cluster_id;
    if (start < end) // this is the first entry, so it should be safe
      wgs->orientation = FORWARD;
    else
      wgs->orientation = REVERSE;
    return;
  }

  //fprintf(stderr, "name:%s orientation: %c start: %d end: %d\n", wgs->name, wgs->orientation==FORWARD?'F':'R', start, end);
  
  if (wgs->orientation == REVERSE){
    if (start < end){
      fprintf(stderr, "invalidating %s\n", wgs->name);
      wgs->orientation = INVALID;
      return;      
    }
  }
  
  else if (wgs->orientation == REVERSE){
    if (start > end){
      fprintf(stderr, "invalidating %s\n", wgs->name);
      wgs->orientation = INVALID;
      return;      
    }
  }
  


  asat = wgs->members;
  prev = asat;
  
  if (wgs->orientation == FORWARD){
    while(asat != NULL && start > asat->start){   
      prev = asat;
      asat = asat->next;
    }
  }
  else{
    while(asat != NULL && start < asat->start){   
      prev = asat;
      asat = asat->next;
    }
  }

  if (asat!=NULL && asat->start == start && asat->end == end && asat->cluster_id != cluster_id){
    // already inserted; duplication needed somewhere (for
    // experimental purposes 
    

    /* start duplication */


    /* end duplication */

    return;
  }

  alloc_asatseq(&new);
  new->start = start;
  new->end = end;
  new->cluster_id = cluster_id;
  
  /* insert new in front of asat */  
  new->next = asat;
 
  if (prev != asat)
    prev->next = new;

  else if (asat == wgs->members)
    wgs->members = new;

  /*
  if (prev == wgs->members)
    prev->next = new;
  else
    wgs->members = new;
  */

  //printf("inserting %d to cluster, nmem: %d\n", seqno, (*cl)->nmember);
}


int checkPattern(struct asatseq **as1, struct asatseq **as2, FILE *log){

  struct asatseq *asat1, *asat2, *prev;

  /*
    asat1 = wgs1->members;
    asat2 = wgs2->members;
  */
  asat1 = *as1;
  asat2 = *as2;
  prev = asat1;

  while (asat1 != NULL){
    if (checkPattern2(asat1, asat2)){
      /* as1 precedes as2 */
      merge_asat(as1, as2, log);
      return TRUE;
    }
    prev = asat1;
    asat1 = asat1->next;
  }
  
  asat1 = *as1;
  asat2 = *as2;

  while (asat2 != NULL){
    if (checkPattern2(asat2, asat1)){
      /* as2 precedes as1 */
      merge_asat(as2, as1, log);
      return TRUE;
    }
    asat2 = asat2->next;
  }

  return FALSE;

}

int checkPattern2(struct asatseq *as1, struct asatseq *as2){
  /* 
     this is for exact matches
     i don't know why it a "little" different 
  */

  struct asatseq *asat1, *asat2;

  /*
    asat1 = wgs1->members;
    asat2 = wgs2->members;
  */
  asat1 = as1;
  asat2 = as2;

  
  while (asat1 != NULL && asat2 != NULL){
    if (asat1->cluster_id != asat2->cluster_id)
      return FALSE;
    asat1 = asat1->next;
    asat2 = asat2->next;
    if (asat1 != NULL && asat2 == NULL)
      return FALSE;
  }

  return TRUE;
  
}

int checkPattern3(struct asatseq *as1, struct asatseq *as2){
  /* 
     this is for exact matches
  */

  struct asatseq *asat1, *asat2;

  /*
    asat1 = wgs1->members;
    asat2 = wgs2->members;
  */
  asat1 = as1;
  asat2 = as2;

  
  while (asat1 != NULL && asat2 != NULL){
    if (asat1->cluster_id != asat2->cluster_id)
      return FALSE;
    asat1 = asat1->next;
    asat2 = asat2->next;
    if (asat1 != NULL && asat2 == NULL)
      return FALSE;
    if (asat1 == NULL && asat2 != NULL)
      return FALSE;
  }

  return TRUE;
  
}

int checkPattern4(struct asatseq *as1, struct asatseq *as2){
  /* 
     this is for 
     near matches; grouping
  */

  struct asatseq *asat1, *asat2;

  /*
    asat1 = wgs1->members;
    asat2 = wgs2->members;
  */

  asat1 = as1;
  asat2 = as2;

  
  while (asat1 != NULL){
    asat2 = as2;
    while (asat2 != NULL){
      if (asat1->cluster_id == asat2->cluster_id)
	return TRUE;
      asat2 = asat2->next;
    }
    asat1 = asat1->next;
  }

  return FALSE;
  
}


void print_clusterid(struct asatseq *asatroot, FILE *log){

  struct asatseq *asat;
  //asat = wgs->amembers;
  asat = asatroot;

  while (asat != NULL){
    fprintf (log, " %5d ", asat->cluster_id);
    asat = asat->next;
  }
  
  fprintf(log, "\n");
}


void copy_members(struct wgsread **wgsroot){
  struct wgsread *wgs;
  struct asatseq *asat1;
  struct asatseq *new;
  struct asatseq *asat2;


  wgs = (*wgsroot);
  
  asat1 = wgs->members;

  
  while (asat1 != NULL){

    if (wgs->amembers == NULL){
      alloc_asatseq(&(wgs->amembers));
      wgs->amembers->start = asat1->start;
      wgs->amembers->end = asat1->end;
      wgs->amembers->cluster_id = asat1->cluster_id;
      asat2 = wgs->amembers;
    }
    
    else{
      alloc_asatseq(&new);
      new->start = asat1->start;
      new->end = asat1->end;
      new->cluster_id = asat1->cluster_id;
      new->next = NULL;
      asat2->next = new;
      asat2 = new;
    }

    asat1 = asat1->next;

  }

}



void copy_asat(struct asatseq **as1, struct asatseq *as2){
  /* as1 destination, as2 source */
  /* copy as2 into as1 */

  struct asatseq *asat1;
  struct asatseq *new;
  struct asatseq *asat2;


  asat2 = as2;

  asat1 = NULL;
  
  while (asat2 != NULL){

    if (asat1 == NULL){
      alloc_asatseq(&asat1);
      asat1->start = asat2->start;
      asat1->end = asat2->end;
      asat1->cluster_id = asat2->cluster_id;
      asat1->next = NULL;
    }
    
    else{
      alloc_asatseq(&new);
      new->start = asat2->start;
      new->end = asat2->end;
      new->cluster_id = asat2->cluster_id;
      new->next = NULL;
      asat1->next = new;
      asat1 = new;
    }

    asat2 = asat2->next;

  }

  *as1 = asat1;

}

void merge_asat(struct asatseq **as1, struct asatseq **as2, FILE *log){
  /* as1 destination, as2 source */
  /* copy as2 into as1 */

  struct asatseq *asat1;
  struct asatseq *new;
  struct asatseq *asat2;
  struct asatseq *nnode;
  struct asatseq *temp;

  asat1 = *as1;
  asat2 = *as2;
  new   = NULL;
  
  /*
    fprintf(log, "Merging\t");
    print_clusterid(asat1, log);
    fprintf(log, "\t");
    print_clusterid(asat2, log);
  */

  //while (asat1->cluster_id != asat2->cluster_id){
  while (!checkPattern2(asat1, asat2)){
    if (new == NULL){
      alloc_asatseq(&new);
      new->start = asat1->start;
      new->end = asat1->end;
      new->cluster_id = asat1->cluster_id;
      new->next = NULL;
    }
    else{
      alloc_asatseq(&nnode);
      nnode->start = asat1->start;
      nnode->end = asat1->end;
      nnode->cluster_id = asat1->cluster_id;
      nnode->next = NULL;
      temp = new;
      while (temp->next != NULL)
	temp=temp->next;
      temp->next = nnode;
    }
    asat1 = asat1->next;
  }

 
  while (asat2 != NULL){
    if (new == NULL){
      alloc_asatseq(&new);
      new->start = asat1->start;
      new->end = asat1->end;
      new->cluster_id = asat1->cluster_id;
      new->next = NULL;
    }
    else{
      alloc_asatseq(&nnode);
      nnode->start = asat2->start;
      nnode->end = asat2->end;
      nnode->cluster_id = asat2->cluster_id;
      nnode->next = NULL;
      temp = new;
      while (temp->next != NULL)
	temp=temp->next;
      temp->next = nnode;
    }
    asat2 = asat2->next;
  }

  *as1 = new;
  *as2 = new;

}


/* alignment */

float sig(char s, char t){ // character similarity score
  if (toupper(s) == toupper(t) && s!='-')
    return 5;
  else
    return -4;
} // G

float max3(float a, float b, float c){
  if (a>=b){
    if (a>=c)
      return a;
    else
      return c;
  }
  else{
    if (b>=c)
      return b;
    else
      return c;
  } 
} // max3

float max(float a, float b){
  if (a>b)
    return a;
  return b;
} // max

void align(char *S, char *T){
  int sSize = strlen(S);
  int tSize = strlen(T);
  int i,j,k;
  int ii, jj;
  int larger=sSize;
  float Wg, Ws; // gap penalty , space penalty
  //char *Sp, *Tp; // Sprime, Tprime
  if (tSize > larger)
    larger = tSize;
  Wg=16; Ws=4;

  
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      G[i][j] = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
      if (V[i][j] == G[i][j])
	P[i][j] = 2; // match S[i]-T[1][j]
      else if (V[i][j] == E[i][j])
	P[i][j] = 0; // match T[j]-"-"
      else
	P[i][j] = 1; // match S[i]-"-"
    }
  


  /* vectors ready, set, go! */

  

  Tp[0]=0;
  Sp[0]=0;
  ii=0; jj=0;
  i = sSize; j = tSize;
  while (!(i==0 && j==0)){
    if (P[i][j] == 2){ // diagonal: match S[i]-T[j] 
      //while (i>0 && j>0 && V[i][j] == G[i][j]){ 
      Tp[jj++] = T[--j];
      Sp[ii++] = S[--i];
	//}
    } // if
    else if (P[i][j] == 1){ // toUp: match S[i]-"-"
      //while (i>0 && j>0 && V[i][j] == F[i][j]){
	Tp[jj++] = '-';
	Sp[ii++] = S[--i];      
	//     }
    } // else if   
    else { // toLeft: match T[j]-"-"
      //while (i>0 && j>0 && V[i][j] == E[i][j]){
	Tp[jj++] = T[--j];
	Sp[ii++] = '-';
	//}
    } // else
    if (i==0){
      for (k=0;k<j;k++){
	Sp[ii++] = '-';
      }
      for (k=j;k>0;k--){
	Tp[jj++] = T[k-1];
      }
      break;
    } // if i==0
    if (j==0){
      for (k=0;k<i;k++){
	Tp[jj++] = '-';
      }
      for (k=i;k>0;k--){
	Sp[ii++] = S[k-1];
      }
      break;
    } // if j==0
    //ii++; jj++;
  } // while
  Sp[ii] = 0;
  Tp[jj] = 0;

} // align

void makevectors(void){
  int Wg=12;
  int Ws=2;
  int i,j;
  
  /* create vectors */
  V = (float **)malloc(sizeof(float *)*(ALPHASATLEN+1));
  F = (float **)malloc(sizeof(float *)*(ALPHASATLEN+1));
  E = (float **)malloc(sizeof(float *)*(ALPHASATLEN+1));
  G = (float **)malloc(sizeof(float *)*(ALPHASATLEN+1));
  P = (int **)malloc(sizeof(int *)*(ALPHASATLEN+1));

  for (i=0;i<=ALPHASATLEN;i++){
    V[i]=(float *)malloc(sizeof(float)*(ALPHASATLEN+1));    
    F[i]=(float *)malloc(sizeof(float)*(ALPHASATLEN+1));
    E[i]=(float *)malloc(sizeof(float)*(ALPHASATLEN+1));
    G[i]=(float *)malloc(sizeof(float)*(ALPHASATLEN+1));
    P[i]=(int *)malloc(sizeof(int)*(ALPHASATLEN+1));
  } // for


  
  /* initialize vectors */
  V[0][0] = 0;
  P[0][0] = 0;
  for (i=1;i<=ALPHASATLEN;i++){
    V[i][0] = 0 - Wg - i*Ws;
    //V[i][0] = 0;
    E[i][0] = -1000000; /* -infinity */
    P[i][0] = 1;
  }
  for (j=1;j<=ALPHASATLEN;j++){
    V[0][j] = 0 - Wg - j*Ws;
    //V[0][j] = 0;
    F[0][j] = -1000000; /* -infinity */
    P[0][j] = 0;
  }

  Tp = (char *)malloc(ALPHASATLEN*2);
  Sp = (char *)malloc(ALPHASATLEN*2);
    
}
