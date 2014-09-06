#include <stdio.h>

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

char tocharacter(char prev, char color){

  switch(prev){
  case 'A':
    switch(color){
    case '0':
      return 'A';
    case '1':
      return 'C';
    case '2':
      return 'G';
    case '3':
      return 'T';
    default:
      fprintf(stderr, "boom %c\n", color);
      exit(0);
    }
    break;
  case 'C':
    switch(color){
    case '0':
      return 'C';
    case '1':
      return 'A';
    case '2':
      return 'T';
    case '3':
      return 'G';
    default:
      fprintf(stderr, "boom %c\n", color);
      exit(0);
    }
    break;  
 case 'G':
   switch(color){
   case '0':
     return 'G';
   case '1':
     return 'T';
   case '2':
     return 'A';
   case '3':
     return 'C';
    default:
      fprintf(stderr, "boom %c\n", color);
      exit(0);
   }
   break;

 case 'T':
   switch(color){
   case '0':
     return 'T';
   case '1':
     return 'G';
   case '2':
     return 'C';
   case '3':
     return 'A';
    default:
      fprintf(stderr, "boom %c\n", color);
      exit(0);
   }
   break;
  default:
    fprintf(stderr, "boom %c-%c\n", prev, color);
    exit(0);
  }
}


main(){
  char name[100];
  char seq[100];
  char line[1000];
  int len;
  int i;

  char *ch;

  while (scanf("%s\n%s", name, seq) > 0){
    
    if (name[0] == '#'){
      fgets(line, 1000, stdin);
      continue;
    }

    ch = strchr(seq, '.');

    if (ch != NULL) *ch = 0;

    printf("%s\n", name);
    len = strlen(seq);

    for (i=1; i<len; i++){
      seq[i] = tocharacter(seq[i-1], seq[i]);
    }
    
    //seq[len+1] = 0;

    printf("%s\n", (seq+1));
    
  }

}
