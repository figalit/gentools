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
  char tmpfile[100];
  FILE *tmp;

  double *x, *y;

  double xv, yv;
  int count;
  
  char bacname[30];
  int s,e;
  int female=0;

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
    else if (!strcmp(argv[i], "-y"))
      female=0;
    else if (!strcmp(argv[i], "-x"))
      female=1;
      
    else if (!strcmp(argv[i], "-M"))
      female=0;
    else if (!strcmp(argv[i], "-F"))
      female=1;
      
  }

  if (infile[0]==0){
    fprintf(stderr, "No input file given!.\n");
    return 0;
  }

  histogram = fopen(infile, "r");

  sprintf(tmpfile, "%s.tmp", infile);

  if (histogram==NULL){
    fprintf(stderr, "Unable to open file %s.\n", infile);
    return 0;
  }
  

  count = 0;
  //while (fscanf(histogram, "%lf\t%lf\n", &yv, &xv) > 0) if (xv != 500) count++;
  
  while (fscanf(histogram, "%s\t%d\t%d\t%lf\n", bacname, &s, &e,&yv) > 0) if (xv != 500)count++;

  fprintf(stderr, "%d lines\n", count);

  rewind(histogram);
  
  count++;
  x = (double *) malloc(count * sizeof(double));
  y = (double *) malloc(count * sizeof(double));
  count--;

  i=0;

  tmp = fopen(tmpfile, "w");
  while (fscanf(histogram, "%s\t%d\t%d\t%lf\n", bacname, &s, &e, &yv) > 0){
    //while (fscanf(histogram, "%lf\t%lf\n",  &yv, &xv) > 0){
    if (!strcmp(bacname, "U13369"))
      continue;
    else {
      if (!strcmp(bacname, "AC000404") ||
	  !strcmp(bacname, "AC004485") ||
	  !strcmp(bacname, "AC006316") ||
	  !strcmp(bacname, "AL021879") ||
	  !strcmp(bacname, "AL031769") ||
	  !strcmp(bacname, "AC003687") ||
	  !strcmp(bacname, "AC005516") ||
	  !strcmp(bacname, "AC007387") ||
	  !strcmp(bacname, "AL023913") ||
	  !strcmp(bacname, "AP001706") ||
	  !strcmp(bacname, "AC004057") ||
	  !strcmp(bacname, "AC006137") ||
	  !strcmp(bacname, "AC015734") ||
	  !strcmp(bacname, "AL031230") ||
	  !strcmp(bacname, "Z83307") ||
	  !strcmp(bacname, "AC004408") ||
	  !strcmp(bacname, "AC006238") ||
	  !strcmp(bacname, "AF109907") ||
	  !strcmp(bacname, "AL031587"))
	xv = 2;
      else if (!strcmp(bacname, "chr16.0")) xv = 16;
      else if (!strcmp(bacname, "chr16.1")) xv = 14;
      else if (!strcmp(bacname, "chr16.2")) xv = 6;
      else if (!strcmp(bacname, "chr16.3")) xv = 36;
      else if (!strcmp(bacname, "chr16.4")) xv = 6;
      else{
	if (female) xv = 2;
	else xv = 1;
      }
    }
      x[i] = xv;
      y[i] = yv;
      i++;
      fprintf (tmp, "%lf\t%lf\n", xv, yv);
    }


  fclose(tmp);
  //  while(fscanf(histogram, "%lf\t%lf\n", &(x[i]), &y[i++]) > 0)
  // ;
  

  sum = 0;
  
  fclose(histogram);

  sprintf(line, "%s.plot", infile);
  minPlot = fopen(line, "w");
  
  fprintf(minPlot, "set terminal png\n");
  fprintf(minPlot, "set output \"%s.png\"\n", infile);

  fprintf(minPlot, "f(x)=a*x+b\n");
  fprintf(minPlot, "fit f(x) \"%s\" via a,b\n", tmpfile);
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

  
  fprintf(stderr, "PEARSON1: %lf\n", correlation);

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

  fprintf(stderr, "PEARSON2: %lf\n", correlation);


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

  fprintf(stderr, "CORRELATION2: %lf\n", rsq);


  sprintf(line, "%s.plot", infile);
  minPlot = fopen(line, "w");


  fprintf(minPlot, "set terminal png\n");
  fprintf(minPlot, "set output \"%s.png\"\n", infile);
  fprintf(minPlot, "f(x)=%lf*x+%lf\n", a, b);
  //fprintf(minPlot, "fit f(x) \"%s\" via a,b\n", infile);
  fprintf(minPlot, "set title \"Correlation %s ; R = %lf, R^2 = %lf\"\n", infile, correlation, rsq);
  fprintf(minPlot, "plot \"%s\" with points pt 7 title \"\", f(x) title \"fit\"\n", tmpfile);

  fflush(minPlot);fclose(minPlot);

  sprintf(line, "gnuplot %s.plot", infile);
  system(line);
  remove("fit.log");

  remove(tmpfile);

  strcpy(tmpfile, infile);
  tmpfile[strlen(tmpfile)-2]=0;
  sprintf(tmpfile, "%scn", tmpfile);
  if (rsq < 0.8)
    printf("mv %s %s.plot %s.png %s reject/\n", infile, infile, infile, tmpfile);
  

  return 1;
}
