#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 250000000
#define SIZE 2000000
#define STDEV 200000

char seq[MAX];


int main(int argc, char **argv){
  FILE *idxfile;
  FILE *in; 
  char *seqname;

  char newseqname[35];
  char ch;
  int len;
  int part;
  int i,j;
  char logname[40];
  FILE *out, *log;

  char chrname[35];
  char idxname[500];
  int istart;
  char infile[100];
  char chrfilename[100];
  char newseqfile[100];
  char *chrfile;
  char outdirname[100];
  char *dirname;
  char systemcmd[1000];
  int dumped;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outdirname, argv[i+1]);
  }

  sprintf(logname, "%s.log", infile);
  log = fopen(logname, "w");

  idxfile = fopen(infile, "r");

  while(fscanf(idxfile, "%s%s%d", idxname, chrname, &istart) > 0){
 
    dumped = 0;

    dirname = strtok(idxname, "/");
    chrfile = strtok(NULL, ".");

    printf("%s\t%s\n", dirname, chrfile);

    
    seqname = (char *) malloc(sizeof(char)*30);
    
    sprintf(chrfilename, "%s/%s", dirname, chrfile);

    printf("chrfilename: %s\n", chrfilename);

    in = fopen(chrfilename, "r");
    if (in == NULL){
      printf("fopen\n"); return 0;
    }

    len = 0;
    
    fscanf(in, "%s\n", seqname);
    if (seqname[0] == '>')
      seqname++;
    
    while(fscanf(in, "%c", &ch) > 0){
      if (!isspace(ch))
	seq[len++] = ch;
    }
    
    fclose(in);
    
    printf("file %s read.\n", chrfilename);

    if (len <= SIZE+STDEV){
      // copy file here
      sprintf(systemcmd, "cp %s %s", chrfilename, outdirname);
      system(systemcmd);
      printf("Copying %s\n", chrfilename);
      fprintf(log, "%s/%s.index\t%s\t%d\n", outdirname, chrfilename, chrname, istart);
      continue;
    }
    
    part = 1;
    
    sprintf(newseqname, "%s_%d",  seqname, part);
    sprintf(newseqfile, "%s/%s_%d", outdirname, seqname, part);
    printf("newseqfile: %s\n", newseqfile);

    out = fopen(newseqfile, "w");
    if (out == NULL){
      printf("fopenout\n"); return 0;
    }
    
    fprintf(log, "%s.index\t%s\t%d\n", newseqfile, chrname, istart);
    printf("partitioning %d\toutdirname:%s\tnewseqname:%s\n", part,outdirname,newseqname);
    fprintf(out, ">%s\n", newseqname);
    
    for (i=0;i<len;i++){
      if (i / SIZE >= part){
	if (len - i <= STDEV){
	  printf("dumping the rest\n");
	  for (j=i;j<len;j++){
	    fprintf(out, "%c", seq[j]);
	    if (j%60==0)
	      fprintf(out, "\n");
	  }
	  fclose(out); dumped = 1; break;
	}
	if (dumped)
	  break;
	fclose(out);
	part = i / SIZE + 1;
	sprintf(newseqname, "%s_%d", seqname, part);
	sprintf(newseqfile, "%s/%s_%d", outdirname, seqname, part);
	printf("newseqfile: %s\n", newseqfile);
	printf("partitioning %d\toutdirname:%s\tnewseqname:%s\n", part,outdirname,newseqname);
	out = fopen(newseqfile, "w");
	fprintf(out, ">%s\n", newseqname);
	fprintf(stdout, "%s.index\t%s\t%d\n", newseqfile, newseqname, (i+istart));
	fprintf(log, "%s.index\t%s\t%d\n", newseqfile, chrname, (i+istart));
      }
      fprintf(out, "%c", seq[i]);
      if (i%60==0)
	fprintf(out, "\n");
    }
    if (dumped)
      continue;

    fclose(out);
    //free(seqname);
     
  }
  
  
  
  return 1;
}
