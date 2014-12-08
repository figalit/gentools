#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// boolean
typedef int bool;

#define TRUE 1
#define FALSE 0

// long
#define MAX_VALUE +2147483647

// max 128 samples are allowed
// max defined chromosome number
const int MAX_NUMBER_OF_FILES = 128;
const int MAX_NUMBER_OF_CHROMOSOMES = 93;

long dupNo = 0;

struct Read {
	
	long start;
	long end;

};

struct InputHolder {

	char** sampleNames;
	char** fileNames;
	FILE** filePointers;
	int fileCount;

};

// keeps the last read that is taken from the file with the fileIndex 

struct ReadHolder {
	
	struct Read* read;
	int fileIndex;

};

void* cleanMalloc(size_t size) {
 	void* memSpace = malloc(size);
	memset(memSpace, 0, size);
	
	return memSpace;
}

// reads the next "targetChr" chromosome read from the given file
struct Read* readNext(FILE *fp, const char* targetChr) { 

	char chrm[50]= {0};

	long start = 0;
	long end = 0;

	while(!feof(fp) && fscanf(fp, "%s\t%ld\t%ld\n", chrm, &start, &end) > 0) {
		
		if(strcmp(chrm, targetChr) == 0) {

			struct Read* read = malloc(sizeof(struct Read));
			read->start = start;
			read->end = end;
			
			return read;

		}

	}
	
	fclose(fp);
	
	return NULL;
	
}

// finds a read that has the minimum start value from the active reads. Reads a new chromosome read from the corresponding file
struct ReadHolder* findWithMinStart(struct Read** reads, const struct InputHolder* inputHolder, const long startFrom, const char* chromosomeName) {
	
	int i=0, j=inputHolder->fileCount, foundIndex = -1;
	long minStart = MAX_VALUE;

	struct ReadHolder* readWithMinStart = NULL;

	for (; i<j; i++) { // to find the position which has minimum start point and which read has that position

		if(reads[i] != NULL && reads[i]->start >= startFrom && reads[i]->start < minStart) {

			minStart = reads[i]->start;
			foundIndex = i;

		}

	}

	if(foundIndex != -1) { // if there is a read then assign 

		readWithMinStart = malloc(sizeof(struct ReadHolder));
		readWithMinStart->read = reads[foundIndex];
		readWithMinStart->fileIndex = foundIndex;

		reads[foundIndex] = readNext(inputHolder->filePointers[foundIndex], chromosomeName);
	
	}

	return readWithMinStart;

}

struct InputHolder* readSampleNamesAndFileNames(const char* bedFileNamesFile) {

	FILE *fp;

	if ((fp = fopen(bedFileNamesFile, "r")) == NULL) {

		printf("%s could not be opened...\n", bedFileNamesFile);
		exit(EXIT_FAILURE);

	}

	char** sampleNames = cleanMalloc(MAX_NUMBER_OF_FILES * sizeof(char*));	
	char** fileNames = cleanMalloc(MAX_NUMBER_OF_FILES * sizeof(char*));

	char sampleName[50] = {0};
	char fileName[50] = {0};

	int fileCount = 0;

	while(!feof(fp) && fscanf(fp, "%s %s\n", sampleName, fileName) > 0) {

		if(fileCount == MAX_NUMBER_OF_FILES) {

			printf("Maximum 128 samples are allowed...\n");
			exit(EXIT_FAILURE);
		
		}

		char* sampleNameCopy = cleanMalloc(50 * sizeof(char));
		memcpy(sampleNameCopy, sampleName, 50);
		
		char* fileNameCopy = malloc(50 * sizeof(char));
		memcpy(fileNameCopy, fileName, 50);

		sampleNames[fileCount] = sampleNameCopy;
		fileNames[fileCount++] = fileNameCopy;
	}

	struct InputHolder* inputHolder = malloc(sizeof(struct InputHolder));

	inputHolder->sampleNames = sampleNames;
	inputHolder->fileNames = fileNames;
	inputHolder->fileCount = fileCount;
	inputHolder->filePointers = cleanMalloc(MAX_NUMBER_OF_FILES * sizeof(FILE*));

	return inputHolder;
	
} 

void openBedFiles(struct InputHolder* inputHolder) {
	
	int i=0, j = inputHolder->fileCount;

	for(; i<j; i++) {
	
		FILE *bedFp;
	
		if ((bedFp = fopen(inputHolder->fileNames[i], "r")) == NULL) {

			printf("Bed file could not be opened %s ...\n", inputHolder->fileNames[i]);
			exit(EXIT_FAILURE);

		}

		inputHolder->filePointers[i] = bedFp;
		
	}

}

char** readChromosomeNames(const char* chromosomeFileName) {
	
	FILE *fp;

	if ((fp = fopen(chromosomeFileName, "r")) == NULL) {

		printf("Chromosomes file could not be opened. %s\n", chromosomeFileName);
		exit(EXIT_FAILURE);

	}

	char chromosomeName[50] = {0};
	long length = 0; 

	char** chromosomeNames = cleanMalloc((MAX_NUMBER_OF_FILES+1) * sizeof(char*));

	int chromosomeCount = 0;

	while(!feof(fp) && fscanf(fp, "%s %ld\n", chromosomeName, &length) > 0) {
		
		char *chromosomeNameCopy= malloc(50 * sizeof(char));
		memcpy(chromosomeNameCopy, chromosomeName, 50);

		chromosomeNames[chromosomeCount++] = chromosomeNameCopy;

	}

	fclose(fp);

	return chromosomeNames;
}

// finds index the read with minimum end value within current reads
int findMinEndIndex(struct Read** processingReads, int readCount) {

	int foundIndex = -1, i = 0;
	long min = MAX_VALUE;

	for(; i<readCount; i++) {

		if(processingReads[i] != NULL && processingReads[i]->end < min) {

			min = processingReads[i]->end;
			foundIndex = i;
	
		}
	
	}

	return foundIndex;
}

// checks if the new read intersects with current reads 
bool isIntersecting(struct Read** reads, const int count, struct Read* nextRead) {

	int i = 0;
	
	for(; i < count; i++) {
		
		if(reads[i]!=NULL && reads[i++]->end >= nextRead->start) {
			
			return TRUE;
			
		}
		
	}

	return FALSE;
}

void writeReads(struct InputHolder* inputHolder, const char* chromosomeName, struct Read** processingReads, const long processingReadsStart, const long until,  FILE* outputFilePtr) {
	
	int i = 0, j = inputHolder->fileCount;

	const long svLength = until - processingReadsStart;

	bool firstReadWritten = FALSE;

	for(; i<j; i++) {

		if(processingReads[i] != NULL) {
						
									
			if (!firstReadWritten) {
							
				fprintf(outputFilePtr, "%s \t %ld \t dup_%ld \t N \t <DUP> . \t . \t END=%ld;IMPRECISE;SVLEN=%ld;SVTYPE=DUP;SAMPLE=%s", 
					chromosomeName, processingReadsStart, dupNo, until, svLength, inputHolder->sampleNames[i]);								
				dupNo++;
				firstReadWritten = TRUE;
							
			} else {
							
				fprintf(outputFilePtr, ",%s", inputHolder->sampleNames[i]);	
							
			}

		}

	}
					
	if(firstReadWritten) {
				
		fprintf(outputFilePtr, ";SVMETHOD=RD;SVALG=mrCaNaVar\n");
					
	}

}

void insertRead(struct Read** processingReads, struct ReadHolder* readHolder, long *processingReadsStart, int* processingReadsCount) {
	processingReads[readHolder->fileIndex] = readHolder->read;
	*processingReadsStart = readHolder->read->start;
	*processingReadsCount = *processingReadsCount + 1;
}

void freeRead(struct Read** processingReads, const int indexToFree, long* processingReadsStart, int* processingReadsCount) {
	
	*processingReadsStart = processingReads[indexToFree]->end;
	*processingReadsCount = *processingReadsCount - 1;
		
	free(processingReads[indexToFree]);
	processingReads[indexToFree] = NULL;
}

void processChromosome(struct InputHolder* inputHolder, const char* chromosomeName, FILE* outputFilePtr) {

	int i, processingReadsCount = 0, inputFileCount = inputHolder->fileCount, minEndIndex;
	long processingReadsStart = 0, svLength = 0;


	// in each iteration there will be maximum one read from each sample
	struct Read** reads = cleanMalloc(inputFileCount * sizeof(struct Read*));  

	// processing reads have common parts
	struct Read** processingReads = cleanMalloc(inputFileCount * sizeof(struct Read*));

	for(i=0; i<inputFileCount; i++) {
	
		reads[i] = readNext(inputHolder->filePointers[i], chromosomeName);		

	}
	
	struct ReadHolder* nextReadHolder;

	while((nextReadHolder = findWithMinStart(reads, inputHolder, processingReadsStart, chromosomeName)) != NULL) {

		if(processingReadsCount == 0) {

			insertRead(processingReads, nextReadHolder, &processingReadsStart, &processingReadsCount);
		
		} else {

			if(isIntersecting(processingReads, inputFileCount, nextReadHolder->read)) {

				while ( (minEndIndex = findMinEndIndex(processingReads, inputFileCount)) != -1 && processingReads[minEndIndex]->end < nextReadHolder->read->start) {

					writeReads(inputHolder, chromosomeName, processingReads, processingReadsStart, processingReads[minEndIndex]->end, outputFilePtr);

					freeRead(processingReads, minEndIndex, &processingReadsStart, &processingReadsCount);
					
				}
				
				writeReads(inputHolder, chromosomeName, processingReads, processingReadsStart, nextReadHolder->read->start, outputFilePtr);

				insertRead(processingReads, nextReadHolder, &processingReadsStart, &processingReadsCount);

			} else {

				while ( (minEndIndex = findMinEndIndex(processingReads, inputFileCount)) != -1) {

					writeReads(inputHolder, chromosomeName, processingReads, processingReadsStart, processingReads[minEndIndex]->end, outputFilePtr);

					freeRead(processingReads, minEndIndex, &processingReadsStart, &processingReadsCount);
					
				}

				insertRead(processingReads, nextReadHolder, &processingReadsStart, &processingReadsCount);

			}
		}
		
		fflush(outputFilePtr);
	
	}

	while ( (minEndIndex = findMinEndIndex(processingReads, inputFileCount)) != -1) {

		writeReads(inputHolder, chromosomeName, processingReads, processingReadsStart, processingReads[minEndIndex]->end, outputFilePtr);

		freeRead(processingReads, minEndIndex, &processingReadsStart, &processingReadsCount);
	
	}
	
}

int main(int argc, char const *argv[]) {

	if(argc != 3) {
	
		printf("Two input files are required... (Sample names...bed files) and (chromosome names) files are required...\n");
		exit(EXIT_FAILURE);
	
	}

	struct InputHolder* inputHolder = readSampleNamesAndFileNames(argv[1]);

	int i;

	//output file name
	char* fileName = "sv.vcf";
	FILE* outputFilePtr = fopen(fileName,"w");

	// output file problem while opening 
	if(outputFilePtr == NULL) { 

		printf("%s file could not be opened for writing...\n", fileName);
		exit(EXIT_FAILURE);

	}
	
	i = 0;
	char **chromosomeNames = readChromosomeNames(argv[2]);
	char *chromosomeName;
	
	while( (chromosomeName = chromosomeNames[i++]) != NULL ){
		openBedFiles(inputHolder);
		processChromosome(inputHolder, chromosomeName, outputFilePtr); // The problem is here!!!

	}

	//processChromosome(inputHolder, "chr1",outputFilePtr);
	
	fclose(outputFilePtr);
	
	return 0;

}