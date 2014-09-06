#include <stdio.h>
#define IDENTITY 0.50

int main(int argc, char **argv){
  char name[100];
  int cstart, cend, fstart, fend, wins;
  int pwins;
  int fwins;
  int windiff;
  int thisdiff;
  char orient;
  char chr[100];
  char prev[100];
  int cnt=0;
  int i;
  FILE *in;
  FILE *out;

  prev[0] = 0;

  if (argc != 3)
    return 0;

  in = fopen(argv[1], "r");
  out = fopen(argv[2], "w");
  pwins = 0;
  windiff = 0;
  
  if (in == NULL || out == NULL)
    return 0;

  while(fscanf(in, "%s %d %d %d %d %d %c %s", name, &cstart, &cend, &fstart, &fend, 
	      &wins, &orient, chr) > 0){
    if (strcmp(name, prev)){
      strcpy(prev, name);
      pwins = wins;
      fwins = wins;
      windiff = 0;
      /* this is rank 1, so print it */
      fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", name, cstart, cend, fstart, fend, 
	      wins, orient, chr);
      cnt=1;
    }

    else{
      thisdiff = pwins - wins;
      if (windiff == 0){ // this is rank2
	if (wins > fwins*IDENTITY){ // seems okay
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", name, cstart, cend, fstart, fend, 
		  wins, orient, chr);
	  windiff = thisdiff;
	  pwins = wins;
	  cnt++;
	}
      }
      else{
	if (thisdiff < 2*windiff){
	  fprintf(out, "%10s\t\%10d\t%10d\t%6d\t%6d\t%d\t\t%c\t%s\n", name, cstart, cend, fstart, fend, wins, orient, chr);
	  cnt++;
	  windiff = thisdiff;
	  pwins = wins;
	}
      }
    }
    
    
  }


  return 1;
  
  
}
