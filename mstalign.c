/*
  experimental MST phylogeny
  reads consensus sequences from a fasta file
  builds a graph where edge weight is the edit distance, 
  then extracts Minimum Spanning Tree via Prim
*/

#include "fastatools.h"

#define MATRIXFILE "mstmatrix.txt"

#define FALSE 0
#define TRUE 1
#define NONE 100000


struct node{
  char name[SEQ_LENGTH];
  struct node **children;
  struct node *parent;
  int whoami;
  int nkids;
};


float pairwise(char *, char *);
int sig(char, char);
int max3(int, int, int);
int max(int, int);
float getmin(int, int *, int *);
float getmin_prim(int, int *, int *);
float getmin_prim_single(int, int, int *);
struct node *find_this(struct node *, int);
int dnaindex(char);
void fill_scorematrix(void);


float **tmat;
int **mark;
int *inthetree;
int verbose=FALSE;

struct node **graph;

int scorematrix[17][17];


int main(int argc, char **argv){

  FILE *fasta;
  FILE *tree;
  FILE *tmatfile;
  char treefname[100];

  int nseq;
  int i, j;
  int dummy;
  float score;
  struct node *G;
  struct node *current;
  float min;
  int thisguy;
  int first, second;
  int source;
  int insert;
  int nkids;
  float max_sf;
  int rootindex;

  if (argc != 2 && argc!=3){
    fprintf(stderr, "experimental MST phylogeny\nreads consensus sequences from a fasta file\nbuilds a graph where edge weight is the edit distance,\nthen extracts Minimum Spanning Tree via Prim.\n");
    fprintf(stderr, "Usage: %s [fastafile]\n", argv[0]);
    return 0;
  }

  if (argv[1][0]=='-'){
    if (!strcmp(argv[1], "-v"))
      verbose=TRUE;
    fasta = fopen(argv[2], "r");
    strcpy(treefname, argv[2]);
  }
  else{
    fasta = fopen(argv[1], "r");
    strcpy(treefname, argv[1]);
  }  
    
  for (i=strlen(treefname)-1;i>=0;i--){
    if (treefname[i] == '.')
      treefname[i]=0;
  }

  if (fasta == NULL){
    fprintf(stderr, "Unable to open file %s\n", argv[1]);
    return 0;
  }

  nseq = readSingleFasta(fasta);

  fill_scorematrix();
  
  for (i=0;i<nseq;i++){
    if (strstr(names[i], "-")){
      dummy = 0;
      for (j=0;j<strlen(names[i]);j++){
	if (names[i][j] == '-'){
	  dummy++;
	  if (dummy == 2){
	    names[i][j] = 0;
	    break;
	  }
	}
      }
      j=0;
      while (names[i][j++] != '-')
	;
      names[i]+=j;
    }
  }
  
  tmat = (float **)malloc(sizeof(float *) * nseq);
  for (i=0;i<nseq;i++)
    tmat[i] = (float *)malloc(sizeof(float) * (i+1));
 
  mark = (int **)malloc(sizeof(int *) * nseq);
  for (i=0;i<nseq;i++)
    mark[i] = (int *)malloc(sizeof(int) * (i+1));

  inthetree = (int *)malloc(sizeof(int) * nseq);
  
  /*
  graph = (struct node **) malloc(sizeof (struct node *) * nseq);
  
  for (i=0;i<nseq;i++){
    graph[i] = (struct node *) malloc(sizeof (struct node));
    strcpy(graph[i]->name, names[i]);
    graph[i]->children = (struct node **) malloc(sizeof (struct node *) * nseq);
    for (j=0;j<nseq;j++){
      graph[i]->children[j] = (struct node *) malloc(sizeof (struct node) * nseq);
      graph[i]->children[j]->nkids = 0;
    }
    graph[i]->whoami = i;
    graph[i]->parent = NULL;
  }
  fprintf(stderr, "\n");
  */

  /*

  graph = (struct node **) malloc(sizeof (struct node *) * nseq);
  
  for (i=0;i<nseq;i++){
    graph[i] = (struct node *) malloc(sizeof (struct node) * nseq);
  }

  for (i=0;i<nseq;i++)
    graph[i] = NULL;

  */


  max_sf = -1000000;
  rootindex = -1;  


  for (i=0;i<nseq;i++){
    inthetree[i] = FALSE;
    for (j=0;j<i;j++){
      tmat[i][j] = pairwise(seqs[i], seqs[j]);///(strlen(seqs[i])+strlen(seqs[j]));
      mark[i][j] = FALSE;
      if (verbose)
	fprintf(stderr, "Sequences [%d,%d] aligned: %f\n", i, j, tmat[i][j]); 
    }
  }

  for (i=0;i<nseq;i++){
    score = 0.0;
    for (j=0;j<nseq;j++){
      if (i>j)
	score+=tmat[i][j];
      else if (i<j)
	score+=tmat[j][i];
    }
    if (score > max_sf){
      max_sf = score;
      rootindex = i;
    }
  }

  
  //srand(time(NULL));
  //rootindex = rand()%nseq;

  if (verbose)
    printf("ROOT: %d: %s\n", rootindex, names[rootindex]);
  
  G = (struct node *) malloc(sizeof (struct node) * nseq);
  strcpy(G->name, names[rootindex]);
  G->children = (struct node **) malloc(sizeof (struct node *) * nseq);
  for (j=0;j<nseq;j++){
    G->children[j] = (struct node *) malloc(sizeof (struct node) * nseq);
  }
  G->whoami = rootindex;
  G->nkids = 0;
  inthetree[rootindex] = TRUE;
  
  if (verbose)  
    fprintf(stderr, "\n");
  
  //graph[0] = G;

  /* control experiment

  nseq = 7;
  
  for (i=0;i<nseq;i++)
    for (j=0;j<i;j++)
      tmat[i][j]=0.0;

  tmat[1][0] = 7;
  tmat[2][0] = NONE;
  tmat[2][1] = 8;
  tmat[3][0] = 5;
  tmat[3][1] = 9;
  tmat[3][2] = NONE;
  tmat[4][0] = NONE;
  tmat[4][1] = 7;
  tmat[4][2] = 5;
  tmat[4][3] = 15;
  tmat[5][0] = NONE;
  tmat[5][1] = NONE;
  tmat[5][2] = NONE;
  tmat[5][3] = 6;
  tmat[5][4] = 8;
  tmat[6][0] = NONE;
  tmat[6][1] = NONE;
  tmat[6][2] = NONE;
  tmat[6][3] = NONE;
  tmat[6][4] = 9;
  tmat[6][5] = 11;

  strcpy(names[0], "A");
  strcpy(names[1], "B");
  strcpy(names[2], "C");
  strcpy(names[3], "D");
  strcpy(names[4], "E");
  strcpy(names[5], "F");
  strcpy(names[6], "G");

  rootindex = 0;
  
  end of control experiment
  */

  inthetree[rootindex] = TRUE;

  strcat(treefname, ".viz");
	 
  tree = fopen(treefname, "w");

  fprintf(tree, "digraph G {\n\tsize=\"80,110\";\n");

  if (strstr(names[rootindex], "D19Z") 
      || strstr(names[rootindex], "D21Z3")
      || strstr(names[rootindex], "D21Z4")
      || strstr(names[rootindex], "D21Z5")
      || strstr(names[rootindex], "D21Z7")
      || strstr(names[rootindex], "D5Z12"))
    fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=orange, fontsize=6];\n", rootindex, names[rootindex] );  
  else if (strstr(names[rootindex], "WMA") ||  strstr(names[rootindex], "EEB"))
    fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=green, fontsize=6];\n", rootindex, names[rootindex] );  
    else if (strstr(names[rootindex], "AC026005"))
      fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=blue, fontsize=6];\n", insert, names[insert] ); 
  else if (strstr(names[rootindex], "D"))
    fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=red, fontsize=6];\n", rootindex, names[rootindex] );  
  else
    fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=grey, fontsize=6];\n", rootindex, names[rootindex] );  



  while(1){
    //min = getmin(nseq, &first, &second);
    min = getmin_prim(nseq, &first, &second);
    if (verbose)
      printf("Main  first: %d second: %d min: %f\n", first, second, min);
    
    if (min == NONE)
      break;
    
    /*
      if (!inthetree[first]){
      insert = first;
      source = second;
      }
      else{*/
    
    insert = second;
    source = first;
    
    //}
    
    /*
      
    if (graph[source] == NULL){
    fprintf(stderr, "Something is wrong with graph\n");
      return 0;
    } 

    current = graph[source];
    nkids = current->nkids++;
    current = current->children[nkids];

    strcpy(current->name, names[insert]);
    current->whoami = insert;

    current->children = (struct node **) malloc(sizeof (struct node *) * nseq);
    for (j=0;i<nseq;j++){
      current->children[j] = (struct node *) malloc(sizeof (struct node) * nseq);
      current->children[j]->nkids = 0;
    }

    graph[insert] = current;


    */
    if (verbose)
      printf("insert: %d, source: %d\n", first,second,insert,source);

    
    if (strstr(names[insert], "D19Z") 
	|| strstr(names[insert], "D21Z3")
	|| strstr(names[insert], "D21Z4")
	|| strstr(names[insert], "D21Z5")
	|| strstr(names[insert], "D21Z7")
	|| strstr(names[insert], "D5Z12"))
      fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=orange, fontsize=6];\n", insert, names[insert] );
    else if (strstr(names[insert], "WMA") ||  strstr(names[insert], "EEB"))
    fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=green, fontsize=6];\n", insert, names[insert] );  
    else if (strstr(names[insert], "AC026005"))
      fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=blue, fontsize=6];\n", insert, names[insert] );  
    else if (strstr(names[insert], "D"))
      fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=red, fontsize=6];\n", insert, names[insert] );  
    else
      fprintf(tree, "\t%d [shape=circle, label=\"%s\", style=filled, height=0.3, width=0.3, color=grey, fontsize=6];\n", insert, names[insert] );
    fprintf(tree, "\t%d -> %d;\n", source, insert);



    /* 
       insert this to the tree
     */

    /*

    current = find_this(G, first);

    for (j=0;j<nseq;j++){
      current->children[nkids]->children[j] = (struct node *) malloc(sizeof (struct node) * nseq);
    }
    
    strcpy(current->children[nkids]->name, names[second]);
    current->children[nkids]->nkids=0;
    current->children[nkids]->whoami=second;
    
    current->nkids++;
    */


    /*
      if (graph[source]->parent == NULL)
    current = graph[source];
    else
    current = graph[source]->parent;    
    
    
    nkids  = current->nkids++;
    graph[insert]->parent = current;
    current->children[nkids] = graph[insert];

    */
    inthetree[first] = TRUE;
    inthetree[second] = TRUE;
    
    
  }

  fprintf(tree, "}\n");

  tmatfile = fopen(MATRIXFILE, "w");
    
  for (i=0;i<nseq;i++){
    for (j=0;j<i;j++){
      fprintf(tmatfile, "[\t%d\t%d]: %f\n", i,j,tmat[i][j]);
    }
  }
  /*
  for (i=0;i<nseq;i++){
    fprintf(tree, "NODE: %d\n---------------\n", i);
    for (j=0;j<graph[i]->nkids;j++){
      if (graph[i]->children[j]->nkids >= 0)
	fprintf(tree, "KID: %s, nkids: %d\n", graph[i]->children[j]->name, graph[i]->children[j]->nkids);
    }
    fprintf(tree,"\n\n");
  }
  */

  fclose(tmatfile); fclose(tree);

  return 1;

}


float pairwise(char *S, char *T){
  int tSize, sSize; // textSize, sourceSize
  int Wg=16, Ws=4; // gap penalty , space penalty
  int **V, **F, **E, **G, **P; // tables
  //int V[MAX+1][MAX+1], G[MAX+1][MAX+1], E[MAX+1][MAX+1], F[MAX+1][MAX+1];
  int i, j;
  float ret;
  tSize = strlen(T);
  sSize = strlen(S);
  /* create vectors */
  V = (int **)malloc(sizeof(int *)*(sSize+1));
  F = (int **)malloc(sizeof(int *)*(sSize+1));
  E = (int **)malloc(sizeof(int *)*(sSize+1));
  G = (int **)malloc(sizeof(int *)*(sSize+1));
  P = (int **)malloc(sizeof(int *)*(sSize+1));
  for (i=0;i<=sSize;i++){
    V[i]=(int *)malloc(sizeof(int)*(tSize+1));    
    F[i]=(int *)malloc(sizeof(int)*(tSize+1));
    E[i]=(int *)malloc(sizeof(int)*(tSize+1));
    G[i]=(int *)malloc(sizeof(int)*(tSize+1));
    P[i]=(int *)malloc(sizeof(int)*(tSize+1));
  } // for 
  /* initialize vectors */
  V[0][0] = 0;
  P[0][0] = 0;
  E[0][0] = -10000;  
  G[0][0] = -10000;
  F[0][0] = -10000;
  for (i=1;i<=sSize;i++){
    V[i][0] = -Wg - i*Ws;
    E[i][0] = -10000; /* -infinity */
    P[i][0] = 1;
  }
  for (j=1;j<=tSize;j++){
    V[0][j] = -Wg - j*Ws;
    F[0][j] = -10000; /* -infinity */
    P[0][j] = 0;
  }
  for (i=1;i<=sSize;i++)
    for (j=1;j<=tSize;j++){
      G[i][j] = V[i-1][j-1] + sig(S[i-1], T[j-1]);
      F[i][j] = max((F[i-1][j]-Ws), (V[i-1][j]-Wg-Ws));
      E[i][j] = max((E[i][j-1]-Ws), (V[i][j-1]-Wg-Ws));
      V[i][j] = max3(G[i][j], F[i][j], E[i][j]);
      if (V[i][j] == G[i][j])
        P[i][j] = 2; // match S[i]-T[j]
      else if (V[i][j] == E[i][j])
        P[i][j] = 0; // match T[j]-"-"
      else
        P[i][j] = 1; // match S[i]-"-"
    }

  ret = V[sSize][tSize];
  for (i=0;i<=sSize;i++){  
    free(V[i]);
    free(F[i]);
    free(E[i]);
    free(G[i]);
    free(P[i]);
  } // for
  free(V);
  free(G);
  free(E);
  free(F); 
  free(P); 
  return -1*ret;
} // pairwise



int sig(char s, char t){ // character similarity score

  /*
  if (toupper(s) == toupper(t)){ 
    if (s!='-')
      return 10;
    else
      return 0;
  } // if
  else
    return -10;
  */

  return scorematrix[dnaindex(toupper(s))][dnaindex(toupper(t))];

} // sig


int max(int a, int b){
  if (a>b)
    return a;
  return b;
}


int max3(int a, int b, int c){
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


float getmin(int nseq, int *first, int *second){
  int i,j;
  int ii,jj;
  float min = NONE;
  for (i=0; i<nseq; i++){
    for (j=0; j<i; j++){
      if (i>j && mark[i][j]==FALSE && tmat[i][j]<min && (inthetree[j] ^ inthetree[i])){
	min = tmat[i][j];
	ii=i; jj=j;
      }
    }  
  }
  
  if (min != NONE){
    mark[ii][jj] = TRUE;
    
    *first = ii;
    *second = jj;
  }
  
  return min;

}

float getmin_prim(int nseq, int *first, int *second){
  int i,i_f, i_s;
  int isthis;
  float min=NONE;
  float score;

  for (i=0;i<nseq;i++){
    if (inthetree[i]){
      score = getmin_prim_single(nseq, i, &isthis);
      if (score < min){
	min = score;
	i_f = i; i_s = isthis;
      }
    }
  }

  if (min != NONE){
    *first = i_f;
    *second = i_s;
    if (verbose)
      printf("getmin_prim: f: %d s: %d min: %f\n", i_f, i_s, min);
    if (i_f>i_s)
      mark[i_f][i_s] = 0;
    else
      mark[i_s][i_f] = 0;
  }

  return min;

}


float getmin_prim_single(int nseq, int first, int *second){
  int i,j;
  int ii,jj;
  float min = NONE;

  
  for (i=0; i<nseq; i++){
    if (i>first){
      if (!inthetree[i] && mark[i][first]==FALSE && tmat[i][first]<min){
	min = tmat[i][first];
	ii=i; 
      }
    }
    else if (i<first){
      if (!inthetree[i] && mark[first][i]==FALSE && tmat[first][i]<min){
	min = tmat[first][i];
	ii=i; 
      }
    }
  }


  /*
  for (i=0; i<nseq; i++){
    for (j=0; j<i; j++){
      if (i>j && mark[i][j]==FALSE && tmat[i][j]<min && (inthetree[j] ^ inthetree[i])){
	min = tmat[i][j];
	ii=i; jj=j;
      }
    }  
  }
  
  */

  /*
  if (min != NONE){
    if (ii>first)
      mark[ii][first] = TRUE;
    else
      mark[first][ii] = TRUE;
    //*first = ii;
    *second = ii;
  }
  */

  if (min != NONE){
    if (verbose)
      printf("getmin_prim_single: f: %d s: %d min: %f\n", first, ii, min);
    *second = ii;
  }

  return min;

}


struct node *find_this(struct node *root, int whoami){
  
  struct node *current = root;
  int i;

  if (root == NULL)
    return NULL;


  while (current!=NULL && current->whoami != whoami){
    
    for (i=0;i<current->nkids;i++){
      if (current->children[i]->whoami == whoami)
	return current->children[i];
      if (current->children[i]->nkids != 0)
	current = find_this(current->children[i], whoami);
    }
    
  }

  return current;

}


int dnaindex(char ch){
  switch (ch){
  case 'A':
    return 0;
    break;
  
  case 'B':
    return 1;
    break;

  case 'C':
    return 2;
    break;
  
  case 'D':
    return 3;
    break;
  
  case 'G':
    return 4;
    break;
  
  case 'H':
    return 5;
    break;
  
  case 'K':
    return 6;
    break;
  
  case 'M':
    return 7;
    break;
  
  case 'N':
    return 8;
    break;
  
  case 'R':
    return 9;
    break;
  
  case 'S':
    return 10;
    break;
  
  case 'T':
    return 11;
    break;
  
  case 'U':
    return 12;
    break;
  
  case 'V':
    return 13;
    break;
  
  case 'W':
    return 14;
    break;
  
  case 'X':
    return 15;
    break;
  
  case 'Y':
    return 16;
    break;  
    
  }
}


void fill_scorematrix(void){
  int i, j;

  for (i=0;i<17;i++)
    for (j=0;j<17;j++)
      scorematrix[i][j] = 10;

  scorematrix[1][0] = scorematrix[2][0] = scorematrix[3][2] = scorematrix[4][0] = scorematrix[4][2] = 
    scorematrix[5][4] = scorematrix[6][0] = scorematrix[6][2] = scorematrix[7][4] = scorematrix[7][6] = 
    scorematrix[9][2] = scorematrix[10][0] = scorematrix[11][0] = scorematrix[11][2] = scorematrix[11][4] = 
    scorematrix[11][7] = scorematrix[11][9] = scorematrix[11][10] = scorematrix[12][0] = scorematrix[12][2] = 
    scorematrix[12][4] = scorematrix[12][7] = scorematrix[12][9] = scorematrix[12][10] = scorematrix[13][11] = 
    scorematrix[13][12] = scorematrix[14][2] = scorematrix[14][4] = scorematrix[14][10] = scorematrix[16][0] =
    scorematrix[16][4] = scorematrix[16][9] = -9;
  
  for (i=0;i<17;i++)
    for (j=0;j<i;j++)
      scorematrix[j][i] = scorematrix[i][j];


}
