#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

/*
$color{AA} = 0; 
$color{CC} = 0; 
$color{GG} = 0; 
$color{TT} = 0; 
$color{AC} = 1; 
$color{CA} = 1; 
$color{GT} = 1; 
$color{TG} = 1; 
$color{AG} = 2; 
$color{CT} = 2; 
$color{GA} = 2; 
$color{TC} = 2; 
$color{AT} = 3; 
$color{CG} = 3; 
$color{GC} = 3; 
$color{TA} = 3; 
*/

char tocolor(char prev, char letter){

  switch(prev){
  case 'A':
    switch(letter){
    case 'A':
      return '0';
    case 'C':
      return '1';
    case 'G':
      return '2';
    case 'T':
      return '3';
    case 'N':
      return '.';
    default:
      fprintf(stderr, "boom %c\n", letter);
      exit(0);
    }
    break;
  case 'C':
    switch(letter){
    case 'A':
      return '1';
    case 'C':
      return '0';
    case 'G':
      return '3';
    case 'T':
      return '2';
    case 'N':
      return '.';
    default:
      fprintf(stderr, "boom %c\n", letter);
      exit(0);
    }
    break;  
 case 'G':
   switch(letter){
   case 'A':
     return '2';
   case 'C':
     return '3';
   case 'G':
     return '0';
   case 'T':
     return '1';
   case 'N':
     return '.';
    default:
      fprintf(stderr, "boom %c\n", letter);
      exit(0);
   }
   break;

 case 'T':
   switch(letter){
   case 'A':
     return '3';
   case 'C':
     return '2';
   case 'G':
     return '1';
   case 'T':
     return '0';
   case 'N':
     return '.';
    default:
      fprintf(stderr, "boom %c\n", letter);
      exit(0);
   }
   break;
 case 'N':
   switch(letter){
   case 'A':
     return '.';
   case 'C':
     return '.';
   case 'G':
     return '.';
   case 'T':
     return '.';
   case 'N':
     return '.';
    default:
      fprintf(stderr, "boom %c\n", letter);
      exit(0);
   }
   break;
  default:
    fprintf(stderr, "boom %c-%c\n", prev, letter);
    exit(0);
  }
}


int main(int argc, char **argv){
  char line[1000];
  char ch, pch;
  char color;
  char fname[100];
  FILE *fasta;
  FILE *csfasta;
  int ncnt;
  
  if (argc != 2){
    fprintf(stderr, "FASTA to Color Space Converter.\nUsage: %s <fasta_file>\n", argv[0]);
    return -1;
  }

  fasta = fopen(argv[1], "r");

  if (fasta == NULL){
    fprintf(stderr, "Unable to open file %s.\n", argv[1]);
    return -1;
  }

  sprintf(fname, "%s.cs", argv[1]);

  csfasta = fopen(fname, "w");

  if (csfasta == NULL){
    fprintf(stderr, "Unable to create file %s.\n", fname);
    return -1;
  }


  ch=0; pch=0;

  while (fscanf(fasta, "%c", &ch) > 0){
    if (ch=='>'){
      pch='T';
      fgets(line, 1000, fasta);
      fprintf(csfasta, ">%s%c", line, pch);
    }
    else if (isspace(ch)){
      fprintf(csfasta, "%c", ch);
    }
    else{

      ch = toupper(ch);
      
      if (pch != 0){
	color = tocolor(pch, ch);	
	fprintf(csfasta, "%c", color);
      }
      
      pch = ch;
    }
  }

  fclose(fasta); fclose(csfasta);

  return 0;
}
