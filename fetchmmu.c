#include <stdio.h>

main(){
  char name[100];
  char sep[5];
  char orient[3];
  char command[1000];
  int start;
  int end;
  while (scanf("%s%s%s %d %d", name, sep, orient, &start, &end) > 0){
    sprintf(command, "fastacmd -d /home/eray/MACAQUE_FOSMIDS/MACAQUE_WGS_0506 -s \"%s\" > sequence", name);
    system(command);
    if (orient[0] == 'R'){
      sprintf(command, "clip sequence %d %d sequence.cut", end, start);
      system(command);
      system("mv sequence.cut sequence");
      system("revcomp sequence sequence.rev");
      system("mv sequence.rev sequence");
    }
    else{
      sprintf(command, "clip sequence %d %d sequence.cut", start, end);
      system(command);
      system("mv sequence.cut sequence");
    }
    system("cat sequence >> temp");
    remove("sequence");
  }
}
