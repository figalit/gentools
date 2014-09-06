
/*
  TODO:
  
  PREFER CHROMOSOMALS over TRANS


*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TRANSDIST 99999999


/* for some reason I pick inversions.
   ask Evan if it's an OK idea
   check MCF-7 old & new predictions
*/
#define CONCORDANT "1"
#define INVERSION "3"
#define DISCORDANT "3"


int main(int argc, char **argv){
  // for now, stdin, stdout
  char name[100];
  char fwd[100];
  int fcstart;
  int fcend;
  int fwstart;
  int fwend;
  int fwin;
  char forient[3];
  char rev[100];
  int rcstart;
  int rcend;
  int rwstart;
  int rwend;
  int rwin;
  char rorient[3];
  char chr_f[15];
  char chr_r[15];
  char chr_clone[15];
  int dist;
  int twin;
  char call[100];
  FILE *temp = fopen("temp","w");
  int absdist;
  char flag;
  char clonestart[50], cloneend[50];
  FILE *in;
  FILE *out;

  int count;
  char s_dist[100]; // string version of dist
  char s_dist2[100]; // string version of dist
  int expectsize;

  /* prev */
  char prev[100];
  char fwd2[100];
  int fcstart2;
  int fcend2;
  int fwstart2;
  int fwend2;
  int fwin2;
  char forient2[3];
  char rev2[100];
  int rcstart2;
  int rcend2;
  int rwstart2;
  int rwend2;
  int rwin2;
  char rorient2[3];
  char chr_f2[15];
  char chr_r2[15];
  int dist2;
  int twin2;
  char call2[100];
  char clonestart2[50], cloneend2[50];
  char chr_clone2[15];
  /* end prev */

  char infile[100];
  char outfile[100];
  int i;
  int newhigh, newlow;
  int formatUpdate = 0;
  int recall = 0;
  int stdev;

  infile[0] = 0;
  outfile[0] = 0;
  newhigh = -1; newlow = -1;
  expectsize = 0;
  stdev = 0;

  if (argc < 5){
    printf("%s -i [pfastmatch] -o [sortedout] <-e [expectsize] -s [stdev]> <-h [newhigh] -l [newlow]>\n", argv[0]);
    exit(0);
  }


  for (i=1; i<argc; i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(infile, argv[i+1]);
    else if (!strcmp(argv[i], "-o"))
      strcpy(outfile, argv[i+1]);
    else if (!strcmp(argv[i], "-h"))
      newhigh = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-l"))
      newlow = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-e"))
      expectsize = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-s"))
      stdev = atoi(argv[i+1]);
    else if (!strcmp(argv[i], "-f"))
      formatUpdate = 1;
    /*else if (!strcmp(argv[i], "-f"))
      formatUpdate = 1;*/
  }
  


  if (infile[0] == 0){
    printf("I need an infile\n");
    exit(0);
  }
  
  if (outfile[0] == 0){
    printf("I need an outfile\n");
    exit(0);
  }

  in = fopen(infile, "r");
  out = fopen(outfile, "w");

  printf("newhigh %d\nnewlow %d\n", newhigh, newlow);

  if (formatUpdate){
    while (fscanf(in, "%s %s %d %d %d %d %d %s %s %d %d %d %d %d %s %s %s %d %s %s %s %s",
		  name, fwd, &fcstart, &fcend, &fwstart, &fwend, &fwin, forient, 
		  rev, &rcstart, &rcend, &rwstart, &rwend, &rwin, rorient,
		  chr_f, s_dist, &twin, call, chr_r, clonestart, cloneend) > 0){


      if (!strcmp(chr_f, chr_r))
	strcpy(chr_clone, chr_f);
      else{
	strcpy(chr_clone, "NA");
	strcpy(clonestart, "NA");
	strcpy(cloneend, "NA");
	strcpy(s_dist, "NA");
      }

      if(!strcmp(rorient, "R")) strcpy(rorient, "F");
      else strcpy(rorient, "R");

      if (!strcmp(call, "concordant")) strcpy(call, "inversion");
      else if (!strcmp(call, "inversion")) strcpy(call, "concordant");
      else if (!strcmp(call, "delinv")) strcpy(call, "deletion");
      else if (!strcmp(call, "insinv")) strcpy(call, "insertion");
      else if (!strcmp(call, "deletion")) strcpy(call, "delinv");
      else if (!strcmp(call, "insertion")) strcpy(call, "insinv");
      
      fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%s\t%s\t%s\t%s\n",
	      name, fwd, chr_f, fcstart, fcend, fwstart, fwend, fwin, forient, 
	      rev, chr_r, rcstart, rcend, rwstart, rwend, rwin, rorient, 
	      s_dist, twin, call, chr_clone, clonestart, cloneend);
      
    }

    exit(0);
  }


  if (newhigh != -1 || newlow != -1){
    printf("newhigh %d\nnewlow %d\n", newhigh, newlow);
    expectsize = (newhigh+newlow)/2;
    stdev = (newhigh-expectsize) / 3;

    while (fscanf(in, "%s %s %s %d %d %d %d %d %s %s %s %d %d %d %d %d %s %s %d %s %s %s %s",
		  name, fwd, chr_f, &fcstart, &fcend, &fwstart, &fwend, &fwin, forient, 
		  rev, chr_r, &rcstart, &rcend, &rwstart, &rwend, &rwin, rorient, 
		  s_dist, &twin, call, chr_clone, clonestart, cloneend) > 0){

      if (strcmp(s_dist, "NA"))
	dist = atoi(s_dist);    
      else
	dist = TRANSDIST;
      
      if (dist == TRANSDIST)
	strcpy(call, "transchromosomal"); // for compatibility
      else if (!strcmp(call, "wrongorient"))
	  ; // do nothing, leave it be
      else if (dist<=newhigh && dist>=newlow){
	if (strcmp(forient, rorient)) // different orientation
	  strcpy(call, "concordant"); 	  
	else
	  strcpy(call, "inversion");  // same orientation
      }
      else if (dist > newhigh){ // deletion
	if (strcmp(forient, rorient)) // different orientation
	  strcpy(call, "deletion"); 	  
	else
	  strcpy(call, "delinv");  // same orientation
      }
      else if (dist < newlow){ // insertion
	if (strcmp(forient, rorient)) // different orientation
	  strcpy(call, "insertion"); 	  
	else
	  strcpy(call, "insinv");  // same orientation
      }
      else{
	printf("dist %d, newhigh %d newlow %d\n", dist, newhigh, newlow);
	assert(0);
      }
      
      fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n",
	      name, fwd, chr_f, fcstart, fcend, fwstart, fwend, fwin, forient, 
	      rev, chr_r, rcstart, rcend, rwstart, rwend, rwin, rorient, 
	      dist, twin, call, chr_clone, clonestart, cloneend);
      
    }
    fclose(out);
    exit(0); // for now
  }


  if (expectsize == 0 || stdev == 0){
    printf("Expected insert size & stdev needed.\n");
    exit(0);
  }

  while (fscanf(in, "%s %s %s %d %d %d %d %d %s %s %s %d %d %d %d %d %s %s %d %s %s %s %s",
		name, fwd, chr_f, &fcstart, &fcend, &fwstart, &fwend, &fwin, forient, 
		rev, chr_r, &rcstart, &rcend, &rwstart, &rwend, &rwin, rorient, 
		s_dist, &twin, call, chr_clone, clonestart, cloneend) > 0){
    

    if (strcmp(s_dist, "NA"))
      dist = atoi(s_dist);    
    else
      dist = TRANSDIST;

    absdist = abs(dist - expectsize);

    if (!strcmp(call, "concordant"))
      strcpy(call, CONCORDANT);
    else if (!strcmp(call, "inversion"))
     strcpy(call, INVERSION);
    else
     strcpy(call, DISCORDANT);

    if (dist>expectsize)
      fprintf(temp, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%s\t%s\t%s\t+\n",
	      name, fwd, chr_f, fcstart, fcend, fwstart, fwend, fwin, forient, 
	      rev, chr_r, rcstart, rcend, rwstart, rwend, rwin, rorient, 
	      absdist, twin, call, chr_clone, clonestart, cloneend);
    else 
      fprintf(temp, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%s\t%s\t%s\t-\n",
	      name, fwd, chr_f, fcstart, fcend, fwstart, fwend, fwin, forient, 
	      rev, chr_r, rcstart, rcend, rwstart, rwend, rwin, rorient, 
	      absdist, twin, call, chr_clone, clonestart, cloneend);

  }

  fclose(temp);
  fprintf(stdout,"sorting\n");
  /*19 is number of total windows, 18 is the |distance-10K|, 20 is the call*/
  system("sort -k 1,1 -k 20,20n -k 19,19nr -k 18,18n temp > sorted");
  remove("temp");
  temp = fopen("sorted", "r");
  prev[0] = 0;
  count = 0;



  if (newhigh == -1){
    newhigh = expectsize+3*stdev;
    newlow = expectsize-3*stdev;
  }

  fprintf(stdout,"writing output\n");
  while (fscanf(temp, "%s %s %s %d %d %d %d %d %s %s %s %d %d %d %d %d %s %s %d %s %s %s %s %c",
		name, fwd, chr_f, &fcstart, &fcend, &fwstart, &fwend, &fwin, forient, 
		rev, chr_r, &rcstart, &rcend, &rwstart, &rwend, &rwin, rorient, 
		s_dist, &twin, call, chr_clone, clonestart, cloneend, &flag) > 0){
    
    dist = atoi(s_dist);
    if (flag == '+')
      dist = dist + expectsize;
    else
      dist = expectsize - dist;

    if (!strcmp(call, CONCORDANT))
      strcpy(call, "concordant");
    else if (strcmp(chr_f, chr_r) && dist<newhigh && dist>newlow)
      //!strcmp(call, INVERSION))  HEREINV
      strcpy(call, "inversion");
    else{
      if (dist == TRANSDIST)
	strcpy(call, "transchromosomal"); // for compatibility
      /*
      else if (!strcmp(call, "wrongorient"))
	; // do nothing, leave it be
      */
      else if (!strcmp(forient, "F") && !strcmp(rorient, "R") && rcstart<fcstart)
	strcpy(call, "wrongorient");
      else if (!strcmp(forient, "R") && !strcmp(rorient, "F") && rcstart>fcstart)
	strcpy(call, "wrongorient");
      else if (dist<=newhigh && dist>=newlow){
	if (strcmp(forient, rorient)) // different orientation
	  strcpy(call, "concordant"); 	  
	else
	  strcpy(call, "inversion");  // same orientation
      }
      else if (dist > newhigh){ // deletion
	if (strcmp(forient, rorient)) // different orientation
	  strcpy(call, "deletion"); 	  
	else
	  strcpy(call, "delinv");  // same orientation
      }
      else if (dist < newlow){ // insertion
	if (strcmp(forient, rorient)) // different orientation
	  strcpy(call, "insertion"); 	  
	else
	  strcpy(call, "insinv");  // same orientation
      }
      else{
	printf("dist %d, newhigh %d newlow %d\n", dist, newhigh, newlow);
	assert(0);
      }    
    }

    if (strcmp(prev, name)) { // new entry
      // if concordant by size, print the first guy
      if (!strcmp(call, "concordant")){  // || !strcmp(call, "inversion")){ HEREINV
	if (count == 1){
	  if (dist2 != TRANSDIST)
	    fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n",
		    prev, fwd2, chr_f2, fcstart2, fcend2, fwstart2, fwend2, fwin2, forient2, 
		    rev2, chr_r2, rcstart2, rcend2, rwstart2, rwend2, rwin2, rorient2, 
		    dist2, twin2, call2, chr_clone2, clonestart2, cloneend2);
	  else
	    fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%s\t%s\t%s\t%s\n",
		    prev, fwd2, chr_f2, fcstart2, fcend2, fwstart2, fwend2, fwin2, forient2, 
		    rev2, chr_r2, rcstart2, rcend2, rwstart2, rwend2, rwin2, rorient2, 
		    "NA", twin2, call2, chr_clone2, clonestart2, cloneend2);
	    
	}
	// this is either conc or inv; so dist cannot be NA
	fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n",
		name, fwd, chr_f, fcstart, fcend, fwstart, fwend, fwin, forient, 
		rev, chr_r, rcstart, rcend, rwstart, rwend, rwin, rorient, 
		dist, twin, call, chr_clone, clonestart, cloneend);
	count = 2; //bypass any unwanted duplications
      }
      else { // discordant by size or transchromosomal
	// get unique guys.
	// keep this info first

	if (count == 1){
	  if (dist2 != TRANSDIST)
	    fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n",
		    prev, fwd2, chr_f2, fcstart2, fcend2, fwstart2, fwend2, fwin2, forient2, 
		    rev2, chr_r2, rcstart2, rcend2, rwstart2, rwend2, rwin2, rorient2, 
		    dist2, twin2, call2, chr_clone2, clonestart2, cloneend2);
	  else
	    fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%s\t%s\t%s\t%s\n",
		    prev, fwd2, chr_f2, fcstart2, fcend2, fwstart2, fwend2, fwin2, forient2, 
		    rev2, chr_r2, rcstart2, rcend2, rwstart2, rwend2, rwin2, rorient2, 
		    "NA", twin2, call2, chr_clone2, clonestart2, cloneend2);
	}

	//strcpy(prev, name); 

	strcpy(fwd2, fwd);
	fcstart2 = fcstart; fcend2 = fcend;
	fwstart2 = fwstart; fwend2 = fwend; 
	fwin2 =  fwin; strcpy(forient2, forient);

	strcpy(rev2, rev);
	rcstart2 = rcstart; rcend2 = rcend;
	rwstart2 = rwstart; rwend2 = rwend; 
	rwin2 =  rwin; strcpy(rorient2, rorient);
	
	dist2 = dist;
	twin2 = twin;
	
	strcpy(chr_f2, chr_f);
	strcpy(chr_r2, chr_r);
	strcpy(chr_clone2, chr_clone);
	strcpy(clonestart2,clonestart);
	strcpy(cloneend2,cloneend);
	strcpy(call2, call);
	count = 1;
      }

      strcpy(prev, name); // finally remember this one as prev
    }
    else { // repeated entry
      // do nothing here, just remember this is not unique
      // if this is concordant by size, we already printed the first that comes along
      // if not, then this is not a unique discordant, so who cares about this one ?
      count++;
    } 
  }
  fclose(temp);
  remove("sorted");
  
}
