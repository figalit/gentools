#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define MAX 250000000

char gen[MAX];
void rcomp(char *, char *);

void insert_mm(char *seq, int error);

#define ALL_BITS     0xffffffffL
#define MSB          0x80000000L
#define BITS         32
#define STEP         7
#define HALF_RANGE   0x40000000L

static unsigned int r250_buffer[ 250 ];
static int r250_index;

float ranf(void);

float box_muller(float m, float s);

int main(int argc, char **argv){
  int len=0;
  char fname[500];
  int nseq=0;
  int i,j, k;
  FILE *in;
  int rnum;
  char seq[500];
  char rseq[500];
  char ch; char line[500];
  int seqlen;
  char seqname[50];
  int readpos;
  int readpos2;
  int rc;
  int error;
  int fragsize;
  float stdev;
  char swap[500];
  unsigned int mask, msb;

  
  fname[0]=0;
  fragsize = 0;
  stdev = 0;

  if (argc==1){
    printf("fastq simulator. Generates a number of reads of given length, introduces mismatches with 1%% probability. Kind of...\n\n");
    printf("\t-i [fasta_file]: Read this fasta file to simulate the reads from.\n");
    printf("\t-l [length]    : Read length.\n");
    printf("\t-e [error]    : Number of errors to insert.\n");    
    printf("\t-n [nseq]      : Number of sequences to generate.\n\n");
    printf("\t-f [nseq]      : Average fragment size.\n\n");
    printf("\t-n [nseq]      : Standard deviation of the fragment size.\n\n");
    return 0;
  }



  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      error = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-n"))
      nseq = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-f"))
      fragsize = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-d"))
      stdev = atof(argv[i+1]);
  }
    
  if (fragsize<=0){
    fprintf(stderr, "Fragsize should be a positive number. Use -f parameter.\n");
    return -1;
  }

  r250_index = 0;

  fprintf(stderr, "Fragsize: %d\tStdev: %f\n", fragsize, stdev);

  for (j = 0; j < 250; j++)      /* fill r250 buffer with BITS-1 bit values */
    r250_buffer[j] = (unsigned int)lrand48();


  for (j = 0; j < 250; j++)/* set some MSBs to 1 */
    if ( rand() > HALF_RANGE )
      r250_buffer[j] |= MSB;
  
  msb = MSB;        /* turn on diagonal bit */
  mask = ALL_BITS;/* turn off the leftmost bits */

  for (j = 0; j < BITS; j++)
    {
      k = STEP * j + 3;/* select a word to operate on */
      r250_buffer[k] &= mask; /* turn off bits left of the diagonal */
      r250_buffer[k] |= msb;/* turn on the diagonal bit */
      mask >>= 1;
      msb  >>= 1;
    }



  in = fopen(fname, "r");

  fscanf(in, ">%s", seqname);
  fgets(line, 100, in);

  i=0;
  while (fscanf(in, "%c", &ch) > 0){
    if (!isspace(ch)) gen[i++]=toupper(ch);
  }
  gen[i]=0;
  seqlen = i;
  
  srand(time(NULL));
  srand48(time(NULL));

  i=0;
  while (i<nseq){
    readpos = rand() % (seqlen - len - 1);
    /* first read */
    memcpy(seq, gen+readpos, len);
    seq[len]=0;
    /* end of first read */
    /* second read */
    readpos2 = readpos + (int)box_muller((float)fragsize, stdev) - len;
    //readpos2 = (int)box_muller((float)fragsize, stdev);
    
    //printf ("%d\n", readpos2);

    if (readpos2+len >= seqlen) continue;

    memcpy(swap, gen+readpos2, len);
    swap[len]=0;
    rcomp(swap, rseq);
    /* end of second read */


    rc = rand() % 2;
    if (rc == 1){
      strcpy(swap, seq);
      strcpy(seq, rseq);
      strcpy(rseq, swap);
    }



    if (strchr(seq, 'N') == NULL && strchr(rseq, 'N')==NULL){
      rnum = rand() % 100;
      if (rnum == 0)
	insert_mm(seq, error);
      
      rnum = rand() % 100;
      if (rnum == 0)
	insert_mm(rseq, error);
      
      //      printf("@read_%s_%d_%c\n%s\n+\n", seqname,  (readpos+1), ( (rc==0) ? '+' : '-') ,  (rc==0) ? seq : rseq);

      printf("@read_%s_%d/1\n%s\n+\n", seqname,  (readpos+1), seq);
      for (j=0;j<len;j++){
	rnum = rand() % 30 + 10  + 33;
	printf("%c", rnum);
      }
      printf("\n");
      
      printf("@read_%s_%d/2\n%s\n+\n", seqname,  (readpos+1), rseq);
      for (j=0;j<len;j++){
	rnum = rand() % 30 + 10  + 33;
	printf("%c", rnum);
      }
      printf("\n");
      

      i++;
    }
  }
}


void rcomp(char *seq, char *rseq){
  int i;
  int len = strlen(seq);

  for (i=0;i<len;i++){
    switch (seq[i]){
    case 'A':
      rseq[len-i-1] = 'T';
      break;
    case 'C':
      rseq[len-i-1] = 'G';
      break;
    case 'G':
      rseq[len-i-1] = 'C';
      break;
    case 'T':
      rseq[len-i-1] = 'A';
      break;
    default:
      rseq[len-i-1] = seq[i];
      break;
    }
  }
  rseq[len]=0;
}

void insert_mm(char *seq, int error){
  int len = strlen (seq);
  int nmm = rand() % error + 1;
  int i;
  int pos;
  int mmchar;
  
  for (i=0;i<nmm;i++){
    pos = rand() % len;
    mmchar = rand() % 4;
    switch (mmchar){
    case 0:
      seq[pos] = 'A';
      break;
    case 1:
      seq[pos] = 'C';
      break;
    case 2:
      seq[pos] = 'G';
      break;
    case 3:
      seq[pos] = 'T';
      break;
    default:
      break;
    }
  } 
}


float ranf()/* returns a random double in range 0..1 */
{

  
  register int j;
  register unsigned int new_rand;
 
  if ( r250_index >= 147 )
    j = r250_index - 147;// wrap pointer around 
  else
    j = r250_index + 103;

  new_rand = r250_buffer[ r250_index ] ^ r250_buffer[ j ];
  r250_buffer[ r250_index ] = new_rand;

  if ( r250_index >= 249 )// increment pointer for next time 
    r250_index = 0;
  else
    r250_index++;

  return (float)new_rand / ALL_BITS;
  

  //  return (float)rand() / RAND_MAX;
}

float box_muller(float m, float s)/* normal random variate generator */
{        /* mean m, standard deviation s */
  float x1, x2, w, y1;
  static float y2;
  static int use_last = 0;

  if (use_last)        /* use value from previous call */
    {
      y1 = y2;
      use_last = 0;
    }
  else
    {
      do {
	//fprintf(stderr, "loop %f %f\n", x1, y1);
	x1 = 2.0 * ranf() - 1.0;
	x2 = 2.0 * ranf() - 1.0;
	w = x1 * x1 + x2 * x2;
      } while ( w >= 1.0 );

      w = sqrt( (-2.0 * log( w ) ) / w );
      y1 = x1 * w;
      y2 = x2 * w;
      use_last = 1;
    }

  return( m + y1 * s );
}
