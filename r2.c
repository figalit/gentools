/*
  
andy stuff

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_HORS 188
#define MAX_CHR 30
#define TRUE 1
#define FALSE 0


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
  double *minsize;
  double *maxsize;
  double min, max;
  int nhor, narray;
  int i,j;
  int myfreq;
  char family[100];
  double sst;
  double sse;
  double mean;
  double sum;
  double rsq;
  double a, b;

  char infile[100];

  double *x, *y;

  double xv, yv;
  int count;
  int PNG=0;

  //struct array *lengths;


  /*
  if (argc != 4){
    fprintf(stderr,"Given a plotfile, a histogram file and a min/max array size file\nreads the frequencies; merges the same array names\nsorts wrt to the estimated array sizes dumps 2 plots.\nUpdate on : Dec 1, 2005.\n");
    fprintf(stderr, "\nUsage: %s [plotfile] [histogramfile] [arraysize-estimates-file]\n", argv[0]);
    return 0;
    }*/

  infile[0]=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-png"))
      PNG=1;
  }

  if (infile[0]==0){
    fprintf(stderr, "No input file given!.\n");
    return 0;
  }

  histogram = fopen(infile, "r");

  if (histogram==NULL){
    fprintf(stderr, "Unable to open file %s.\n", infile);
    return 0;
  }
  

  count = 0;
  while (fscanf(histogram, "%lf\t%lf\n", &xv, &yv) > 0) count++;

  fprintf(stderr, "%d lines\n", count);

  rewind(histogram);
  
  count++;
  x = (double *) malloc(count * sizeof(double));
  y = (double *) malloc(count * sizeof(double));
  count--;

  i=0;

  while(fscanf(histogram, "%lf\t%lf\n", &(x[i]), &y[i++]) > 0)
    ;
  

  sum = 0;
  
  fclose(histogram);

  sprintf(line, "%s.plot", infile);
  minPlot = fopen(line, "w");
  
  if (!PNG){
    fprintf(minPlot, "set terminal postscript eps enhanced color\n");
    fprintf(minPlot, "set output \"%s.eps\"\n", infile);
  }
  else{
    fprintf(minPlot, "set terminal png\n");
    fprintf(minPlot, "set output \"%s.png\"\n", infile);
  }

  fprintf(minPlot, "f(x)=a*x+b\n");
  fprintf(minPlot, "fit f(x) \"%s\" via a,b\n", infile);
  fclose(minPlot);
  sprintf(line, "gnuplot %s.plot", infile);
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
  
  

  /*

  pseudocode from wikipedia

  sum_sq_x = 0
  sum_sq_y = 0
  sum_coproduct = 0
  mean_x = x[1]
  mean_y = y[1]
  for i in 2 to N:
    sweep = (i - 1.0) / i
    delta_x = x[i] - mean_x
    delta_y = y[i] - mean_y
    sum_sq_x += delta_x * delta_x * sweep
    sum_sq_y += delta_y * delta_y * sweep
    sum_coproduct += delta_x * delta_y * sweep
    mean_x += delta_x / i
    mean_y += delta_y / i 

  pop_sd_x = sqrt( sum_sq_x / N )
  pop_sd_y = sqrt( sum_sq_y / N )
  cov_x_y = sum_coproduct / N
  correlation = cov_x_y / (pop_sd_x * pop_sd_y)


  */

  

  //C-ified wikipedia 

  /* PEARSON R; doesn't give the same value with excel/openoffice */

  double sum_sq_x, sum_sq_y;
  double sum_coproduct, mean_x, mean_y;
  double sweep, delta_x, delta_y;
  double pop_sd_x, pop_sd_y, cov_x_y, correlation;
  
  sum_sq_x = 0;
  sum_sq_y = 0;
  sum_coproduct = 0;
  mean_x = x[0];
  mean_y = y[0];

  for (i=1; i<count; i++){
    sweep = (i - 1.0) / i;
    delta_x = x[i] - mean_x;
    delta_y = y[i] - mean_y;
    sum_sq_x += delta_x * delta_x * sweep;
    sum_sq_y += delta_y * delta_y * sweep;
    sum_coproduct += delta_x * delta_y * sweep;
    mean_x += delta_x / i;
    mean_y += delta_y / i ;
  }

  pop_sd_x = sqrt( sum_sq_x / count );
  pop_sd_y = sqrt( sum_sq_y / count );
  cov_x_y = sum_coproduct / count;
  correlation = cov_x_y / (pop_sd_x * pop_sd_y);

  
  printf("PEARSON1: %lf\n", correlation);

  /* PEARSON R v2 */

  double ssx, ssy;
  double sumx, sumy;

  sumx = 0.0; sumy=0.0; ssx=0.0; ssy=0.0; sst=0.0;

  for (i=0;i<count;i++){
    sst += x[i] * y[i];
    ssx += x[i] * x[i];
    ssy += y[i] * y[i];
    sumx += x[i];
    sumy += y[i];
   
  }

  double num, denum;

//  double correlation;

  num = (count * sst) - sumx*sumy;
  denum = sqrt((count * ssx) - (sumx * sumx)) *  sqrt((count * ssy) - (sumy * sumy))  ;

  correlation = num / denum;

  printf("PEARSON2: %lf\n", correlation);


  /* the R calculations above do not seem to be concodant with other tools */


  /* R-square, concordant with gnumeric / ooo / excel; we'll use this one */

  
     
  j=0;
  sum = 0.0;
  sse = 0.0;
  sst = 0.0;
  
  for (i=0;i<count;i++){
      sum += y[i];
  }  

  mean = sum/(double)count;


  for (i=0;i<count;i++){
  yv = (b + a*(int)(x[i])) - mean;
  sse = sse + yv*yv;
  yv = (double)y[i] - mean;
  sst = sst + yv*yv;
  }
  
  rsq = sse/sst;

  correlation = sqrt(rsq);

  printf("CORRELATION2: %lf\n", rsq);


  sprintf(line, "%s.plot", infile);
  minPlot = fopen(line, "w");


  if (!PNG){
    fprintf(minPlot, "set terminal postscript eps enhanced color\n");
    fprintf(minPlot, "set output \"%s.eps\"\n", infile);
  }
  else{
    fprintf(minPlot, "set terminal png\n");
    fprintf(minPlot, "set output \"%s.png\"\n", infile);
  }

  fprintf(minPlot, "f(x)=%lf*x+%lf\n", a, b);
  //fprintf(minPlot, "fit f(x) \"%s\" via a,b\n", infile);
  fprintf(minPlot, "set title \"Correlation %s ; R = %lf, R^2 = %lf\"\n", infile, correlation, rsq);
  fprintf(minPlot, "plot \"%s\" with points pt 7 title \"\", f(x) title \"fit\"\n", infile);

  fflush(minPlot);fclose(minPlot);

  sprintf(line, "gnuplot %s.plot", infile);
  system(line);
  remove("fit.log");


  return 1;
}
