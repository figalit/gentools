#include <stdio.h>
#include <string.h>
#include <time.h>

  char **seqs;

main(int argc, char **argv){
  char tmp[500];
  int i, j, s;
  FILE *in;
  FILE *out;
  int copies;
  char outfname[100];
  char infname[100];
  char ch;
  int seqcnt=0;
  if (argc != 3){
    printf("Shuffles a given FASTA sequence file, given number of times\n");
    printf("%s [infile.fa] [NumOfOuts]\n", argv[0]);
    exit(0);
  }
  if ((in = fopen(argv[1], "r")) == NULL){
    printf("Unable  to open file %s\n", argv[1]);
    exit (0);
  }
  copies = atoi(argv[2]);
  strcpy(infname, argv[1]);
  infname[strlen(infname)-3] = 0;
  while (fscanf(in, "%c", &ch) > 0)
    if (ch == '>')
      seqcnt++;
  rewind(in);
  seqs = (char **) malloc(sizeof(char *) * seqcnt);
  for (i=0;i<seqcnt;i++)
    seqs[i] = (char *) malloc(sizeof(char) * 80);
  i = -1; j =0;
  while (fscanf(in, "%c", &ch) > 0){
    if (ch == '>'){
      if (i!=-1)
	seqs[i][j] = 0;
      i++; j=0;
    }
    seqs[i][j++] = ch;
  }
  srand(time(NULL));
  for (s=0;s<copies;s++){
    for (i=0; i<seqcnt; i++){
      j = rand()*rand()*rand() % seqcnt;
      strcpy(tmp, seqs[i]);
      strcpy(seqs[i], seqs[j]);
      strcpy(seqs[j], tmp);
    }
    sprintf(outfname, "%sshuf_%d.fa", infname, (s+1));
    out = fopen(outfname, "w");

    for (i=0; i<seqcnt; i++){
      fprintf(out, ">s_%d\n%s\n", i, seqs[i]);
    }
  }
}
