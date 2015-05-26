#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#define SEQLEN 200
char **seqs;
char **names;
char **quals;
char **seqs2;
char **names2;
char **quals2;

main(int argc, char **argv){
  char tmp[500];
  unsigned int i, j, s;
  FILE *in;
  FILE *in2;
  FILE *out;
  FILE *out2;
  gzFile gin;
  gzFile gin2;
  gzFile gout;
  gzFile gout2;
  int copies;
  char outfname[SEQLEN];
  char outprefix[SEQLEN];
  char ch;
  int seqcnt=0;
  int gzmode=0;

  char name[SEQLEN]; char qual[SEQLEN]; char seq[SEQLEN];
  
  if (argc != 4){
    printf("Shuffles a given pair of FASTQ sequence files.\n");
    printf("%s [infile_1.fq] [infile_2.fq] [outprefix]\n", argv[0]);
    exit(0);
  }

  if (argv[1][strlen(argv[1])-1] == 'z' && argv[1][strlen(argv[1])-2] == 'g') // cheap trick
    gzmode = 1;

  if (!gzmode){

    if ((in = fopen(argv[1], "r")) == NULL){
      printf("Unable  to open file %s\n", argv[1]);
      exit (0);
    }
    
    if ((in2 = fopen(argv[2], "r")) == NULL){
      printf("Unable  to open file %s\n", argv[2]);
      exit (0);
    }
  }
  else{
    if ((gin = gzopen(argv[1], "r")) == NULL){
      printf("Unable  to open file %s\n", argv[1]);
      exit (0);
    }
    
    if ((gin2 = gzopen(argv[2], "r")) == NULL){
      printf("Unable  to open file %s\n", argv[2]);
      exit (0);
    }
  }

  strcpy(outprefix, argv[3]);
  
  fprintf(stderr, "Scanning.\n");

  if (!gzmode){
    while (!feof(in)){
      fgets(tmp, SEQLEN, in); 
      if (feof(in)) break;
      fgets(tmp, SEQLEN, in);
      fgets(tmp, SEQLEN, in); // + line
      fgets(tmp, SEQLEN, in);    
      
      seqcnt++;
    }
    rewind(in);
  }
  else{
    while (!gzeof(gin)){
      gzgets(gin, tmp, SEQLEN); 
      if (gzeof(gin)) break;
      gzgets(gin, tmp, SEQLEN);
      gzgets(gin, tmp, SEQLEN); // + line
      gzgets(gin, tmp, SEQLEN);    
      
      seqcnt++;
    }
    gzrewind(gin);
  }

  seqs = (char **) malloc(sizeof(char *) * seqcnt);
  names = (char **) malloc(sizeof(char *) * seqcnt);
  quals = (char **) malloc(sizeof(char *) * seqcnt);

  seqs2 = (char **) malloc(sizeof(char *) * seqcnt);
  names2 = (char **) malloc(sizeof(char *) * seqcnt);
  quals2 = (char **) malloc(sizeof(char *) * seqcnt);


  for (i=0;i<seqcnt;i++){
    seqs[i] = (char *) malloc(sizeof(char) * SEQLEN);
    names[i] = (char *) malloc(sizeof(char) * SEQLEN);
    quals[i] = (char *) malloc(sizeof(char) * SEQLEN);
    seqs2[i] = (char *) malloc(sizeof(char) * SEQLEN);
    names2[i] = (char *) malloc(sizeof(char) * SEQLEN);
    quals2[i] = (char *) malloc(sizeof(char) * SEQLEN);
  }

  i = 0; j =0;

  fprintf(stderr, "Loading %d sequences.\n", seqcnt);

  if (!gzmode){
    while (!feof(in)){
      fgets(names[i], SEQLEN, in); 
      if (feof(in)) break;
      fgets(seqs[i], SEQLEN, in);
      fgets(tmp, SEQLEN, in); // + line
      fgets(quals[i], SEQLEN, in);
      
      fgets(names2[i], SEQLEN, in2); 
      fgets(seqs2[i], SEQLEN, in2);
      fgets(tmp, SEQLEN, in2); // + line
      fgets(quals2[i], SEQLEN, in2);
      
      i++;
    }
  }
  else{
    while (!gzeof(gin)){
      gzgets(gin, names[i], SEQLEN); 
      if (gzeof(gin)) break;
      gzgets(gin, seqs[i], SEQLEN);
      gzgets(gin, tmp, SEQLEN); // + line
      gzgets(gin, quals[i], SEQLEN);
      
      gzgets(gin2, names2[i], SEQLEN); 
      gzgets(gin2, seqs2[i], SEQLEN);
      gzgets(gin2, tmp, SEQLEN); // + line
      gzgets(gin2, quals2[i], SEQLEN);
      
      i++;
    }
  }

  fprintf(stderr, "Loaded %d/%d sequences.\n", i, seqcnt);


  srand(time(NULL));

  fprintf(stderr, "Shuffling.\n");
  for (i=0; i<seqcnt; i++){
    j = rand() % seqcnt;
    strcpy(seq, seqs[i]);
    strcpy(seqs[i], seqs[j]);
    strcpy(seqs[j], seq);
    
    strcpy(qual, quals[i]);
    strcpy(quals[i], quals[j]);
    strcpy(quals[j], qual);
    
    strcpy(name, names[i]);
    strcpy(names[i], names[j]);
    strcpy(names[j], name);
    
    strcpy(seq, seqs2[i]);
    strcpy(seqs2[i], seqs2[j]);
    strcpy(seqs2[j], seq);

    strcpy(qual, quals2[i]);
    strcpy(quals2[i], quals2[j]);
    strcpy(quals2[j], qual);
    
    strcpy(name, names2[i]);
    strcpy(names2[i], names2[j]);
    strcpy(names2[j], name);
  }
  

  if (!gzmode){
    sprintf(outfname, "%sshuf_1.fq", outprefix);
    out = fopen(outfname, "w");
    sprintf(outfname, "%sshuf_2.fq", outprefix);
    out2 = fopen(outfname, "w");
    
    for (i=0; i<seqcnt; i++){
      fprintf(out, "%s%s+\n%s", names[i], seqs[i], quals[i]);
      fprintf(out2, "%s%s+\n%s", names2[i], seqs2[i], quals2[i]);
    }
    
    fclose(out); fclose(out2);
  }
  else{
    sprintf(outfname, "%sshuf_1.fq.gz", outprefix);
    gout = gzopen(outfname, "w");
    sprintf(outfname, "%sshuf_2.fq.gz", outprefix);
    gout2 = gzopen(outfname, "w");
  
    for (i=0; i<seqcnt; i++){
      gzprintf(gout, "%s%s+\n%s", names[i], seqs[i], quals[i]);
      gzprintf(gout2, "%s%s+\n%s", names2[i], seqs2[i], quals2[i]);
    }
  
    gzclose(gout); gzclose(gout2);
  }
}
