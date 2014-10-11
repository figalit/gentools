/* Change int h to int gh everywhere  DES June 1994 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "multpair.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define gap(k)  ((k) <= 0 ? 0 : g + gh * (k))
#define tbgap(k)  ((k) <= 0 ? 0 : tb + gh * (k))
#define tegap(k)  ((k) <= 0 ? 0 : te + gh * (k))

/*
 *	Prototypes
 */
static void add(sint v);
static sint calc_score(sint iat, sint jat, sint v1, sint v2);
static float tracepath(sint tsb1,sint tsb2);
static float tracepath2(sint tsb1,sint tsb2);//, int seq1, int seq2);
static void forward_pass(char *ia, char *ib, sint n, sint m);
static void reverse_pass(char *ia, char *ib);
static sint diff(sint A, sint B, sint M, sint N, sint tb, sint te);
static void del(sint k);

/*
 *   Global variables
 */
#ifdef MAC
#define pwint   short
#else
#define pwint   int
#endif
static sint		int_scale;

//extern float    **tmat;
extern float    pw_go_penalty;
extern float    pw_ge_penalty;
extern float	transition_weight;
extern sint 	nseqs;
extern sint 	max_aa;
extern sint 	gap_pos1,gap_pos2;
extern sint  	max_aln_length;
extern sint 	*seqlen_array;
extern sint 	debug;
extern sint  	mat_avscore;
extern short 	blosum30mt[],pam350mt[],idmat[],pw_usermat[],pw_userdnamat[];
extern short    clustalvdnamt[],swgapdnamt[];
extern short    gon250mt[];
extern short 	def_dna_xref[],def_aa_xref[],pw_dna_xref[],pw_aa_xref[];
extern Boolean  dnaflag;
extern char 	**seq_array;
extern char 	**names;
extern char 	*amino_acid_codes;
extern char 	pw_mtrxname[];
extern char 	pw_dnamtrxname[];

static float 	mm_score;
static sint 	print_ptr,last_print;
static sint 	*displ;
static pwint 	*HH, *DD, *RR, *SS;
static sint 	g, gh;
static sint   	seq1, seq2;
static sint     matrix[NUMRES][NUMRES];
static pwint    maxscore;
static sint    	sb1, sb2, se1, se2;
float tmatval;

FILE *logfile;  // calkan - logfile for distances
int loglines; // calkan - to split up huge logs
void log_pairs(char [], char []); // calkan - logging pair alignments
void plot_histogram(void); // calkan plot histogram

sint pairalign(sint istart, sint iend, sint jstart, sint jend)
{
  short	 *mat_xref;
  static sint    si, sj, i;
  static sint    n,m,len1,len2;
  static sint    maxres;
  static short    *matptr;
  static char   c;
  static float gscale,ghscale;
  char new_logfile[100];
  int lognumber = 1;
  int ii, jj;
  displ = (sint *)ckalloc((2*max_aln_length+1) * sizeof(sint));
  HH = (pwint *)ckalloc((max_aln_length) * sizeof(pwint));
  DD = (pwint *)ckalloc((max_aln_length) * sizeof(pwint));
  RR = (pwint *)ckalloc((max_aln_length) * sizeof(pwint));
  SS = (pwint *)ckalloc((max_aln_length) * sizeof(pwint));
	
  /* calkan - initialize logfile */
  logfile = fopen(logfilename, "w");
  loglines = 0;
  if (logfile == NULL){
    printf("Error opening logfile !\n");
    exit(0);
  }
	
  //  fprintf(logfile, "FILE	BEGIN	END	indel_N	indel_S	base_S	base_Match	base_Mis	transversions	transitions	per_sim	SE_sim	per_sim_indel	SE_sim_indel	K_jc	SE_jc	k_kimura	SE_kimura	K_tn	SE_tn	d_lake	AA	AT	AC	AG	AN	TA	TT	TC	TG	TN	CA	CT	CC	CG	CN	GA	GT	GC	GG	GN	NA	NT	NC	NG	NN\n");
  /*  fprintf(logfile, "%20s%11s%14s%14s%14s%14s%14s%14s%14s%14s%22s%22s%22s%22s%22s%22s%22s%22s%22s%22s%22s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s\n", 
	  "FILE","BEGIN","END","indel_N","indel_S","base_S", "base_Match","base_Mis",
	  "transversions","transitions","per_sim","SE_sim","per_sim_indel","SE_sim_indel",
	  "K_jc","SE_jc","k_kimura","SE_kimura","K_tn","SE_tn","d_lake","AA","AT","AC","AG",
	  "AN","TA","TT","TC","TG","TN","CA","CT","CC","CG","CN","GA","GT","GC","GG","GN", 
	  "NA","NT","NC","NG","NN");
  */
  
  fprintf(logfile, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", 
	  "FILE","BEGIN","END","indel_N","indel_S","base_S", "base_Match","base_Mis",
	  "transversions","transitions","per_sim","SE_sim","per_sim_indel","SE_sim_indel",
	  "K_jc","SE_jc","k_kimura","SE_kimura","K_tn","SE_tn","d_lake","AA","AT","AC","AG",
	  "AN","TA","TT","TC","TG","TN","CA","CT","CC","CG","CN","GA","GT","GC","GG","GN", 
	  "NA","NT","NC","NG","NN");

#ifdef MAC
  int_scale = 10;
#else
  int_scale = 100;
#endif

  gscale=ghscale=1.0;
  if (dnaflag)
    {
      if (debug>1) fprintf(stdout,"matrix %s\n",pw_dnamtrxname);
      if (strcmp(pw_dnamtrxname, "iub") == 0)
	{ 
	  matptr = swgapdnamt;
	  mat_xref = def_dna_xref;
	}
      else if (strcmp(pw_dnamtrxname, "clustalw") == 0)
	{ 
	  matptr = clustalvdnamt;
	  mat_xref = def_dna_xref;
	  gscale=0.6667;
	  ghscale=0.751;
	}
      else
	{
	  matptr = pw_userdnamat;
	  mat_xref = pw_dna_xref;
	}
      maxres = get_matrix(matptr, mat_xref, matrix, TRUE, int_scale);
      if (maxres == 0) return((sint)-1);

      matrix[0][4]=transition_weight*matrix[0][0];
      matrix[4][0]=transition_weight*matrix[0][0];
      matrix[2][11]=transition_weight*matrix[0][0];
      matrix[11][2]=transition_weight*matrix[0][0];
      matrix[2][12]=transition_weight*matrix[0][0];
      matrix[12][2]=transition_weight*matrix[0][0];
    }
  else
    {
      if (debug>1) fprintf(stdout,"matrix %s\n",pw_mtrxname);
      if (strcmp(pw_mtrxname, "blosum") == 0)
	{
	  matptr = blosum30mt;
	  mat_xref = def_aa_xref;
	}
      else if (strcmp(pw_mtrxname, "pam") == 0)
	{
	  matptr = pam350mt;
	  mat_xref = def_aa_xref;
	}
      else if (strcmp(pw_mtrxname, "gonnet") == 0)
	{
	  matptr = gon250mt;
	  int_scale /= 10;
	  mat_xref = def_aa_xref;
	}
      else if (strcmp(pw_mtrxname, "id") == 0)
	{
	  matptr = idmat;
	  mat_xref = def_aa_xref;
	}
      else
	{
	  matptr = pw_usermat;
	  mat_xref = pw_aa_xref;
	}

      maxres = get_matrix(matptr, mat_xref, matrix, TRUE, int_scale);
      if (maxres == 0) return((sint)-1);
    }


  for (si=MAX(0,istart);si<nseqs && si<iend;si++)  // MAIN LOOP : ERAY TURKCE COMMENT YAZARIM ROCKY ANLAMASIN
    {
      n = seqlen_array[si+1];
      len1 = 0;
      for (i=1;i<=n;i++) {
	c = seq_array[si+1][i];
	if ((c!=gap_pos1) && (c != gap_pos2)) len1++;
      }

      for (sj=MAX(si+1,jstart+1);sj<nseqs && sj<jend;sj++)
	{
	  m = seqlen_array[sj+1];
	  if(n==0 || m==0) {
	    /*
	      if (si>sj)
	      tmat[si+1][sj+1]=1.0;
	      else
	      tmat[sj+1][si+1]=1.0;
	    */
	    continue;
	  }
	  len2 = 0;
	  for (i=1;i<=m;i++) {
	    c = seq_array[sj+1][i];
	    if ((c!=gap_pos1) && (c != gap_pos2)) len2++;
	  }

	  if (debug>1) fprintf(stdout,"mat_avscore %d %d %d\n",mat_avscore,n,m);
	  if (dnaflag) {
	    g = 2 * (float)pw_go_penalty * int_scale*gscale;
	    gh = pw_ge_penalty * int_scale*ghscale;
	  }
	  else {
	    if (mat_avscore <= 0)
              g = 2 * (float)(pw_go_penalty + log((double)(MIN(n,m))))*int_scale;
	    else
              g = 2 * mat_avscore * (float)(pw_go_penalty +
					    log((double)(MIN(n,m))))*gscale;
	    gh = pw_ge_penalty * int_scale;
	  }

	  if (debug>1) fprintf(stdout,"go %d ge %d\n",(pint)g,(pint)gh);

	  /*
	    align the sequences
	  */
	  seq1 = si+1;
	  seq2 = sj+1;

	  forward_pass(&seq_array[seq1][0], &seq_array[seq2][0],
		       n, m);

	  reverse_pass(&seq_array[seq1][0], &seq_array[seq2][0]);

	  last_print = 0;
	  print_ptr = 1;
	  /*
	    sb1 = sb2 = 1;
	    se1 = n-1;
	    se2 = m-1;
	  */

	  /* use Myers and Miller to align two sequences */

	  maxscore = diff(sb1-1, sb2-1, se1-sb1+1, se2-sb2+1, 
			  (sint)0, (sint)0);
 
	  /* calculate percentage residue identity */

	  //mm_score = tracepath2(sb1,sb2,seq1,seq2);
	  mm_score = tracepath2(sb1,sb2);   //, seq1, seq2);
	  if (debug>0) fprintf(stdout,"%f %d %d\n",mm_score,len1,len2);
	  if(len1==0 || len2==0) mm_score=0;
	  else
	    mm_score /= (float)MIN(len1,len2);
	  /*
	    if (si>sj)
	    tmat[si+1][sj+1] = ((float)100.0 - mm_score)/(float)100.0;
	    else
	    tmat[sj+1][si+1] = ((float)100.0 - mm_score)/(float)100.0;
	  */
	  if (debug>1)
	    {
	      fprintf(stdout,"Sequences (%d:%d) Aligned. Score: %d CompScore:  %d\n",
		      (pint)si+1,(pint)sj+1, 
		      (pint)mm_score, 
		      (pint)maxscore/(MIN(len1,len2)*100));
	    }
	  else
	    {
	      info("Sequences (%d:%d) Aligned. Score:  %d",
		   (pint)si+1,(pint)sj+1, 
		   (pint)mm_score);
	      loglines++;
	      if (loglines == 1000000){
		fclose(logfile);
		lognumber++;
		sprintf(new_logfile, "%s.%d", logfilename, lognumber);
	  	logfile = fopen(new_logfile, "w");
  		loglines = 0;
  		if (logfile == NULL){
    		  printf("Error opening logfile !\n");
    		  exit(0);
  		}
	
		fprintf(logfile, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", 
			"FILE","BEGIN","END","indel_N","indel_S","base_S", "base_Match","base_Mis",
			"transversions","transitions","per_sim","SE_sim","per_sim_indel","SE_sim_indel",
			"K_jc","SE_jc","k_kimura","SE_kimura","K_tn","SE_tn","d_lake","AA","AT","AC","AG",
			"AN","TA","TT","TC","TG","TN","CA","CT","CC","CG","CN","GA","GT","GC","GG","GN", 
			"NA","NT","NC","NG","NN");
		
                /*fprintf(logfile, 
		 "%20s%11s%14s%14s%14s%14s%14s%14s%14s%14s%22s%22s%22s%22s%22s%22s%22s%22s%22s%22s%22s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s%7s\n", 
	  	 "FILE","BEGIN","END","indel_N","indel_S","base_S", "base_Match","base_Mis",
	  	"transversions","transitions","per_sim","SE_sim","per_sim_indel","SE_sim_indel",
	  	"K_jc","SE_jc","k_kimura","SE_kimura","K_tn","SE_tn","d_lake","AA","AT","AC","AG",
	  	"AN","TA","TT","TC","TG","TN","CA","CT","CC","CG","CN","GA","GT","GC","GG","GN", 
	  	"NA","NT","NC","NG","NN");*/
	      }  // open another logfile
	    }


	}
    }
  displ=ckfree((void *)displ);
  HH=ckfree((void *)HH);
  DD=ckfree((void *)DD);
  RR=ckfree((void *)RR);
  SS=ckfree((void *)SS);
  /*
    printf("\n");
    for (ii=1;ii<=nseqs;ii++){
    for (jj=1;jj<=nseqs;jj++)
    printf("%f ",tmat[ii][jj]);
    printf("\n");
    }
  */      

  /* end of story, close logfile, plot histogram */
  fclose(logfile);
  plot_histogram();

  return((sint)1);
}

static void add(sint v)
{

  if(last_print<0) {
    displ[print_ptr-1] = v;
    displ[print_ptr++] = last_print;
  }
  else
    last_print = displ[print_ptr++] = v;
}

static sint calc_score(sint iat,sint jat,sint v1,sint v2)
{
  sint ipos,jpos;
  sint ret;

  ipos = v1 + iat;
  jpos = v2 + jat;

  ret=matrix[(int)seq_array[seq1][ipos]][(int)seq_array[seq2][jpos]];

  return(ret);
}


static float tracepath2(sint tsb1,sint tsb2){
  char c1,c2;
  sint  i1,i2,r;
  sint i,k,pos,to_do;
  sint count;
  float score;
  char s1[seqlen_array[seq1]+seqlen_array[seq2]], s2[seqlen_array[seq1]+seqlen_array[seq2]];
  int nogapcount_s1;
  int nogapcount_s2;
  int gapcount_s1;
  int gapcount_s2;
  char seqnames[1000];
	
  to_do=print_ptr-1;
  i1 = tsb1;
  i2 = tsb2;
  pos = 0;
  count = 0;
  for(i=1;i<=to_do;++i) {

    if(displ[i]==0) {
      c1 = seq_array[seq1][i1];
      c2 = seq_array[seq2][i2];
   
      if (c1>max_aa) 
	s1[pos] = '-';
      else 
	s1[pos]=amino_acid_codes[c1];
      if (c2>max_aa) 
	s2[pos] = '-';
      else 
	s2[pos]=amino_acid_codes[c2];
   
      if ((c1!=gap_pos1) && (c1 != gap_pos2) &&
	  (c1 == c2)) count++;
      ++i1;
      ++i2;
      ++pos;
    }
    else {
      if((k=displ[i])>0) {
     
	for (r=0;r<k;r++)
	  {
	    s1[pos+r]='-';
	    if (seq_array[seq2][i2+r]>max_aa) s2[pos+r] = '-';
	    else s2[pos+r]=amino_acid_codes[seq_array[seq2][i2+r]];
	  }
     
	i2 += k;
	pos += k;
      }
      else {
     
	for (r=0;r<(-k);r++)
	  {
	    s2[pos+r]='-';
	    if (seq_array[seq1][i1+r]>max_aa) 
	      s1[pos+r] = '-';
	    else 
	      s1[pos+r]=amino_acid_codes[seq_array[seq1][i1+r]];
	  }
     
	i1 -= k;
	pos -= k;
      }
    }
  }

  //fprintf(stdout,"\n");

  nogapcount_s1 = 0;
  nogapcount_s2 = 0;
  gapcount_s1 = 0;
  gapcount_s2 = 0;

  for (i=0;i<pos;i++) {
    if (s1[i] != '-')
      nogapcount_s1++;
    else
      gapcount_s1++;
  }
  //fprintf(stdout,"\n");
  for (i=0;i<pos;i++) {
    if (s2[i] != '-')
      nogapcount_s2++;
    else
      gapcount_s2++;
  }
  //fprintf(stdout,"\n");
  
  /* test for end gaps */
  
  if (nogapcount_s1 < seqlen_array[seq1]) { // seq1 needs an end, seq2 needs gap
    i = pos;
    while(nogapcount_s1 < seqlen_array[seq1]){
      s1[i] = amino_acid_codes[seq_array[seq1][i-gapcount_s1]];
      s2[i] = '-';
      i++;
      nogapcount_s1++;
    }
  }
  
  else {
    if (nogapcount_s2 < seqlen_array[seq2]) { // seq2 needs an end, seq1 needs gap
      i = pos ;
      while(nogapcount_s2 < seqlen_array[seq2]){
	s2[i] = amino_acid_codes[seq_array[seq2][i-gapcount_s2]];
	s1[i] = '-';
	i++;
	nogapcount_s2++;
      }
    }
  }

  s1[i] = 0;
  s2[i] = 0;



  /* we are all set log it */
  sprintf(seqnames,"%s-vs-%s", names[seq1],names[seq2]); 
  //fprintf(logfile,"%s.%s",names[seq1],names[seq2]); 
 
  fprintf(logfile,"%s\t", seqnames); 
  log_pairs(s1, s2);        // s1: aligned input sb1;  s2: aligned input sb2
  
  score = 100.0 * (float)count;
  //printf("\n---------\ns1: %s\n", s1);
  //printf("\n---------\ns2: %s\n", s2);

  return(score);
}


static float tracepath(sint tsb1,sint tsb2)
{
  char c1,c2;
  sint  i1,i2,r;
  sint i,k,pos,to_do;
  sint count;
  float score;
  char s1[600], s2[600];

  to_do=print_ptr-1;
  i1 = tsb1;
  i2 = tsb2;

  pos = 0;
  count = 0;
  for(i=1;i<=to_do;++i) {

    if (debug>1) fprintf(stdout,"%d ",(pint)displ[i]);
    if(displ[i]==0) {
      c1 = seq_array[seq1][i1];
      c2 = seq_array[seq2][i2];

      if (debug>0)
	{
	  if (c1>max_aa) s1[pos] = '-';
	  else s1[pos]=amino_acid_codes[c1];
	  if (c2>max_aa) s2[pos] = '-';
	  else s2[pos]=amino_acid_codes[c2];
	}

      if ((c1!=gap_pos1) && (c1 != gap_pos2) &&
	  (c1 == c2)) count++;
      ++i1;
      ++i2;
      ++pos;
    }
    else {
      if((k=displ[i])>0) {

	if (debug>0)
	  for (r=0;r<k;r++)
	    {
	      s1[pos+r]='-';
	      if (seq_array[seq2][i2+r]>max_aa) s2[pos+r] = '-';
	      else s2[pos+r]=amino_acid_codes[seq_array[seq2][i2+r]];
	    }

	i2 += k;
	pos += k;
      }
      else {

	if (debug>0)
	  for (r=0;r<(-k);r++)
	    {
	      s2[pos+r]='-';
	      if (seq_array[seq1][i1+r]>max_aa) s1[pos+r] = '-';
	      else s1[pos+r]=amino_acid_codes[seq_array[seq1][i1+r]];
	    }

	i1 -= k;
	pos -= k;
      }
    }
  }
  if (debug>0) fprintf(stdout,"\n");
  if (debug>0) 
    {
      for (i=0;i<pos;i++) fprintf(stdout,"%c",s1[i]);
      fprintf(stdout,"\n");
      for (i=0;i<pos;i++) fprintf(stdout,"%c",s2[i]);
      fprintf(stdout,"\n");
    }
  /*
    if (count <= 0) count = 1;
  */
  score = 100.0 * (float)count;
  return(score);
}


static void forward_pass(char *ia, char *ib, sint n, sint m)
{

  sint i,j;
  pwint f,hh,p,t;

  maxscore = 0;
  se1 = se2 = 0;
  for (i=0;i<=m;i++)
    {
      HH[i] = 0;
      DD[i] = -g;
    }

  for (i=1;i<=n;i++)
    {
      hh = p = 0;
      f = -g;

      for (j=1;j<=m;j++)
	{

	  f -= gh; 
	  t = hh - g - gh;
	  if (f<t) f = t;

	  DD[j] -= gh;
	  t = HH[j] - g - gh;
	  if (DD[j]<t) DD[j] = t;

	  hh = p + matrix[(int)ia[i]][(int)ib[j]];
	  if (hh<f) hh = f;
	  if (hh<DD[j]) hh = DD[j];
	  if (hh<0) hh = 0;

	  p = HH[j];
	  HH[j] = hh;

	  if (hh > maxscore)
	    {
	      maxscore = hh;
	      se1 = i;
	      se2 = j;
	    }
	}
    }

}


static void reverse_pass(char *ia, char *ib)
{

  sint i,j;
  pwint f,hh,p,t;
  pwint cost;

  cost = 0;
  sb1 = sb2 = 1;
  for (i=se2;i>0;i--)
    {
      HH[i] = -1;
      DD[i] = -1;
    }

  for (i=se1;i>0;i--)
    {
      hh = f = -1;
      if (i == se1) p = 0;
      else p = -1;

      for (j=se2;j>0;j--)
	{

	  f -= gh; 
	  t = hh - g - gh;
	  if (f<t) f = t;

	  DD[j] -= gh;
	  t = HH[j] - g - gh;
	  if (DD[j]<t) DD[j] = t;

	  hh = p + matrix[(int)ia[i]][(int)ib[j]];
	  if (hh<f) hh = f;
	  if (hh<DD[j]) hh = DD[j];

	  p = HH[j];
	  HH[j] = hh;

	  if (hh > cost)
	    {
	      cost = hh;
	      sb1 = i;
	      sb2 = j;
	      if (cost >= maxscore) break;
	    }
	}
      if (cost >= maxscore) break;
    }

}

static int diff(sint A,sint B,sint M,sint N,sint tb,sint te)
{
  sint type;
  sint midi,midj,i,j;
  int midh;
  static pwint f, hh, e, s, t;

  if(N<=0)  {
    if(M>0) {
      del(M);
    }

    return(-(int)tbgap(M));
  }

  if(M<=1) {
    if(M<=0) {
      add(N);
      return(-(int)tbgap(N));
    }

    midh = -(tb+gh) - tegap(N);
    hh = -(te+gh) - tbgap(N);
    if (hh>midh) midh = hh;
    midj = 0;
    for(j=1;j<=N;j++) {
      hh = calc_score(1,j,A,B)
	- tegap(N-j) - tbgap(j-1);
      if(hh>midh) {
	midh = hh;
	midj = j;
      }
    }

    if(midj==0) {
      del(1);
      add(N);
    }
    else {
      if(midj>1)
	add(midj-1);
      displ[print_ptr++] = last_print = 0;
      if(midj<N)
	add(N-midj);
    }
    return midh;
  }

  /* Divide: Find optimum midpoint (midi,midj) of cost midh */

  midi = M / 2;
  HH[0] = 0.0;
  t = -tb;
  for(j=1;j<=N;j++) {
    HH[j] = t = t-gh;
    DD[j] = t-g;
  }

  t = -tb;
  for(i=1;i<=midi;i++) {
    s=HH[0];
    HH[0] = hh = t = t-gh;
    f = t-g;
    for(j=1;j<=N;j++) {
      if ((hh=hh-g-gh) > (f=f-gh)) f=hh;
      if ((hh=HH[j]-g-gh) > (e=DD[j]-gh)) e=hh;
      hh = s + calc_score(i,j,A,B);
      if (f>hh) hh = f;
      if (e>hh) hh = e;

      s = HH[j];
      HH[j] = hh;
      DD[j] = e;
    }
  }

  DD[0]=HH[0];

  RR[N]=0;
  t = -te;
  for(j=N-1;j>=0;j--) {
    RR[j] = t = t-gh;
    SS[j] = t-g;
  }

  t = -te;
  for(i=M-1;i>=midi;i--) {
    s = RR[N];
    RR[N] = hh = t = t-gh;
    f = t-g;

    for(j=N-1;j>=0;j--) {

      if ((hh=hh-g-gh) > (f=f-gh)) f=hh;
      if ((hh=RR[j]-g-gh) > (e=SS[j]-gh)) e=hh;
      hh = s + calc_score(i+1,j+1,A,B);
      if (f>hh) hh = f;
      if (e>hh) hh = e;

      s = RR[j];
      RR[j] = hh;
      SS[j] = e;

    }
  }

  SS[N]=RR[N];

  midh=HH[0]+RR[0];
  midj=0;
  type=1;
  for(j=0;j<=N;j++) {
    hh = HH[j] + RR[j];
    if(hh>=midh)
      if(hh>midh || (HH[j]!=DD[j] && RR[j]==SS[j])) {
	midh=hh;
	midj=j;
      }
  }

  for(j=N;j>=0;j--) {
    hh = DD[j] + SS[j] + g;
    if(hh>midh) {
      midh=hh;
      midj=j;
      type=2;
    }
  }

  /* Conquer recursively around midpoint  */


  if(type==1) {             /* Type 1 gaps  */
    diff(A,B,midi,midj,tb,g);
    diff(A+midi,B+midj,M-midi,N-midj,g,te);
  }
  else {
    diff(A,B,midi-1,midj,tb,0.0);
    del(2);
    diff(A+midi+1,B+midj,M-midi-1,N-midj,0.0,te);
  }

  return midh;       /* Return the score of the best alignment */
}

static void del(sint k)
{
  if(last_print<0)
    last_print = displ[print_ptr-1] -= k;
  else
    last_print = displ[print_ptr++] = -(k);
}



/* 
   calkan
   pyrimidine detection
*/

int pyrimidine(char c){
  if (toupper(c) == 'C' || toupper(c) == 'T')
    return 1;
  return 0;
}

/* 

   calkan - log pairs
   input s1, s2 strings
   uses logfile for file output
   computes a lot of distances on s1-s2 alignment
   outputs it
   also
   fills in the Kimura histogram array

*/

void log_pairs(char s1[], char s2[]){
  /* data for distance computation */
  int indel_number = 0;
  int indel_spaces = 0;
  int base_spaces = 0;
  int samebases = 0; // matches
  int base_mismatches = 0;
  int transversions = 0; // purine-pyrimidine
  int transitions = 0; // pyrimidine-pyrimidine || purine-purine
  int s1a, s1c, s1g, s1t;
  int s2a, s2c, s2g, s2t;
  int ag,ac,at,aa,an;
  int gc,gt, ga, gg,gn;
  int ca,cc,cg,ct,cn;
  int ta,tc,tg,tt,tn;
  int na,nc,ng,nt,nn;

  /* variables for loops */
  int i,j;

  /* variables for distances */
  double percent_sim;
  double SE_percent_sim;
  double percent_sim_windel;
  double SE_percent_sim_windel;
  double p; // temp variable
  double q; // temp variable
  double a; // temp variable
  double b; // temp variable
  double h; // temp variable
  double b1; // temp variable
  double q_t[4]; // temp
  char bases[4] = {'A', 'G', 'T', 'C'}; // temp
  double k_jukes_cantor;
  double SE_k_jukes_cantor;
  double k_kimura;
  double SE_k_kimura;
  double k_tajima_nei;
  double SE_k_tajima_nei;
  double d_lake;
  double SE_d_lake;


  s1a = s1c = s1g = s1t = 0;
  s2a = s2c = s2g = s2t = 0;
  ag = 0; ac = 0; at = 0; aa = 0; an = 0; 
  gc = 0; gt = 0; gg = 0; ga = 0; gn = 0; 
  ca = 0; cc = 0; cg = 0; ct = 0; cn = 0; 
  ta = 0; tc = 0; tg = 0; tt = 0; tn = 0; 
  na = 0; nc = 0; ng = 0; nt = 0; nn = 0; 

  // collect data
  for (i = 0; i<strlen(s1); i++){
    switch (toupper(s1[i])){
    case 'A':
      s1a++;
      if (s2[i] == 'T') at++;
      else if (s2[i] == 'G') ag++;
      else if (s2[i] == 'C') ac++;
      else if (s2[i] == 'A') aa++;
      else if (s2[i] == 'N') an++;
      break;
    case 'C':
      s1c++;
      if (s2[i] == 'T') ct++;
      else if (s2[i] == 'G') cg++;
      else if (s2[i] == 'A') ca++;
      else if (s2[i] == 'C') cc++;
      else if (s2[i] == 'N') cn++;
      break;
    case 'G':
      s1g++;
      if (s2[i] == 'T') gt++;
      else if (s2[i] == 'A') ga++;
      else if (s2[i] == 'C') gc++;
      else if (s2[i] == 'G') gg++;
      else if (s2[i] == 'N') gn++;
      break;
    case 'T':
      s1t++;
      if (s2[i] == 'G') tg++;
      else if (s2[i] == 'A') ta++;
      else if (s2[i] == 'C') tc++;
      else if (s2[i] == 'T') tt++;
      else if (s2[i] == 'N') tn++;
      break;
    case 'N':
      if (s2[i] == 'G') ng++;
      else if (s2[i] == 'A') na++;
      else if (s2[i] == 'C') nc++;
      else if (s2[i] == 'T') nt++;
      else if (s2[i] == 'N') nn++;
      break;
    default:
      break;
    }

    /*
    if (s1[i] != '-' && s1[i] != 'N'){
    switch (toupper(s2[i])){
    case 'A':
      s2a++;
      break;
    case 'C':
      s2c++;
      break;
    case 'G':
      s2g++;
      break;
    case 'T':
      s2t++;
      break;
    default:
      break;
    }
    }
    */
    /*
    if (toupper(s1[i]) != 'N' && toupper(s2[i]) != 'N' && s1[i]!='-' && s2[i] != '-')
      base_spaces++;
    */
    /*
    if (s1[i] != s2[i] && s1[i] != '-' && s2[i] != '-'){
      base_mismatches++;
      if (pyrimidine(s1[i]) ^ pyrimidine(s2[i])) // XOR - 1 if 1-0 or 0-1
	transversions++;
      else
	transitions++;
    }
    else if (s1[i] == s2[i])
      samebases++;
      else */
    if (s1[i] == '-'){
      indel_spaces++;
      if (i == 0)
	indel_number++;
      else if (s1[i-1] != '-')
	indel_number++;
    }
    else if (s2[i] == '-'){
      indel_spaces++;
      if (i == 0)
	indel_number++;
      else if (s2[i-1] != '-')
	indel_number++;
    }
  }
  
  s1a = aa+ag+ac+at;
  s1g = ga+gg+gc+gt;
  s1c = ca+cg+cc+ct;
  s1t = ta+tg+tc+tt;

  s2a = aa+ga+ca+ta;
  s2g = ag+gg+cg+tg;
  s2c = ac+gc+cc+tc;
  s2t = at+gt+ct+tt;

  base_mismatches = ag+ac+at+ga+gc+gt+ca+cg+ct+ta+tg+tc;
  transversions = at+ta+ac+ca+gc+cg+gt+tg;
  transitions = ag+ga+ct+tc;
  samebases = aa+gg+cc+tt;
  base_spaces = aa+ag+ac+at+ga+gc+gg+gt+ta+tc+tt+tg+ca+cc+cg+ct;

  //  printf("indelnumber %d\nindelspaces %d\nbasespaces %d\nbasematches %d\nbasemismatches %d\ntransversions %d\ntransitions %d\n", indel_number, indel_spaces, base_spaces, samebases, base_mismatches, transversions, transitions);

  /* compute distances */
  
  percent_sim = (double)samebases / (double)base_spaces;
  SE_percent_sim = sqrt(((double)samebases/(double)base_spaces) * (1-(double)samebases/(double)base_spaces)/(double)base_spaces);
  //printf("%2.15f -- %2.15f\n", percent_sim, SE_percent_sim);
  
  percent_sim_windel = (double)samebases / ((double)base_spaces + (double)indel_number);
  SE_percent_sim_windel = sqrt(percent_sim_windel * (1-percent_sim_windel) / ((double)(base_spaces+indel_number)));

  //printf("%2.15f -- %2.15f\n", percent_sim_windel, SE_percent_sim_windel);

  p = (double)base_mismatches / (double)base_spaces;
  k_jukes_cantor = -0.75 * log(1-4*p/3);
  SE_k_jukes_cantor = sqrt((1-p)*p/((double)base_spaces * pow((1-4*p/3),2)));

  //printf("%2.15f -- %2.15f\n", k_jukes_cantor, SE_k_jukes_cantor);
  
  p = (double)transitions / (double)base_spaces;
  q = (double)transversions / (double)base_spaces;
  a = 1 / (1 - 2*p - q);
  b = 1 / (1 - 2*q);
  k_kimura = 0.5 * log(a) + 0.25 * log(b);
  SE_k_kimura = sqrt((a*a*p + pow((a+b)/2,2)*q - pow(a*p +(((a+b)/2*q)), 2)) / (double)base_spaces);

  //printf("%2.15f -- %2.15f\n", k_kimura, SE_k_kimura);



  // OK , remaining: tajima_nei & lake
  // read the perl code
  
  q_t[0] = ((double)(s1a+s2a)) / (2 * (double)base_spaces);
  q_t[1] = ((double)(s1g+s2g)) / (2 * (double)base_spaces);
  q_t[2] = ((double)(s1t+s2t)) / (2 * (double)base_spaces);
  q_t[3] = ((double)(s1c+s2c)) / (2 * (double)base_spaces);

  b1 = 1 - q_t[0]*q_t[0] - q_t[1]*q_t[1] - q_t[2]*q_t[2] - q_t[3]*q_t[3];
  //printf("q0 %2.10f\n q1 %2.10f\n q2 %2.10f\n q3 %2.10f\n",q_t[0], q_t[1], q_t[2], q_t[3]);
  h = 0;

  /*
  h += pow( (double)(ag) / (double)base_spaces, 2) / (2 * q_t[0] * q_t[1]);
  h += pow( (double)(at) / (double)base_spaces, 2) / (2 * q_t[0] * q_t[2]);
  h += pow( (double)(ac) / (double)base_spaces, 2) / (2 * q_t[0] * q_t[3]);
  ask if perl code is buggy
  */
  //printf("h: %2.15f\n", h);
  h += pow( ((double)(gt+tg) / (double)base_spaces), 2) / (2 * q_t[0] * q_t[1]);
  //printf("h: %2.15f\n", h);
  h += pow( ((double)(gc+cg) / (double)base_spaces), 2) / (2 * q_t[0] * q_t[2]);
  // printf("h: %2.15f\n", h);
  h += pow( ((double)(ct+tc) / (double)base_spaces), 2) / (2 * q_t[1] * q_t[2]);

  for (i=0;i<3;i++){
    for (j=i+1;j<4;j++){
      if (q_t[i] == 0 || q_t[j] == 0){
	h = 0;
	break;
      } 
    }
  }

  /*
  for (i=0;i<3;i++){
    for (j=i+1;j<4;j++){
      if (q_t[i] == 0 || q_t[j] == 0){
	h = 0;
	break;
      } 
      //      h +=  pow( (double)(bases[i]==bases[j] + bases[j]==bases[i]) / (double)base_spaces, 2) / (2 * q_t[i] * q_t[j]);
      h +=  pow( (double)(bases[i]==bases[j] + bases[j]==bases[i]) / (double)base_spaces, 2) / (2 * q_t[i] * q_t[j]);
    }
  }
  */
  //  printf("h: %2.15f\n", h);
  p = (double) base_mismatches / (double) base_spaces;
  if (h == 0){
    k_tajima_nei = -1000;
    SE_k_tajima_nei = -1000;  
  }
  else{
    b = (b1 + (p*p / h)) / 2;
    k_tajima_nei = -1 * b * log(1 - p/b);
    SE_k_tajima_nei = sqrt(b*b*p*(1-p)/((double)base_spaces*(b-p)*(b-p)));
  }
  //  printf("%2.15f - %2.15f\n", k_tajima_nei, SE_k_tajima_nei);

  h = aa * (tt * (cc*gg - gc*cg) - tc * (ct*gg - gt*cg) + tg * (ct*gc - gt*cc))
    - at * (ta * (cc*gg - gc*cg) - tc * (ca*gg - ga*cg) + tg * (ca*gc - ga*cc))
    + ac * (ta * (ct*gg - gt*cg) - tt * (ca*gg - ga*cg) + tg * (ca*gt - ga*ct))
    - ag * (ta * (ct*gc - gt*cc) - tt * (ca*gc - ga*cc) + tc * (ca*gt - ga*ct));
 
 
  a = (double)s1a * (double)s1c * (double)s1g * (double)s1t ;
  b = (double)s2a * (double)s2c * (double)s2g * (double)s2t ;

  a = s1a * s1c * s1g * s1t ;
  b = s2a * s2c * s2g * s2t ;
  
  if (a*b == 0)
    d_lake = -1000;
  else
    d_lake = -0.25 * (log(h) - 0.5 * log(a*b));
  
  //  printf("j: %f a %f b %f lake %20.15f\n", h,a,b,d_lake);
  
  fprintf(logfile, "%d\t", strlen(s1));
  fprintf(logfile, "%d\t", indel_number);
  fprintf(logfile, "%d\t", indel_spaces);
  fprintf(logfile, "%d\t", base_spaces);
  fprintf(logfile, "%d\t", samebases);
  fprintf(logfile, "%d\t", base_mismatches);
  fprintf(logfile, "%d\t", transversions);
  fprintf(logfile, "%d\t", transitions);
  fprintf(logfile, "%f\t", percent_sim);
  fprintf(logfile, "%f\t", SE_percent_sim);
  fprintf(logfile, "%f\t", percent_sim_windel);
  fprintf(logfile, "%f\t", SE_percent_sim_windel);
  fprintf(logfile, "%f\t", k_jukes_cantor);
  fprintf(logfile, "%f\t", SE_k_jukes_cantor);
  fprintf(logfile, "%f\t", k_kimura);
  fprintf(logfile, "%f\t", SE_k_kimura);
  if (k_tajima_nei != -1000){
    fprintf(logfile, "%f\t", k_tajima_nei);
    fprintf(logfile, "%f\t", SE_k_tajima_nei);
  }
  else{
    fprintf(logfile, "%s\t", "NA");
    fprintf(logfile, "%s\t", "NA");
  }
  if (d_lake != -1000)
    fprintf(logfile, "%f\t", d_lake);
  else
    fprintf(logfile, "%s\t", "NA");
  fprintf(logfile, "%d\t%d\t%d\t%d\t%d\t",aa,at,ac,ag,an);
  fprintf(logfile, "%d\t%d\t%d\t%d\t%d\t",ta,tt,tc,tg,tn);
  fprintf(logfile, "%d\t%d\t%d\t%d\t%d\t",ca,ct,cc,cg,cn);
  fprintf(logfile, "%d\t%d\t%d\t%d\t%d\t",ga,gt,gc,gg,gn);
  fprintf(logfile, "%d\t%d\t%d\t%d\t%d",na,nt,nc,ng,nn);
  fprintf(logfile, "\n");

  /* save Kimura distance in histogram */
  
  (histogram[((int)rint(k_kimura * 100))])++;

}


/* 
   calkan
   plot the histogram
*/

void plot_histogram(void){
  char histFile[100];
  char bin_histFile[100];
  char plotfile[100];
  char command[200];
  FILE *plot;
  FILE *gnuplot;
  int i, j, tot;
  char *ch;
  int histStart = 0;
  int plotdata[101];

  strcpy(histFile, logfilename);
  if ((ch = strchr(histFile, '.')) != NULL)
    *ch = 0;
  sprintf(plotfile, "%s.gnuplot", histFile);
  sprintf(bin_histFile, "%s.binplot", histFile);
  strcat(histFile, ".histogram");
  
  plot = fopen(histFile, "w");
  for (i=100; i>=0; i--){
    if (histogram[i] != 0)
      histStart=1;
    if (histStart)
      fprintf(plot,"%3d %5d\n",i,histogram[i]);
  }
  fclose(plot);

  for (i=0; i<101; i++){
    plotdata[i] = 0;
  }

  tot = 0;
  for (i=0; i<101 ; i++){
    tot+=histogram[i];
    if ((i+1) % bin_size == 0 && (i!=0 || bin_size == 1) || i==100){
      plotdata[i/bin_size] = tot;
      tot = 0;
    }
  }
  plot = fopen(bin_histFile, "w");
  histStart = 0;
  // print histogram
  for (i=100; i>=0; i--){
    if (plotdata[i] != 0)
      histStart=1;
    if (histStart)
      fprintf(plot,"%3d %5d\n",i,plotdata[i]);
  }

  fclose(plot);
  
  gnuplot = fopen(plotfile, "w");
  fprintf(gnuplot,"set terminal postscript\n");
  fprintf(gnuplot,"set output \"%s.ps\"\n",histFile);
  fprintf(gnuplot, "set title \"Histogram Graph for %s, Bin Size= %d\" \n",histFile, bin_size);
  fprintf(gnuplot, "set ylabel \"Pairs of sequences\" \n");
  fprintf(gnuplot, "set xlabel \"Percentage Kimura Distance\" \n");
  fprintf(gnuplot, "plot \"%s\" with boxes\n", bin_histFile);   
  fclose(gnuplot);
  sprintf(command, "gnuplot -persist %s", plotfile);
  system(command);
  printf("\n\nHistogram graph is in: %s.ps\n", histFile); 
  remove(plotfile);
  remove(bin_histFile);
  if ((ch = strchr(histFile, '.')) != NULL)
    *ch = 0;
  sprintf(bin_histFile,"%s.aln", histFile); // alignment file
  remove(bin_histFile);
  sprintf(bin_histFile,"%s.dnd", histFile); // tree file
  remove(bin_histFile);
  
}
