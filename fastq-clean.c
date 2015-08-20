#include <stdio.h>
#include <string.h>
#include <zlib.h>

int main(int argc, char **argv){
  int i;
  char fname1[100], fname2[100], prefix[100];
  int maxn;
  FILE *f1, *f2;
  FILE *o1, *o2;
  gzFile g1,g2,go1,go2;
  int readcnt;
  int gz;
  char name[500], read[500], plus[500], qual[500];
  char name2[500], read2[500], plus2[500], qual2[500];
  int cntn, cntn2;
  int len;
  int intcnt;

  intcnt=0; 

  fname1[0] = 0;
  fname2[0] = 0;
  prefix[0] = 0;
  maxn = 0;
  len = -1;
  gz =0;
  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-1"))
      strcpy(fname1, argv[i+1]);
    if (!strcmp(argv[i], "-2"))
      strcpy(fname2, argv[i+1]);
    if (!strcmp(argv[i], "-o"))
      strcpy(prefix, argv[i+1]);
    if (!strcmp(argv[i], "-n"))
      maxn = atoi(argv[i+1]);
    if (!strcmp(argv[i], "-l"))
      len = atoi(argv[i+1]) + 1;
    if (!strcmp(argv[i], "-gz"))
      gz = 1;
  }

  if (fname1[0]==0 || fname2[0]==0){
    fprintf(stderr, "Pass filenames with -1 and -2 parameters.\n");
    return -1;
  }
  
  if (prefix[0]==0){
    fprintf(stderr, "Pass output file prefix with -o parameter.\n");
    return -1;
  }
  
  if (!gz){
    f1 = fopen(fname1, "r");
    f2 = fopen(fname2, "r");
    if (f1 == NULL || f2 == NULL){
      fprintf(stderr, "Cannot open %s and/or %s.\n", fname1, fname2);
      return -1;
    }

    sprintf(fname1, "%s_1.fastq", prefix);
    sprintf(fname2, "%s_2.fastq", prefix);
    
    o1 = fopen(fname1, "w");
    o2 = fopen(fname2, "w");
  }
  else{
    g1 = gzopen(fname1, "r");
    g2 = gzopen(fname2, "r");
    if (g1 == NULL || g2 == NULL){
      fprintf(stderr, "Cannot open %s and/or %s.\n", fname1, fname2);
      return -1;
    }

    sprintf(fname1, "%s_1.fastq.gz", prefix);
    sprintf(fname2, "%s_2.fastq.gz", prefix);
    
    go1 = gzopen(fname1, "w");
    go2 = gzopen(fname2, "w");
    
  }

  
  readcnt = 1;
  
  /*
  if (f1 == NULL || f2 == NULL){
    fprintf(stderr, "Cannot create %s and/or %s.\n", fname1, fname2);
    return -1;
    }*/

  if (!gz){
    while (fgets(name, 500, f1) > 0){
      fgets(read, 500, f1);
      fgets(plus, 500, f1);
      fgets(qual, 500, f1);
      fgets(name2, 500, f2);
      fgets(read2, 500, f2);
      fgets(plus2, 500, f2);
      fgets(qual2, 500, f2);
      intcnt++;
      
      if (maxn != 0){
	
	cntn=0; cntn2=0;
	for (i=0;i<strlen(read);i++)
	  if (read[i]=='N') cntn++;
	if (cntn > maxn)
	  continue;
	for (i=0;i<strlen(read2);i++)
	  if (read2[i]=='N') cntn2++;
	if (cntn2 > maxn)
	  continue;
      }

      if (len!=-1){
	if (strlen(read) < len-1 || strlen(read2) < len-1)
	  continue;	
      }
      
      if (len!=-1){
	if (strlen(read)>len) {read[len+1]=0; read[len]='\n'; qual[len+1]=0; qual[len]='\n';}
	if (strlen(read2)>len) {read2[len+1]=0; read2[len]='\n'; qual2[len+1]=0; qual2[len]='\n';}
      }
      
      fprintf(o1, "@%s.%d\n%s+\n%s", prefix, readcnt, read, qual);
      fprintf(o2, "@%s.%d\n%s+\n%s", prefix, readcnt, read2, qual2);
      
      readcnt++;
    }
    fclose(o1); fclose(o2);
  }
  else{
    while (gzgets(g1, name, 500) > 0){
      gzgets(g1, read, 500);
      gzgets(g1, plus, 500);
      gzgets(g1, qual, 500);
      gzgets(g2, name2, 500);
      gzgets(g2, read2, 500);
      gzgets(g2, plus2, 500);
      gzgets(g2, qual2, 500);
      
      intcnt++;

      if (maxn != 0){
	
	cntn=0; cntn2=0;
	for (i=0;i<strlen(read);i++)
	  if (read[i]=='N') cntn++;
	if (cntn > maxn)
	  continue;
	for (i=0;i<strlen(read2);i++)
	  if (read2[i]=='N') cntn2++;
	if (cntn2 > maxn)
	  continue;
      }

      if (len!=-1){
	if (strlen(read) < len-1 || strlen(read2) < len-1)
	  continue;
      }
      
      
      
      if (len!=-1){
	if (strlen(read)>len) {read[len+1]=0; read[len]='\n'; qual[len+1]=0; qual[len]='\n';}
	if (strlen(read2)>len) {read2[len+1]=0; read2[len]='\n'; qual2[len+1]=0; qual2[len]='\n';}
      }
      
      gzprintf(go1, "@%s.%d\n%s+\n%s", prefix, readcnt, read, qual);
      gzprintf(go2, "@%s.%d\n%s+\n%s", prefix, readcnt, read2, qual2);
      
      readcnt++;
    }
    gzclose(go1); gzclose(go2);
  }

  
  fprintf(stderr, "In: %d read pairs, out: %d read pairs. Eliminated %d (%f%%).\n", intcnt, readcnt, intcnt-readcnt, ((float)(intcnt-readcnt)/intcnt));
}
