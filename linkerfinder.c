#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>


#define TRUE 1
#define FALSE 0
#define MAX_LINKER_SEQ_LENGTH 200
#define MAX_LINE_LENGTH 1000
#define MAX_FASTA_LENGTH 5000
#define MAX_FILE_NAME_LENGTH 100

char *linkerFileName;
FILE *linkerFile;
char *fastaDirName;
DIR *fastaDir;
char *mapDirName;
int diverPerc;
char linkerSeq[MAX_LINKER_SEQ_LENGTH];
int lenLinkerSeq;

struct timeval start, end;
struct timezone tz;

void processFile(char* fastaFileName, char* mapFileName, char* outFileName);
void printLinkerEnds(FILE *fastaFile, char *fastaName, int fastaStart, int fastaEnd, FILE *outFile);
void getFastaSeqFromFile(FILE *fastaFile, char *fastaName, char *fastaSeq);
void reverseCompSeq(char *fastaSeq, char *reverseCompSeq);
void getLinkerSeq(char *fileName, char *linkerSeq, int *lenLinkerSeq);


int main(int argc, char **argv) {

  if (argc != 5) {
    printf("usage: %s <linker sequnce file> <directory of fasta files> <directory of RepeatMasker outputs> <maximum allowed divergence> \n", argv[0]);	
    exit(-1);
  }
  
  gettimeofday(&start, &tz);
  
  linkerFileName = argv[1];
  fastaDirName = argv[2];
  mapDirName = argv[3];
  diverPerc = atoi(argv[4]);
  
  getLinkerSeq(linkerFileName, linkerSeq, &lenLinkerSeq);
  //printf("linker seq: %s, length linker seq: %d\n", linkerSeq, lenLinkerSeq);
  
  if ((fastaDir = opendir(fastaDirName)) == NULL) {
  	printf("Input fasta directory %s cannot be opened!\n", fastaDirName);
  	exit(-1);
  }

  system("mkdir Output");
  char fastaFileName[MAX_FILE_NAME_LENGTH];
  char mapFileName[MAX_FILE_NAME_LENGTH];
  
  struct dirent *fastaDirEntryPtr = readdir(fastaDir);
  
  while (fastaDirEntryPtr != NULL) {
    if (fastaDirEntryPtr -> d_name[0] != '.') {
      sprintf(fastaFileName, "%s/%s", fastaDirName, fastaDirEntryPtr -> d_name);
      sprintf(mapFileName, "%s/%s.out", mapDirName, fastaDirEntryPtr -> d_name);
      //printf("%s - %s | ", fastaFileName, mapFileName);
      processFile(fastaFileName, mapFileName, fastaDirEntryPtr -> d_name);
      //printf(".");
    }
    fastaDirEntryPtr = readdir(fastaDir); 
  }  

  gettimeofday(&end, &tz);
  printf("\nComputation: %ld microseconds.\n", ((long)(end.tv_sec*1000000+end.tv_usec)-(long)(start.tv_sec*1000000+start.tv_usec)));
 
  exit(0);
}

void processFile(char* fastaFileName, char* mapFileName, char* outFileName) {
  
  FILE *fastaFile;
  FILE *mapFile;
  
  if ((fastaFile = fopen(fastaFileName, "r")) == NULL) {
    printf("Input fasta file %s cannot be opened!\n", fastaFileName);
    exit(0);
  }
  
  if ((mapFile = fopen(mapFileName, "r")) == NULL) {
    printf("Input map file %s cannot be opened\n", mapFileName);
    exit(0);
  }
 
  FILE* outFile;
  char outputFileName[MAX_FILE_NAME_LENGTH];
  sprintf(outputFileName, "Output/%s", outFileName);
  
  if ((outFile = fopen(outputFileName, "w")) == NULL) {
  	printf("Output file %s cannot be created!\n", outputFileName);
  	exit(-1);
  }
  
  //parse the Repeatmasker output
  char line[MAX_LINE_LENGTH];
  int matchScore;
  double mismatch;
  double gapDel;
  double gapIns;
  char *fastaName;
  int fastaStart;
  int fastaEnd;
  char *numAfterBases;
  char *direction;
  char *repeatName;
  char *repeatClass;  //NULL all the time here
  char *numBeforeBases;
  int linkerStart;
  int linkerEnd;
  
  //total number of gaps before/after the mapping part of the linker seq 
  //to consider the alignment
  double allowedDiff = lenLinkerSeq * diverPerc / 100;

  while (fgets(line, MAX_LINE_LENGTH, mapFile) != NULL) {
  	matchScore = atoi(strtok(line, " \t"));
  	mismatch = atof(strtok(NULL, " \t"));
  	gapDel = atof(strtok(NULL, " \t"));
  	gapIns = atof(strtok(NULL, " \t"));
  	fastaName = strtok(NULL, " \t");
  	fastaStart = atoi(strtok(NULL, " \t"));
  	fastaEnd = atoi(strtok(NULL, " \t"));
  	numAfterBases = strtok(NULL, " \t");
  	direction = strtok(NULL, " \t");
  	repeatName = strtok(NULL, " \t");
  	//repeatClass = strtok(NULL, " \t");
  	numBeforeBases = strtok(NULL, " \t");
  	linkerStart = atoi(strtok(NULL, " \t"));
  	linkerEnd = atoi(strtok(NULL, " \t"));
  	
  	//printf("%d - %lf - %lf - %lf - %s - %d - %d - %s - %s - %s - %s - %d - %d\n", matchScore, mismatch, gapDel, gapIns, fastaName, fastaStart, fastaEnd, 
  	//                                                                                   numAfterBases, direction, repeatName, numBeforeBases, linkerStart, linkerEnd);
  	
  	int eligible = FALSE;
    if ((mismatch + gapDel + gapIns) <= diverPerc) {
      //check if (almost) the whole sequence is aligned
      if (direction[0] == '+') {
        int lenMap = linkerEnd - linkerStart + 1;
        if ((lenMap - (lenMap * (mismatch+gapDel+gapIns) / 100) + allowedDiff) >= lenLinkerSeq)
          eligible = TRUE;
      }
      else if (direction[0] == 'C') {
      	int lenMap = linkerStart - linkerEnd + 1;
        if ((lenMap - (lenMap * (mismatch+gapDel+gapIns) / 100) + allowedDiff) >= lenLinkerSeq)
      	  eligible = TRUE;
      }
    }
    
    if (eligible == TRUE)
      printLinkerEnds(fastaFile, fastaName, fastaStart, fastaEnd, outFile);
  }
  fclose(fastaFile);
  fclose(mapFile);
  fclose(outFile);
}

void printLinkerEnds(FILE *fastaFile, char *fastaName, int fastaStart, int fastaEnd, FILE *outFile) {
  
  char fastaSeq[MAX_FASTA_LENGTH];
  getFastaSeqFromFile(fastaFile, fastaName, fastaSeq);
  //printf("fasta name: %s\nfasta seq: %s\n", fastaName, fastaSeq);
  
  char seqBefLinker[MAX_FASTA_LENGTH];
  char seqAfterLinker[MAX_FASTA_LENGTH];
  
  strncpy(seqBefLinker, fastaSeq, fastaStart); 
  seqBefLinker[fastaStart-1] = '\0';
  strcpy(seqAfterLinker, fastaSeq + fastaEnd);
  //seqAfterLinker[strlen(seqAfterLinker)-1] = '\0';
  
  //printf("fasta start: %d, fasta end: %d, fasta length: %d\n", fastaStart, fastaEnd, strlen(fastaSeq));
  //printf("seqBefLinker: %s\n", seqBefLinker);
  //printf("seqAfterLinker: %s\n", seqAfterLinker);
  
  //reverse complement the last part of the fasta sequence
  char reverseComp[MAX_FASTA_LENGTH];
  reverseCompSeq(seqAfterLinker, reverseComp);
  //printf("reverseComp: %s\n*********************\n", reverseComp);
  
  fprintf(outFile, ">%s.FORWARD.1\n%s\n", fastaName, seqBefLinker);
  fprintf(outFile, ">%s.REVERSE.1\n%s\n", fastaName, reverseComp);
}

void getFastaSeqFromFile(FILE *fastaFile, char *fastaName, char *fastaSeq) {

  char line[MAX_LINE_LENGTH];
  
  sprintf(fastaSeq, "");
  
  while (fgets(line, MAX_LINE_LENGTH, fastaFile) != NULL) {
    if (strstr(line, fastaName) != NULL) {//sequence fasta header found    
      while (fgets(line, MAX_LINE_LENGTH, fastaFile)) { //the next sequence or the end of file is not reached yet
        if (line[0] != '>') {
          char *pch = strchr(line, '\n');
          if (pch != NULL)
            *pch = '\0';
          strcat(fastaSeq, line);
        }
        else
          break;
      }
      break;
    }
    char *pch = strchr(line, '\n');
    if (pch != NULL)
      *pch = '\0';
  }
  
  //rewind to the beginning of the file
  rewind(fastaFile);
  //fseek (fastaFile , 0 , SEEK_SET );
}

void reverseCompSeq(char *fastaSeq, char *reverseCompSeq) {

  int lenFasta = strlen(fastaSeq); //1 for the \0 char
 
  //mirror seq first
  char mirroredSeq[MAX_FASTA_LENGTH];
  int k;
  for (k = 0; k < lenFasta; k++)
    mirroredSeq[k] = fastaSeq[lenFasta-k-1];
    
  //complement seq
  for (k = 0; k < lenFasta; k++) {
    switch (mirroredSeq[k]) {
      case 'A':
      case 'a':
        reverseCompSeq[k] = 'T';
        break;      
      case 'T':
      case 't':
        reverseCompSeq[k] = 'A';
        break;
      case 'C':
      case 'c':
        reverseCompSeq[k] = 'G';
      	break;
      case 'G':
      case 'g':
        reverseCompSeq[k] = 'C';
        break;
      case 'N':
      case 'n':
        reverseCompSeq[k] = 'N';
    }
  }
  reverseCompSeq[lenFasta] = '\0';
}

void getLinkerSeq(char *fileName, char *linkerSeq, int *lenLinkerSeq) {

  if ((linkerFile = fopen(fileName, "r")) == NULL) {
  	printf("Input file %s cannot be opened!\n", fileName);
  	return;
  }
  
  char seqName[MAX_LINKER_SEQ_LENGTH]; 
  fgets(seqName, MAX_LINE_LENGTH, linkerFile); //get the sequence name
  fgets(linkerSeq, MAX_LINE_LENGTH, linkerFile); //get the actual sequence
  
  *lenLinkerSeq = strlen(linkerSeq)-1; //get rid of the new line
  linkerSeq[*lenLinkerSeq] = '\0';
}
