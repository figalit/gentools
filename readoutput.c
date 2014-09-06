// Reads a binary or text output file (gzipped or not)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <assert.h>
#include <zlib.h>

#define SEQ_LENGTH 200

int BINARYOUT = 1;
int GZ = 0;
int HEADER=1;
int DOC=0;

int main (int argc, char **argv) {
    char inputfilename[SEQ_LENGTH];
    FILE *input;
    int i;
    char lineString[10000];

    /*** Data to be read from each line ***/
    char fosmidNameLengthShort;
    char fosmidName[256];
    int chrStart;
    int chrEnd;
    short int maxWinStart;
    short int maxWinEnd;
    short int maxWin;
    char orientchar;
    char DBNAMELengthShort;
    char DBNAME[256];
    int sequence_count;
    FILE *doc;
    char fname[200];
    int linecnt=0;
    int WRITEHEADER=1;


    /*** End of data to be read ***/

    for (i=1;i<argc;i++){
	if (!strcmp(argv[i], "-text"))
	    BINARYOUT = 0;
	else if (!strcmp(argv[i], "-gz"))
	    GZ = 1;
	else if (!strcmp(argv[i], "-nh"))
	    HEADER = 0;
	else if (!strcmp(argv[i], "-nwh"))
	    WRITEHEADER = 0;
	else if (!strcmp(argv[i], "-doc")){
	    DOC = 1;
	}
	else if (!strcmp(argv[i], "-infile")) {
	    strcpy(inputfilename, argv[i+1]);
	}
    }

    if (!GZ) {
	input = fopen(inputfilename, "r");
    } else {
	input = gzopen(inputfilename, "r");
    }

    if (DOC){
      sprintf(fname, "%s.tab.gz", inputfilename);
      doc = gzopen(fname, "w");
    }

    //gzwrite(doc, &linecnt, sizeof(linecnt));
    //fwrite(&linecnt, sizeof(linecnt), 1, doc);

    if (!BINARYOUT) {
	if (!GZ) {
	    if (HEADER){
	    	fscanf(input, "%d\n", &sequence_count);
		if (!DOC && WRITEHEADER)
		  printf("%d\n", sequence_count);
	    }
	    while (!feof(input)) {
		fscanf(input, "%s\t%d\t%d\t%d\t%d\t%d\t\t%c\t%s", fosmidName, &chrStart, &chrEnd, &maxWinStart, &maxWinEnd, &maxWin, &orientchar, DBNAME);
		linecnt++;
		fscanf(input, "\n");
		if (!DOC)
		  printf("%10s\t%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", fosmidName, chrStart, chrEnd, maxWinStart, maxWinEnd, maxWin, orientchar, DBNAME);
		else
		  gzprintf(doc, "%s\t%d\n", DBNAME, chrStart);//, chrEnd);
	    }
	} else {
	    if (HEADER){
	    	gzgets(input, lineString, 10000);
	    	sscanf(lineString, "%d\n", &sequence_count);
		if (!DOC && WRITEHEADER)
		  printf("%d\n", sequence_count);
	    }
	    while (!gzeof(input)) {
		gzgets(input, lineString, 10000);
		linecnt++;
		sscanf(lineString, "%s\t%d\t%d\t%d\t%d\t%d\t\t%c\t%s", fosmidName, &chrStart, &chrEnd, &maxWinStart, &maxWinEnd, &maxWin, &orientchar, DBNAME);
		if (!DOC)
		  printf("%10s\t%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", fosmidName, chrStart, chrEnd, maxWinStart, maxWinEnd, maxWin, orientchar, DBNAME);  
		else
		  gzprintf(doc, "%s\t%d\n", DBNAME, chrStart);//, chrEnd);
	    }
	}
    } else {
	if (!GZ) {
    	    if (HEADER){
	    	fread(&sequence_count, sizeof(sequence_count), 1, input);
		if (!DOC && WRITEHEADER)
		  printf("%d\n", sequence_count);
	    }
	    while (!feof(input)) {
		fread(&fosmidNameLengthShort, sizeof(fosmidNameLengthShort), 1, input);
		linecnt++;

		if (feof(input))
		    break;

		fread(fosmidName, fosmidNameLengthShort*sizeof(char), 1, input);
		fosmidName[fosmidNameLengthShort] = '\0';
		fread(&chrStart, sizeof(chrStart), 1, input);
		fread(&chrEnd, sizeof(chrEnd), 1, input);
		fread(&maxWinStart, sizeof(maxWinStart), 1, input);
		fread(&maxWinEnd, sizeof(maxWinEnd), 1, input);
		fread(&maxWin, sizeof(maxWin), 1, input);
		fread(&orientchar, sizeof(orientchar), 1, input);
		fread(&DBNAMELengthShort, sizeof(DBNAMELengthShort), 1, input);
		fread(DBNAME, DBNAMELengthShort*sizeof(char), 1, input);
		DBNAME[DBNAMELengthShort] = '\0';
		
		if (!DOC)
		  printf("%10s\t%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", fosmidName, chrStart, chrEnd, maxWinStart, maxWinEnd, maxWin, orientchar, DBNAME); 
		else
		  gzprintf(doc, "%s\t%d\n", DBNAME, chrStart);//, chrEnd);
	    }
	} else {
	    if (HEADER){
	    	gzread(input, &sequence_count, sizeof(sequence_count));
		if (!DOC && WRITEHEADER)
		  printf("%d\n", sequence_count);
		else
		  printf("%s\t%d\n", DBNAME, chrStart);//, chrEnd);
	    }
	    while (!gzeof(input)) {
		gzread(input, &fosmidNameLengthShort, sizeof(fosmidNameLengthShort));
		linecnt++;

		if (gzeof(input))
		    break;

		gzread(input, fosmidName, fosmidNameLengthShort*sizeof(char));
		fosmidName[fosmidNameLengthShort] = '\0';
		gzread(input, &chrStart, sizeof(chrStart));
		gzread(input, &chrEnd, sizeof(chrEnd));
		gzread(input, &maxWinStart, sizeof(maxWinStart));
		gzread(input, &maxWinEnd, sizeof(maxWinEnd));
		gzread(input, &maxWin, sizeof(maxWin));
		gzread(input, &orientchar, sizeof(orientchar));
		gzread(input, &DBNAMELengthShort, sizeof(DBNAMELengthShort));
		gzread(input, DBNAME, DBNAMELengthShort*sizeof(char));
		DBNAME[DBNAMELengthShort] = '\0';

		if (!DOC)
		  printf("%10s\t%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", fosmidName, chrStart, chrEnd, maxWinStart, maxWinEnd, maxWin, orientchar, DBNAME); 
		else
		  gzprintf(doc, "%s\t%d\n", DBNAME, chrStart);//, chrEnd);
	    }
	}
    }

    
    if (DOC){
      fprintf(stderr, "%d lines.\n", linecnt);
      //gzrewind(doc);
      //gzwrite(doc, &linecnt, sizeof(linecnt));
      //fwrite(&linecnt, sizeof(linecnt), 1, doc);
      gzclose(doc);
    }

    if (!GZ)
	fclose(input);
    else
	gzclose(input);

}
