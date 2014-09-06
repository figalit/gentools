#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50000000

typedef struct read{
  char *name;
  char *r1, *r2, *barcode;
  char *q1, *q2;
  char *bname;
}_read;


struct read **allreads;

char **allbarcodes;
char **allnames;

static int compare(const void *, const void *);
int dist(char *, char *);

int main(int argc, char **argv){
  int i, j;
  FILE *f1, *f2, *fb;
  
  FILE *o1, *o2;

  char fn1[1000], fn2[1000], fnb[1000];
  char on1[1000], on2[1000];
  char oprefix[1000];

  char name[1000], barcode[1000], r1[1000], r2[1000], q1[1000], q2[1000], qb[1000];
  char skip[1000]; char prev[1000];
  char *ch;
  
  char bcodelist[1000];

  FILE *bl;

  int num_pairs;
  
  char bname[100], bcode[100];
  int bcodecnt;

  int mindist;
  int minbar;
  int thisdist;
  int GZ = 0;

  fn1[0]=0;  fn2[0]=0;  fnb[0]=0;
  oprefix[0]=0;
  bcodelist[0]=0;
  

  for (i=1;i<argc;i++){
    
    if (!strcmp(argv[i], "-f1"))
      strcpy(fn1, argv[i+1]);
    else if (!strcmp(argv[i], "-f2"))
      strcpy(fn2, argv[i+1]);
    else if (!strcmp(argv[i], "-fb"))
      strcpy(fnb, argv[i+1]);
    else if (!strcmp(argv[i], "-bl"))
      strcpy(bcodelist, argv[i+1]);
    else if (!strcmp(argv[i], "-bl"))
      GZ = 1;
    //    if (!strcmp(argv[i], "-o"))
    // strcpy(oprefix, argv[i+1]);
  }

  if (fn1[0]==0 ||fn2[0]==0 ||fnb[0]==0){ //|| oprefix1[0]==0){
    //fprintf(stderr, "At least one of -f1, -f2, -fb or -o is missing.\n");
    fprintf(stderr, "At least one of -f1, -f2, -fb, or -bl is missing.\n");
    return 0;
  }

  if (!GZ){
    f1 = fopen(fn1, "r");
    f2 = fopen(fn2, "r");
    fb = fopen(fnb, "r");
  }
  else{
    f1 = gzopen(fn1, "r");
    f2 = gzopen(fn2, "r");
    fb = gzopen(fnb, "r");
  }

  bl = fopen(bcodelist, "r");

  if (f1==NULL || f2==NULL || fb==NULL || bl==NULL){
    fprintf(stderr, "At least one of f1, f2, fb, or bl files is missing.\n");
    return 0;    
  }


  bcodecnt = 0;
  while (fscanf(bl, "%s %s\n", bname, bcode) > 0) bcodecnt++;

  allbarcodes = (char **) malloc(sizeof(char *) * bcodecnt);
  allnames = (char **) malloc(sizeof(char *) * bcodecnt);

  rewind (bl);

  i=0;
  while (fscanf(bl, "%s %s\n", bname, bcode) > 0){
    allbarcodes[i] = (char *) malloc(sizeof(char) * (strlen(bcode)+1));
    allnames[i] = (char *) malloc(sizeof(char) * (strlen(bname)+1));
    strcpy(allbarcodes[i], bcode);
    strcpy(allnames[i], bname);
    i++;
  }
  fclose(bl);


  allreads = (struct read **) malloc(sizeof(struct read *) * MAX);

  i = 0;

  //  while (1){

  if (!GZ){

    while (fscanf(f1, "%s\n", name) > 0){
      /*
	fgets(name, 1000, f1);
	if (feof(f1)) break;
      */
      
      ch = strrchr(name, '/'); *ch=0;
      
      fscanf(f1, "%s\n", r1);
      fscanf(f1, "%s\n", skip);
      fscanf(f1, "%s\n", q1);
      
      fscanf(f2, "%s\n", skip);
      fscanf(f2, "%s\n", r2);
      fscanf(f2, "%s\n", skip);
      fscanf(f2, "%s\n", q2);
      
      fscanf(fb, "%s\n", skip);
      fscanf(fb, "%s\n", barcode);
      fscanf(fb, "%s\n", skip);
      fscanf(fb, "%s\n", qb);
      
      /*
	fgets(r1, 1000, f1);
	r1[strlen(r1)-1] = 0;
	fgets(skip, 1000, f1);
	fgets(q1, 1000, f1);
	q1[strlen(q1)-1] = 0;
	
	fgets(skip, 1000, f2);
	fgets(r2, 1000, f2);
	r2[strlen(r2)-1] = 0;
	fgets(skip, 1000, f2);
	fgets(q2, 1000, f2);
	q2[strlen(q2)-1] = 0;
	
	fgets(skip, 1000, fb);
	fgets(barcode, 1000, fb);
	barcode[strlen(barcode)-1] = 0;
	fgets(skip, 1000, fb);
	fgets(qb, 1000, fb);
	qb[strlen(qb)-1] = 0;
      */
      
      allreads[i] = (struct read *) malloc(sizeof(struct read));
      
      allreads[i]->r1 = (char *) malloc(sizeof(char) * (strlen(r1)+1));
      allreads[i]->r2 = (char *) malloc(sizeof(char) * (strlen(r2)+1));
      allreads[i]->q1 = (char *) malloc(sizeof(char) * (strlen(q1)+1));
      allreads[i]->q2 = (char *) malloc(sizeof(char) * (strlen(q2)+1));
      allreads[i]->barcode = (char *) malloc(sizeof(char) * (strlen(barcode)+1));
      allreads[i]->name = (char *) malloc(sizeof(char) * (strlen(name)+1));
      
      
      strcpy(allreads[i]->r1, r1);
      strcpy(allreads[i]->r2, r2);
      strcpy(allreads[i]->q1, q1);
      strcpy(allreads[i]->q2, q2);
      strcpy(allreads[i]->barcode, barcode);
      strcpy(allreads[i]->name, name);
      
      mindist = 10000;
      minbar = -1;
      
      for (j=0;j<bcodecnt;j++){
	
	thisdist = dist(allbarcodes[j], barcode);
	
	if (thisdist < mindist){
	  mindist = thisdist;
	  minbar = j;
	}
	
      }
      
      if (mindist <= 2){
	allreads[i]->bname = (char *) malloc(sizeof(char) * (strlen(allnames[minbar])+1));
	strcpy(allreads[i]->bname, allnames[minbar]);
      }
      else{
	allreads[i]->bname = (char *) malloc(sizeof(char) * (strlen(allnames[minbar])+1));
	strcpy(allreads[i]->bname, "dump");
      }
      
      i++;
      
    }
    
  } // !gz
  
  else{

    while (fscanf(f1, "%s\n", name) > 0){

      /*
	fgets(name, 1000, f1);
	if (feof(f1)) break;
      */
      
      ch = strrchr(name, '/'); *ch=0;
      
      fscanf(f1, "%s\n", r1);
      fscanf(f1, "%s\n", skip);
      fscanf(f1, "%s\n", q1);
      
      fscanf(f2, "%s\n", skip);
      fscanf(f2, "%s\n", r2);
      fscanf(f2, "%s\n", skip);
      fscanf(f2, "%s\n", q2);
      
      fscanf(fb, "%s\n", skip);
      fscanf(fb, "%s\n", barcode);
      fscanf(fb, "%s\n", skip);
      fscanf(fb, "%s\n", qb);
      
      /*
	fgets(r1, 1000, f1);
	r1[strlen(r1)-1] = 0;
	fgets(skip, 1000, f1);
	fgets(q1, 1000, f1);
	q1[strlen(q1)-1] = 0;
	
	fgets(skip, 1000, f2);
	fgets(r2, 1000, f2);
	r2[strlen(r2)-1] = 0;
	fgets(skip, 1000, f2);
	fgets(q2, 1000, f2);
	q2[strlen(q2)-1] = 0;
	
	fgets(skip, 1000, fb);
	fgets(barcode, 1000, fb);
	barcode[strlen(barcode)-1] = 0;
	fgets(skip, 1000, fb);
	fgets(qb, 1000, fb);
	qb[strlen(qb)-1] = 0;
      */
      
      allreads[i] = (struct read *) malloc(sizeof(struct read));
      
      allreads[i]->r1 = (char *) malloc(sizeof(char) * (strlen(r1)+1));
      allreads[i]->r2 = (char *) malloc(sizeof(char) * (strlen(r2)+1));
      allreads[i]->q1 = (char *) malloc(sizeof(char) * (strlen(q1)+1));
      allreads[i]->q2 = (char *) malloc(sizeof(char) * (strlen(q2)+1));
      allreads[i]->barcode = (char *) malloc(sizeof(char) * (strlen(barcode)+1));
      allreads[i]->name = (char *) malloc(sizeof(char) * (strlen(name)+1));
      
      
      strcpy(allreads[i]->r1, r1);
      strcpy(allreads[i]->r2, r2);
      strcpy(allreads[i]->q1, q1);
      strcpy(allreads[i]->q2, q2);
      strcpy(allreads[i]->barcode, barcode);
      strcpy(allreads[i]->name, name);
      
      mindist = 10000;
      minbar = -1;
      
      for (j=0;j<bcodecnt;j++){
	
	thisdist = dist(allbarcodes[j], barcode);
	
	if (thisdist < mindist){
	  mindist = thisdist;
	  minbar = j;
	}
	
      }
      
      if (mindist <= 2){
	allreads[i]->bname = (char *) malloc(sizeof(char) * (strlen(allnames[minbar])+1));
	strcpy(allreads[i]->bname, allnames[minbar]);
      }
      else{
	allreads[i]->bname = (char *) malloc(sizeof(char) * (strlen(allnames[minbar])+1));
	strcpy(allreads[i]->bname, "dump");
      }
      
      i++;
      
    }

  }


  num_pairs = i;

  printf("Loaded %d read pairs.\nSorting... ", num_pairs);

  qsort(allreads, num_pairs, sizeof (struct read *), compare);
  printf(" done.\n Outputting ... ");
  fflush(stdout);


  prev[0]=0;
  o1=NULL; o2=NULL;

  for (i=0;i<num_pairs; i++){

    //if (strcmp(prev, allreads[i]->barcode)){
    if (strcmp(prev, allreads[i]->bname)){
      if (o1 != NULL) fclose(o1);
      if (o2 != NULL) fclose(o2);

      //      sprintf(on1, "%s_1.fastq", allreads[i]->barcode);
      //sprintf(on2, "%s_2.fastq", allreads[i]->barcode);
      sprintf(on1, "%s_1.fastq", allreads[i]->bname);
      sprintf(on2, "%s_2.fastq", allreads[i]->bname);

      o1 = fopen(on1, "w");
      o2 = fopen(on2, "w");

      if (o1 == NULL || o2 == NULL){
	fprintf(stderr, "Cannot create output files for some reason. Check directory permissions.\n");
	exit(-1);
      }
      //strcpy(prev, allreads[i]->barcode);
      strcpy(prev, allreads[i]->bname);
      
    }

    fprintf(o1, "%s/1\n%s\n+\n%s\n", allreads[i]->name, allreads[i]->r1, allreads[i]->q1);
    fprintf(o2, "%s/2\n%s\n+\n%s\n", allreads[i]->name, allreads[i]->r2, allreads[i]->q2);
    
  }

  printf(" done.\n");
}



static int compare(const void *p1, const void *p2){
  /* compare function to sort the read pointer array */
  struct read *a, *b;

  a = *((struct read **)p1);
  b = *((struct read **)p2);


  //return (strcmp (a->barcode, b->barcode) );
  return (strcmp (a->bname, b->bname) );

}


int dist(char *s, char *t){
  int len = strlen(s);
  int dist;
  int rdist;
  int i;

  dist = 0;

  for (i=0;i<len;i++)
    if (s[i]!=t[i]) dist++;

  rdist = 0;

  for (i=0;i<len;i++){
    if (s[i]=='A' && t[len-i-1]!='T') rdist++;
    else if (s[i]=='C' && t[len-i-1]!='G') rdist++;
    else if (s[i]=='G' && t[len-i-1]!='C') rdist++;
    else if (s[i]=='T' && t[len-i-1]!='A') rdist++;
  }

  if (dist < rdist) return dist;
  else return rdist;

}
