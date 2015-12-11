#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MEMUSE 2147483648
#define MEMSCALE 1.5
#define STRLEN 256

void  fastq_fetch(char *, char *);
static int qname_comp(const void *, const void *);

void set_str( char** target, char* source)
{
  if( *target != NULL)
    {
      free( ( *target));
    }
  
  ( *target) = ( char*) malloc( sizeof( char) * ( strlen( source) + 1));
  strncpy( ( *target), source, ( strlen( source) + 1));
}
 
int main(int argc, char **argv){

  char name[2048];

  if (argc != 3) return;

  fastq_fetch(argv[1], argv[2]);

}


void  fastq_fetch(char *filename1, char *filename2){

  char *res;
  char **names;
  int num_seq;
  char tmp_name[2048];
  char tmp_line[2048];
  /* filename2 is a list of read names */

  char *qname = (char *) malloc(sizeof(char)*2048);
  char qseq[2048];
  char qqual[2048];

  int num_matched;
  int i;
  int j;

  FILE *f1, *f2;
  FILE *of1; 

  int loaded;

  char *ofilename1;
  

  num_seq = 0;
  f2 = fopen(filename2, "r");

  if (f2==NULL){
    fprintf(stderr, "Cannot open file %s for reading.\n", filename2);
    exit(-1);
  }

  while (fscanf(f2, "%s", tmp_name) > 0){
    fgets(tmp_name, 2048, f2);
    num_seq++;
  }
  rewind(f2);

  names = (char **) malloc (sizeof (char *) * num_seq);
  i = 0;

  while (fscanf(f2, "%s", tmp_name) > 0){
    fgets(tmp_line, 2048, f2);
    set_str(&(names[i++]), tmp_name);
  }  
  fclose(f2);

  for (i=0; i<10; i++) printf("%s\n", names[i]);

  f1 = fopen(filename1, "r");
 
  if (f1==NULL){
    fprintf(stderr, "Cannot open file %s for reading.\n", filename1);
    exit(-1);
  }

  qsort(names, num_seq, sizeof(char *), qname_comp); 

  fprintf(stderr, "-------------\n");
  for (i=0; i<10; i++) printf("%s\n", names[i]);

  ofilename1 = (char *) malloc (sizeof(char) * (strlen(filename1)+strlen(filename2)+10));
  
  sprintf(ofilename1, "%s_fetched_%s.fastq", filename1, filename2);
  
  of1 = fopen(ofilename1, "w");

  while (fscanf(f1, "@%s", qname) > 0){
    fgets(tmp_line, 2048, f1);
    
    fscanf(f1, "%s\n+\n%s\n", qseq, qqual);
    res =  (char *) bsearch(&qname, names, num_seq, sizeof(char *), qname_comp);
    if (res != NULL){
      fprintf(of1, "@%s\n%s\n+\n%s\n", qname, qseq, qqual);
      num_matched++;
    }
  }

  fclose(of1);  
  fclose(f1);  

}

static int qname_comp(const void *p1, const void *p2){
  const char **a, **b;
  int i;
  int l1, l2;

  a =  (const char **) p1;
  b =  (const char **) p2;
  
  return strcmp(*a, *b);
}

