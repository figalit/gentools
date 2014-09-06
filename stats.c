#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static int maxfunc(const void *, const void *);

int main(int argc, char **argv){
  FILE *in;
  float *numbers;
  float *xnumbers;
  int nline;
  float number, xnumber;
  int i,j;
  float min, max, xmin, xmax;
  float median;
  double mean, total;
  float stdev;
  float swap;
  float deviation;
  char fname[100];
  char infname[100];
  int RSQUARED;
  int REVERT;
  float a,b;
  float sse, sst, rsq;
  float yprime;
  int current, cnt;
  int png=0;
  int ps=0;
  int title=0;

  if(argc < 3){
    fprintf(stderr, "%s -i [infile] <options>\n", argv[0]);
    return 0;
  }


  infname[0]=0;
  
  a=0.0; b=0.0;
  RSQUARED = 0;
  REVERT = 0;

  for (i=0; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infname, argv[i+1]);
    else if (!strcmp(argv[i], "-r2"))
      RSQUARED = 1;
    else if (!strcmp(argv[i], "-a"))
      a = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-b"))
      b = atof(argv[i+1]);
    else if (!strcmp(argv[i], "-revert"))
      REVERT = 1;   
    else if (!strcmp(argv[i], "-png"))
      png = 1;   
    else if (!strcmp(argv[i], "-ps"))
      ps = 1;   
    else if (!strcmp(argv[i], "-title"))
      title = 1;       
  }


  in = fopen(infname, "r");
  if (in == NULL)
    return 0;

  nline = 0;
  while(fscanf(in, "%f", &number)>0) nline++;

  printf("Loading %d numbers\n", nline);

  rewind(in);


  if (RSQUARED){
  
    nline = nline/2+1;
    numbers = (float *) malloc(nline * sizeof(float));
    xnumbers = (float *) malloc(nline * sizeof(float));
    
    min = INT_MAX;
    max = INT_MIN;
    xmin = INT_MAX;
    xmax = INT_MIN;
    total = 0.0;
    i=0;
    while(fscanf(in, "%f%f", &xnumber, &number)>0) {
      xnumbers[i] = xnumber;
      numbers[i++] = number;
      //printf("%f\t%f\n", xnumber, number);
      if (xnumber>xmax) xmax = xnumber;
      if (xnumber<xmin) xmin = xnumber;
      if (number>max) max = xnumber;
      if (number<min) min = xnumber;
      total += number; 
    }
    nline = i;
    fclose(in);

    mean = total/(double)nline;

    printf("Loaded %d numbers\n", nline);


    sse = 0.0; sst = 0;

    for (i=0;i<nline;i++){
      yprime = (b + a*xnumbers[i]) - mean;
      sse = sse + yprime*yprime;
      yprime = numbers[i] - mean;
      sst = sst + yprime*yprime;
    }
    
    rsq = sse/sst;

    if (rsq>1)
      rsq = 1.0;
    
    
    printf("Rsquare:\t%f\nR:\t%f\n", rsq, sqrt(rsq));
  
    return 1;
  }

  if ( !RSQUARED ) {

    numbers = (float *) malloc(nline * sizeof(float));
    i = 0;
    while(fscanf(in, "%f", &number)>0) numbers[i++] = number;
    fclose(in);
    printf("Loaded %d numbers\n", nline);
    
    
    min = INT_MAX;
    max = INT_MIN;
    total = 0;
    
    /* 
   for (i=0;i<nline;i++){
      for (j=0;j<nline;j++){
	if (i<j && numbers[i]>numbers[j]){
	  swap = numbers[i];
	  numbers[i] = numbers[j];
	  numbers[j] = swap;
	}
	
      }
    }
    */

    qsort(numbers, nline, sizeof(float), maxfunc);
    
    median = numbers[nline/2];
    
    for (i=0;i<nline;i++){
      if (numbers[i] < min)
	min = numbers[i];
      if (numbers[i] > max)
	max = numbers[i];
      total += numbers[i];
      
    }
    
    mean = total/nline;
    
    deviation = 0;
    for (i=0;i<nline;i++)
      deviation += (numbers[i]-mean)*(numbers[i]-mean);
    
    stdev = sqrt(deviation/nline);

    sprintf(fname,"%s.gnuplot", infname);
    in = fopen(fname, "w");
  
    if (!REVERT){
      fprintf(in, "set xlabel \"xlabel\"\n");
      fprintf(in, "set ylabel \"ylabel\"\n"); 
      fprintf(in, "set title \'\'\n");
    }
    else{
      fprintf(in, "set xlabel \"span\"\n");
      fprintf(in, "set ylabel \"number of pairs\"\n"); 
      if (!title)
	fprintf(in, "set title \'Span Histogram\'\n");
      else
	fprintf(in, "set title \'Span Histogram: %s\'\n", infname);
    }

    fprintf(in, "set label \'Statistics:\' at 22000, 5700\n");
    fprintf(in, "set label \'Mean:   %5.2f\' at 22000, 5500\n", mean);
    fprintf(in, "set label \'Median: %5.2f\' at 22000, 5300\n", median);
    fprintf(in, "set label \'Stdev:  %5.2f\'at 22000, 4900\n", stdev);
    fprintf(in, "set label \'Min:    %5.0f\'at 22000, 4700\n", min);
    fprintf(in, "set label \'Max:    %5.0f\'at 22000, 4500\n", max);
    

    if (png){
      fprintf(in, "set terminal png\n");
      fprintf(in, "set output \"%s.png\"\n", infname);
    }
    else if (ps){
      fprintf(in, "set terminal postscript color\n");
      fprintf(in, "set output \"%s.ps\"\n", infname);
    }
    else{
      fprintf(in, "set terminal postscript eps enhanced color\n");
      fprintf(in, "set output \"%s.eps\"\n", infname);
    }
    if (!REVERT)
      fprintf(in, "plot \"%s\" with lines\n", infname);
    else
      fprintf(in, "plot \"%s.reverted\" with boxes ti \"span\"\n", infname);
    fclose(in);

    if (REVERT){
      sprintf(fname,"%s.reverted", infname);
      FILE *reverted = fopen(fname, "w");
      current = -1819289;
      cnt = 1;
      for (i=0;i<nline;i++){
	if ((int)(numbers[i]) == (int)current)
	  cnt++;
	else{
	  if (current != -1819289)
	    fprintf(reverted, "%d\t%d\n", current,cnt);
	  cnt = 1;
	  current = numbers[i];
	}
      }
      fclose(reverted);
    }


  }
  
}

static int maxfunc(const void *p1, const void *p2){
  int a, b;
  a = (int)(*((int *)p1));
  b = (int)(*((int *)p2));
  return (a-b);
}
