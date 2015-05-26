#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
  int copies;
  char outfname[100];
  char infname[100];
  char ch;
  int seqcnt=0;
  
  char name[100]; char qual[100]; char seq[100];
  
  if (argc != 4){
    printf("Shuffles a given pair of FASTQ sequence files, given number of times\n");
    printf("%s [infile_1.fq] [infile_2.fq] [NumOfOuts]\n", argv[0]);
    exit(0);
  }
  if ((in = fopen(argv[1], "r")) == NULL){
    printf("Unable  to open file %s\n", argv[1]);
    exit (0);
  }

  if ((in2 = fopen(argv[2], "r")) == NULL){
    printf("Unable  to open file %s\n", argv[2]);
    exit (0);
  }

  copies = atoi(argv[3]);
  strcpy(infname, argv[1]);
  infname[strlen(infname)-5] = 0;
  
  fprintf(stderr, "Scanning.\n");
  
  while (fscanf(in, "@%s\n%s\n%s\n%s\n", name, seq, tmp, qual) > 0){
    seqcnt++;
  }

  rewind(in);

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
  while (fscanf(in, "@%s\n%s\n%s\n%s\n", name, seq, tmp, qual) > 0){
    strcpy(seqs[i], seq);
    strcpy(names[i], name);
    strcpy(quals[i], qual);
    fscanf(in2, "@%s\n%s\n%s\n%s\n", name, seq, tmp, qual);
    strcpy(seqs2[i], seq);
    strcpy(names2[i], name);
    strcpy(quals2[i], qual);
    i++;
  }

  fprintf(stderr, "Loaded %d/%d sequences.\n", i, seqcnt);


  srand(time(NULL));

  for (s=0;s<copies;s++){
    fprintf(stderr, "Shuffle %d.\n", s);
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
  }

  sprintf(outfname, "%sshuf_%d_1.fq", infname, copies);
  out = fopen(outfname, "w");
  sprintf(outfname, "%sshuf_%d_2.fq", infname, copies);
  out2 = fopen(outfname, "w");
  
  for (i=0; i<seqcnt; i++){
    fprintf(out, "@%s\n%s\n+\n%s\n", names[i], seqs[i], quals[i]);
    fprintf(out2, "@%s\n%s\n+\n%s\n", names2[i], seqs2[i], quals2[i]);
  }

}
