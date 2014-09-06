/*
  Given a plotfile, a histogram file
  and a min/max array size file
  reads the frequencies; merges the same array names
  sorts wrt to the estimated array sizes
  dumps 2 plots

  Update on : Dec 1, 2005

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_HORS 188
#define MAX_CHR 30
#define TRUE 1
#define FALSE 0

typedef int boolean;

/*
struct array{
  char name[100];
  float min;
  float max;
};
*/

int main(int argc, char **argv){

  FILE *plotfile;
  FILE *histogram;
  FILE *asize;
  FILE *minHist;
  FILE *maxHist;
  FILE *minPlot;
  FILE *maxPlot;
  FILE *fit;

  char line[1000];
  char **names;
  int *freqs;
  float *minsize;
  float *maxsize;
  float min, max;
  int nhor, narray;
  int i,j;
  int myfreq;
  char family[100];
  double sst;
  double sse;
  float mean;
  double sum;
  float rsq;
  double a, b, y;

  //struct array *lengths;

  boolean readflag;

  if (argc != 4){
    fprintf(stderr,"Given a plotfile, a histogram file and a min/max array size file\nreads the frequencies; merges the same array names\nsorts wrt to the estimated array sizes dumps 2 plots.\nUpdate on : Dec 1, 2005.\n");
    fprintf(stderr, "\nUsage: %s [plotfile] [histogramfile] [arraysize-estimates-file]\n", argv[0]);
    return 0;
  }

  plotfile  = fopen(argv[1], "r");
  histogram = fopen(argv[2], "r");
  asize     = fopen(argv[3], "r");

  if (plotfile==NULL || histogram==NULL || asize==NULL){
    fprintf(stderr, "Unable to open file(s).\n");
    return 0;
  }
  

  freqs = (int *) malloc(MAX_HORS * sizeof(int));
  minsize = (float *) malloc(MAX_HORS * sizeof(float));
  maxsize = (float *) malloc(MAX_HORS * sizeof(float));
  names = (char **) malloc(MAX_HORS * sizeof(char *));
  
  for (i=0; i<MAX_HORS; i++){
    names[i] = (char *) malloc(100*sizeof(char));
    names[i][0] = 0;
    freqs[i] = 0;
    minsize[i] = 0.0;
    maxsize[i] = 0.0;    
  }
  
  nhor = 0;

  readflag = FALSE;

  sum = 0;

  while (1){
    fgets(line, 1000, plotfile);
    if (strstr(line, "set label"))
      readflag = TRUE; 
    else if (strstr(line, "with boxes"))
      break;
    if (readflag){
      i=0;
      while (line[i++]!='\"')
	;
      j=0;
      while (line[i]!='\"')
	names[nhor][j++] = line[i++];
      names[nhor][j]=0; 
      nhor++;
    }
  }
  
  fclose(plotfile);

  i=0;

  while(fscanf(histogram, "%d", &myfreq) > 0)
    freqs[i++]=myfreq;
  
  
  fclose(histogram);

  /*
  lengths = (struct array *) malloc(sizeof(struct array) * MAX_CHR);
  */

  narray=0;
  while (fscanf(asize, "%s %f %f", family, &min, &max) > 0){
    /*
    lengths[i].min = min;
    lengths[i].max = max;
    strcpy(lengths[i].name, family); */
    for (i=0;i<nhor;i++){
      if (strstr(names[i], family)){
	minsize[i] = min;
	maxsize[i] = max;
      }
    }
  }
  
  fclose(asize);

  /* sort wrt min */

  for (i=0;i<nhor;i++){
    for (j=i+1;j<nhor;j++){
      if (minsize[i] > minsize[j] && i<j){
	min = minsize[i];
	minsize[i] = minsize[j];
	minsize[j] = min;

	max = maxsize[i];
	maxsize[i] = maxsize[j];
	maxsize[j] = max;

	myfreq = freqs[i];
	freqs[i] = freqs[j];
	freqs[j] = myfreq;

	strcpy(family, names[i]);
	strcpy(names[i], names[j]);
	strcpy(names[j], family);

      }
    }
  }

  sprintf(line, "min-%s", argv[1]);
  minPlot = fopen(line, "w");
  sprintf(line, "min-%s", argv[2]);
  minHist = fopen(line, "w");
  
  
  fprintf(minPlot, "set terminal postscript eps enhanced color\n");
  fprintf(minPlot, "set output \"%s.eps\"\n", line);
  /*
  fprintf(minPlot, "set size 2,2\n");
  fprintf(minPlot, "set bmargin 15\nset tmargin -5\nunset xtics\n");
  */

  for (i=0;i<nhor;i++){
    if(minsize[i] != 0.0 && minsize[i] != -1.0){
      //fprintf(minPlot, "set label \"%5.2f-%s\" at first %d,0 rotate by -90 font \"Times,8\"\n",minsize[i],names[i],i);
      fprintf(minHist, "%d %d\n",(int)(minsize[i]*1000), freqs[i]);
    }
    //    else
    //fprintf(minHist, "0\n");
  }

  fflush(minHist); fclose(minHist); 
  fprintf(minPlot, "f(x)=a*x+b\n");
  fprintf(minPlot, "fit f(x) \"min-%s\" via a,b\n", argv[2]);
  fclose(minPlot);
  sprintf(line, "gnuplot min-%s", argv[1]);
  system(line);

  /* 
     get a and b here
  */

  fit = fopen("fit.log", "r");
  

  while(fscanf(fit, "%s", line) > 0){
    if (strstr(line, "========")){
      fscanf(fit, "%s", line); //this should be the same
      fscanf(fit, "%s", line); // this should be "a"
      fscanf(fit, "%s", line); // this should be =
      fscanf(fit, "%s", line); // this should be really a
      a = atof(line);
      fgets(line, 1000, fit); // skip the line
      fscanf(fit, "%s", line); // this should be "b"
      fscanf(fit, "%s", line); // this should be =
      fscanf(fit, "%s", line); // this should be really b
      b = atof(line);
      break;
    }
  }
  
  fclose(fit);
  
  

  j=0;
  sum = 0.0;
  sse = 0.0;
  sst = 0.0;

  for (i=0;i<nhor;i++){
    if(minsize[i] != 0.0 && minsize[i] != -1.0){
      sum += freqs[i];
      j++;
    }
  }  

  mean = sum/j;

  
  for (i=0;i<nhor;i++){
    if(minsize[i] != 0.0 && minsize[i] != -1.0){
      y = (b + a*(int)(minsize[i]*1000)) - mean;
      sse = sse + y*y;
      y = (double)freqs[i] - mean;
      sst = sst + y*y;
    }
  }

  rsq = sse/sst;

  sprintf(line, "min-%s", argv[1]);
  minPlot = fopen(line, "w");

  sprintf(line, "min-%s", argv[2]);
  fprintf(minPlot, "set terminal postscript eps enhanced color\n");
  fprintf(minPlot, "set output \"%s.eps\"\n", line);
  fprintf(minPlot, "f(x)=a*x+b\n");
  fprintf(minPlot, "fit f(x) \"min-%s\" via a,b\n", argv[2]);
  fprintf(minPlot, "set title \"MIN %s - %s ; R-Squared: %f\"\n", argv[1], argv[2], rsq);
  fprintf(minPlot, "plot \"min-%s\" with points, f(x) title \"linear fit\"\n", argv[2]);

  fflush(minPlot);fclose(minPlot);

  sprintf(line, "gnuplot min-%s", argv[1]);
  system(line);
  remove("fit.log");




  /* sort wrt max */

  for (i=0;i<nhor;i++){
    for (j=i+1;j<nhor;j++){
      if (maxsize[i] > maxsize[j] && i<j){
	min = minsize[i];
	minsize[i] = minsize[j];
	minsize[j] = min;

	max = maxsize[i];
	maxsize[i] = maxsize[j];
	maxsize[j] = max;

	myfreq = freqs[i];
	freqs[i] = freqs[j];
	freqs[j] = myfreq;

	strcpy(family, names[i]);
	strcpy(names[i], names[j]);
	strcpy(names[j], family);

      }
    }
  }

  sprintf(line, "max-%s", argv[1]);
  maxPlot = fopen(line, "w");
  sprintf(line, "max-%s", argv[2]);
  maxHist = fopen(line, "w");
  
  fprintf(maxPlot, "set terminal postscript eps enhanced color\n");
  fprintf(maxPlot, "set output \"%s.eps\"\n", line);
  //fprintf(maxPlot, "set size 2,2\n");
  //fprintf(maxPlot, "set bmargin 15\nset tmargin -5\nunset xtics\n");
  

  for (i=0;i<nhor;i++){
    if(maxsize[i] != 0.0 && maxsize[i] != -1.0){
      //fprintf(maxPlot, "set label \"%5.2f-%s\" at first %d,0 rotate by -90 font \"Times,8\"\n",maxsize[i],names[i],i);
      fprintf(maxHist, "%d %d\n", (int)(maxsize[i]*1000), freqs[i]);
    }
    //else
    // fprintf(maxHist, "0\n");
  }

  fflush(maxHist); fclose(maxHist); 

  fprintf(maxPlot, "f(x)=a*x+b\n");
  fprintf(maxPlot, "fit f(x) \"max-%s\" via a,b\n", argv[2]);
  remove("fit.log");
  fflush(maxPlot); fclose(maxPlot);
  sprintf(line, "gnuplot max-%s", argv[1]);
  system(line);

  /* 
     get a and b here
  */

  fit = fopen("fit.log", "r");
  
  while(fscanf(fit, "%s", line) > 0){
    if (strstr(line, "========")){
      fscanf(fit, "%s", line); //this should be the same
      fscanf(fit, "%s", line); // this should be "a"
      fscanf(fit, "%s", line); // this should be =
      fscanf(fit, "%s", line); // this should be really a
      a = atof(line);
      fgets(line, 1000, fit); // skip the line
      fscanf(fit, "%s", line); // this should be "b"
      fscanf(fit, "%s", line); // this should be =
      fscanf(fit, "%s", line); // this should be really b
      b = atof(line);
      break;
    }
  }
  
  fclose(fit);


  j=0;
  sum = 0.0;
  sse = 0.0;
  sst = 0.0;

  for (i=0;i<nhor;i++){
    if(maxsize[i] != 0.0 && maxsize[i] != -1.0){
      sum += freqs[i];
      j++;
    }
  }  

  mean = sum/j;

  
  for (i=0;i<nhor;i++){
    if(maxsize[i] != 0.0 && maxsize[i] != -1.0){
      y = (b + a*(int)(maxsize[i]*1000)) - mean;
      sse = sse + y*y;
      y = (double)freqs[i] - mean;
      sst = sst + y*y;
    }
  }

  rsq = sse/sst;

  sprintf(line, "max-%s", argv[1]);
  maxPlot = fopen(line, "w");
  sprintf(line, "max-%s", argv[2]);
  
  fprintf(maxPlot, "set terminal postscript eps enhanced color\n");
  fprintf(maxPlot, "set output \"%s.eps\"\n", line);
  fprintf(maxPlot, "set title \"MAX %s - %s ; R-Squared: %f\"\n", argv[1], argv[2], rsq);
  fprintf(maxPlot, "f(x)=a*x+b\n");
  fprintf(maxPlot, "fit f(x) \"max-%s\" via a,b\n", argv[2]);

  fprintf(maxPlot, "plot \"max-%s\" with points, f(x) title \"linear fit\"\n", argv[2]);
  fflush(maxPlot); fclose(maxPlot);

  sprintf(line, "gnuplot max-%s", argv[1]);
  system(line);
  remove("fit.log");

  sse = 0.0;
  sst = 0.0;

  
  //a = 0.121026;//jm
  //b = 208.933;//jm
  
  //a = 0.0366188; //s213
  //b = 208.454; //s213


  return 1;
}
