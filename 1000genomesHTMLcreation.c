#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* genome name - library name - correlation
NA18974	2485444798	0.942922
NA18968	2485445945	0.965102
NA18967	2485447156	0.870640
NA18971	2485447298	0.939952
*/

/* sequence coverage file
Genome name:	2485444798 //actually lib name
Total seqs:	319154239
Total A cnt:	3504078597
Total C cnt:	2182618686
Total G cnt:	2330271555
Total T cnt:	3364865291
Total N cnt:	107718475
Sequence coverage:	3.829851
*/

/* Correl_Graphs/
2485444798.eps  2485447298.eps  2485447511.eps  2485447533.eps  
2485447657.eps  2485447673.eps  2485447994.eps  2485448116.eps
*/

int main(int argc, char **argv){

  if (argc != 4) {
    printf("usage: %s <genomename libname correl file> <library sequence coverage file> <output filename>\n", argv[0]);
    exit(-1);
  }

  char* genlibFileName = argv[1];
  char* coverFileName = argv[2];
  char* outputFileName = argv[3];

  FILE *genlibFile, *genlibFile2, *coverFile, *outFile;


  if ((genlibFile = fopen(genlibFileName, "r")) == NULL) {
    printf("Unable to open file %s\n", genlibFileName);
    return -1;
  }

  if ((genlibFile2 = fopen(genlibFileName, "r")) == NULL) {
    printf("Unable to open file %s\n", genlibFileName);
    return -1;
  }
  
  if ((coverFile = fopen(coverFileName, "r")) == NULL) {
    printf("Unable to open file %s\n", coverFileName);
    return -1;
  }

  if ((outFile = fopen(outputFileName, "w")) == NULL) {
    printf("Unable to open file %s\n", outputFileName);
    return -1;
  }

  //HEADING OF THE TABLE
  fprintf(outFile, "<table style=\"text-align: left; width: 1281px; height: 140px;\"");
  fprintf(outFile, "border=\"1\" cellpadding=\"2\" cellspacing=\"2\">");
  fprintf(outFile, "<tbody>");
  fprintf(outFile, "<tr>");
  fprintf(outFile, "<td style=\"vertical-align: top;\">Genome Name<br>");
  fprintf(outFile, "</td>");
  fprintf(outFile, "<td>");
  fprintf(outFile, "<table style=\"text-align: left; width: 450px;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">");
  fprintf(outFile, "<tbody>");
  fprintf(outFile, "<tr>");
  fprintf(outFile, "<td style=\"vertical-align: top; width: 250px\">Library Name</td>");
  fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\">Library Sequence Coverage</td>");
  fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\">DOC vs Known CN Correlation</td>");
  fprintf(outFile, "</tr>");
  fprintf(outFile, "</tbody>");
  fprintf(outFile, "</table>");
  fprintf(outFile, "</td>");
  fprintf(outFile, "<td style=\"vertical-align: top;\">Genome Sequence Coverage<br>");
  fprintf(outFile, "</td>");
  fprintf(outFile, "<td style=\"vertical-align: top;\">McCarrol&nbsp; CN Estimate");
  fprintf(outFile, "Correlation Graphs<br>");
  fprintf(outFile, "</td>");
  fprintf(outFile, "<td style=\"vertical-align: top;\">Insert Size Distribution Graphs<br>");
  fprintf(outFile, "</td>");
  fprintf(outFile, "</tr>");


  char** coverArray = (char**)malloc(1000 * sizeof(char*));
  int b;
  for (b=0; b<1000; b++)
    coverArray[b] = (char*)malloc(50 * sizeof(char));

  b = 0;
  //read sequence coverage file
  char lineseq[100];
  while (fgets(lineseq, 1000, coverFile) != NULL) {
    strtok(lineseq, "\t");
    char temp[50];
    strcpy(temp, strtok(NULL, "\n")); //genome name
    strncpy(coverArray[b], temp, strlen(temp)-5);
    b++;

    fgets(lineseq, 1000, coverFile); //total seqs
    fgets(lineseq, 1000, coverFile); //A
    fgets(lineseq, 1000, coverFile); //C
    fgets(lineseq, 1000, coverFile); //G
    fgets(lineseq, 1000, coverFile); //T
    fgets(lineseq, 1000, coverFile); //N
    
    fgets(lineseq, 1000, coverFile);
    strtok(lineseq, "\t");
    strcpy(coverArray[b], strtok(NULL, "\n"));  //seq coverage
    b++;    

    printf("%s, %s\n", coverArray[b-2], coverArray[b-1]);
  }

  //read the file
  //NA18974	2485444798	0.942922
  int k;
  char genomeName[30];
  char libName[100];
  char correl[10];
  char coverage[10];
  double genomeCoverage;

  char genomeName2[30];
  char libName2[100];
  char correl2[10];

  char line2[1000];

  fgets(line2, 1000, genlibFile2);  //go one line ahead
  
  char line[1000];
  while (fgets(line, 1000, genlibFile) != NULL) {
    strcpy(genomeName, strtok(line, "\t")); //genome name
    strcpy(libName, strtok(NULL, "\t")); //library name
    strcpy(correl, strtok(NULL, "\n"));  //correlation
    genomeCoverage = 0;

    //find the seq coverage of the library
    int g;
    for (g=0; g<1000; g++) {
      if(strcmp(libName, coverArray[g])==0) {
        strcpy(coverage, coverArray[g+1]);
        genomeCoverage += atof(coverArray[g+1]);
        break;
      }
    }

    fprintf(outFile, "<tr>");
    fprintf(outFile, "<td style=\"vertical-align: top;\">%s<br>", genomeName);
    fprintf(outFile, "</td>");

    if (fgets(line2, 1000, genlibFile2) != NULL) {  //go one line ahead
      strcpy(genomeName2, strtok(line2, "\t"));
      strcpy(libName2, strtok(NULL, "\t")); //library name
      strcpy(correl2, strtok(NULL, "\n"));  //correlation
    }
    else
      strcpy(genomeName2, "");

    if (strcmp(genomeName, genomeName2) == 0) {
      fprintf(outFile, "<td>");
      fprintf(outFile, "<table style=\"text-align: left; width: 450px;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">");
      fprintf(outFile, "<tbody>");
      while (strcmp(genomeName, genomeName2) == 0) {//genome continues
        fprintf(outFile, "<tr>");
        fprintf(outFile, "<td style=\"vertical-align: top; width: 250px\">%s</td>", libName);
        fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\">%s</td>", coverage);
        fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\"><a href=\"pictures/1000genome/%s.eps.pdf\">%s</a></td>", libName, correl);
        fprintf(outFile, "</tr>");
     
        if (fgets(line, 1000, genlibFile) != NULL) {
          strcpy(genomeName, strtok(line, "\t")); //genome name
          strcpy(libName, strtok(NULL, "\t")); //library name
          strcpy(correl, strtok(NULL, "\n"));  //correlation

          for (g=0; g<1000; g++) {
            if(strcmp(libName, coverArray[g])==0) {
              strcpy(coverage, coverArray[g+1]);
              genomeCoverage += atof(coverArray[g+1]);
              break;
            }
          }
        }
        else
          break;

        if (fgets(line2, 1000, genlibFile2) != NULL) {  //go one line ahead with the other pointer
          strcpy(genomeName2, strtok(line2, "\t"));
          strcpy(libName2, strtok(NULL, "\t")); //library name
          strcpy(correl2, strtok(NULL, "\n"));  //correlation
        }
        else
          strcpy(genomeName2, "");
      }
      fprintf(outFile, "<tr>");
      fprintf(outFile, "<td style=\"vertical-align: top; width: 250px\">%s</td>", libName);
      fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\">%s</td>", coverage);
      fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\"><a href=\"pictures/1000genome/%s.eps.pdf\">%s</a></td>", libName, correl);
      fprintf(outFile, "</tr>");
      fprintf(outFile, "</tbody>");
      fprintf(outFile, "</table>");
      fprintf(outFile, "</td>"); 
    }
    else {//genome only one lib
      fprintf(outFile, "<td style=\"vertical-align: top;\">");
      fprintf(outFile, "<table style=\"text-align: left; width: 450px;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">");
      fprintf(outFile, "<tbody>");
      fprintf(outFile, "<tr>");
      fprintf(outFile, "<td style=\"vertical-align: top; width: 250px\">%s</td>", libName);
      fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\">%s</td>", coverage);
      fprintf(outFile, "<td style=\"vertical-align: top; width: 100px\"><a href=\"pictures/1000genome/%s.eps.pdf\">%s</a></td>", libName, correl);
      fprintf(outFile, "</tr>");
      fprintf(outFile, "</tbody>");
      fprintf(outFile, "</table>");
      fprintf(outFile, "</td>"); 
    }

    //print genome sequence coverage
    fprintf(outFile, "<td style=\"vertical-align: top;\">%lf<br>", genomeCoverage);
    fprintf(outFile, "</td>");
  
    fprintf(outFile, "<td style=\"vertical-align: top;\">Coming up<br>");
    fprintf(outFile, "</td>");
    fprintf(outFile, "<td style=\"vertical-align: top;\">Coming up<br>");
    fprintf(outFile, "</td>");
    fprintf(outFile, "</tr>");
  }

  fprintf(outFile, "</tbody>");
  fprintf(outFile, "</table>");
}

