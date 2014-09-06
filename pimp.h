#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define SEQ_LENGTH 150

#define MISMATCH 1000

#ifdef CLUSTERMONOMERS
#define CLUSTERCUTOFF 4
#endif
#ifdef HORPATTERN
#define CLUSTERCUTOFF 0
#endif

#define PATTSET_MIN 10
#define ALPHASATLEN 400
#define NAMELENGTH 50

#define TRUE 1
#define FALSE 0

typedef int ORIENTATION;
#define ORIENT_INITIALIZE 3
#define INVALID 2
#define FORWARD 1
#define REVERSE 0

void trim(char *);
int my_strcmp(char *, char *, int);
int my_strcmpAlign(char *, char *, int);
int my_strcmpHamming(char *, char *, int);
int lemming;
char *aligned[2];

/* pattern match stuff */
typedef struct cmember{
  int seqno; // sequence number from names and seqs
  struct cmember *next;
}scmember;

typedef struct cluster{
  int nmember; // no of members
  struct cmember *members;
  struct cluster *next;
  int id;
}scluster;

typedef struct asatseq{
  int cluster_id;
  int start;
  int end;
  int isMerged;
  struct asatseq *next;
}sasatseq;

typedef struct wgsread{
  struct asatseq *members;
  struct wgsread *next;
  struct wgsread *sibling;
  struct asatseq *amembers; // same with members but allows merging
  int hasBigBrother;
  ORIENTATION orientation;
  char name[NAMELENGTH];
  int nsibs;
}swgsread;


void alloc_cmember(struct cmember **);
void alloc_cluster(struct cluster**);
void insert_cmember(struct cluster **, int);
struct cluster * find_cluster(struct cluster *, int);
void breakname(char *, char **, int *, int *);
void free_cmember(struct cmember *);
void free_cluster(struct cluster *);

void alloc_asatseq(struct asatseq **);
void alloc_wgsread(struct wgsread **);
void free_asatseq(struct asatseq *asat);
void free_wgsread(struct wgsread *wgs);
void insert_asat(struct wgsread **, char *, int, int, int);
int checkPattern(struct asatseq **, struct asatseq **, FILE *);
int checkPattern2(struct asatseq *, struct asatseq *);
int checkPattern3(struct asatseq *, struct asatseq *);
int checkPattern4(struct asatseq *, struct asatseq *);


void print_clusterid(struct asatseq *, FILE *);
void add_sibling(struct wgsread **, struct wgsread *);
void merge_sibling(struct wgsread **, struct wgsread *);
void copy_members(struct wgsread **);
void copy_asat(struct asatseq **, struct asatseq *);
void merge_asat(struct asatseq **, struct asatseq **, FILE *);


/*alignment stuff */
float **V, **F, **E, **G; // tables
int **P;
char *Sp;
char *Tp;

void align(char *, char *);
float max(float, float);
float max3(float, float, float);
float sig(char, char);
void makevectors(void);
