#include <stdio.h>
#include <string.h>
#include <time.h>

char **seqs;
char **names;
char **quals;

main(int argc, char **argv){
  char tmp[500];
  unsigned int i, j, s;
  FILE *in;
  FILE *out;
  int copies;
  char outfname[100];
  char infname[100];
  char ch;
  int seqcnt=0;
  
  char name[100]; char qual[100]; char seq[100];
  
  if (argc != 3){
    printf("Shuffles a given FASTQ sequence file, given number of times\n");
    printf("%s [infile.fq] [NumOfOuts]\n", argv[0]);
    exit(0);
  }
  if ((in = fopen(argv[1], "r")) == NULL){
    printf("Unable  to open file %s\n", argv[1]);
    exit (0);
  }
  copies = atoi(argv[2]);
  strcpy(infname, argv[1]);
  infname[strlen(infname)-3] = 0;
  
  fprintf(stderr, "Scanning.\n");
  
  while (fscanf(in, "@%s\n%s\n%s\n%s\n", name, seq, tmp, qual) > 0){
    seqcnt++;
  }

  rewind(in);

  seqs = (char **) malloc(sizeof(char *) * seqcnt);
  names = (char **) malloc(sizeof(char *) * seqcnt);
  quals = (char **) malloc(sizeof(char *) * seqcnt);


  for (i=0;i<seqcnt;i++){
    seqs[i] = (char *) malloc(sizeof(char) * 80);
    names[i] = (char *) malloc(sizeof(char) * 80);
    quals[i] = (char *) malloc(sizeof(char) * 80);
  }

  i = 0; j =0;

  fprintf(stderr, "Loading %d sequences.\n", seqcnt);
  while (fscanf(in, "@%s\n%s\n%s\n%s\n", name, seq, tmp, qual) > 0){
    strcpy(seqs[i], seq);
    strcpy(names[i], name);
    strcpy(quals[i], qual);
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
    }
  }

  sprintf(outfname, "%sshuf_%d.fa", infname, copies);
  out = fopen(outfname, "w");
  
  for (i=0; i<seqcnt; i++){
    fprintf(out, "@%s\n%s\n+\n%s\n", names[i], seqs[i], quals[i]);
  }

}
