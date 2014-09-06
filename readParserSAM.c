#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>

#define STRLEN 100
#define MIN 100
#define MAX 400
#define QUERY 0x0010
#define MATE 0x0020

typedef struct
{
  char *name;
  char *chrname;
  int ed_dis;
  int insert;
  int start;
  int end;
} alignment;

void getlocations(char *filename,alignment **locations,int *len){
  int i,j,k;
  FILE *alignment_file;
  int insert_size;
  char *gene;
  char *ed;
  int tmp;
  int cno;
  int median;
  char *edit;
  char **fields;
  unsigned short flag;

  alignment_file = fopen(filename,"r");
  if(alignment_file==NULL) {
    printf("Error: can't open file %s.\n",filename);
    exit(0);
  }

  fields = (char **)malloc(sizeof(char *)*8);
  for(i=0;i<8;i++)
    fields[i] = (char *)malloc(sizeof(char)*STRLEN);
 
  median = (MAX + MIN) / 2;
  cno=0;
  
  (*locations) = (alignment *)malloc(sizeof(alignment)*1);
  (*locations)[0].name = (char *)malloc(sizeof(char)*STRLEN);
  (*locations)[0].chrname = (char *)malloc(sizeof(char)*STRLEN);
  
  while(cno == 0){
    fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
    flag = (unsigned short) atoi(fields[3]);
    if ( atoi(fields[5]) < atoi(fields[6])){
      if ( (flag & QUERY) || ( !(flag & QUERY) && !(flag & MATE)) ){
	fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
	continue;
      }
    }
    if ( atoi(fields[5]) > atoi(fields[6])){
      if ( !(flag & QUERY) || ( (flag & QUERY) && (flag & MATE)) ){
	fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
	continue;
      }
    }
    break;
  }
  
  strncpy((*locations)[0].name,fields[1],STRLEN);
  strncpy((*locations)[0].chrname,fields[4],STRLEN);
  edit = strrchr(fields[6],':')+1;
  tmp = atoi(edit);
  fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
  edit = strrchr(fields[6],':')+1;
  tmp = tmp + atoi(edit);
  (*locations)[0].ed_dis = tmp;
  (*locations)[0].insert = atoi(fields[2]);
  if ( atoi(fields[5]) < atoi(fields[6]) ){
    (*locations)[0].start = atoi(fields[5]);
    (*locations)[0].end = atoi(fields[6])+63;
  }
  else{
    (*locations)[0].start = atoi(fields[6]);
    (*locations)[0].end = atoi(fields[5])+63;
  }
  
  i=1;
  while(!feof(alignment_file)){
    fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
    flag = (unsigned short) atoi(fields[3]);
    if ( atoi(fields[5]) < atoi(fields[6])){
      if ( (flag & QUERY) || ( !(flag & QUERY) && !(flag & MATE)) ){
	fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
	continue;
      }
    }
    if ( atoi(fields[5]) > atoi(fields[6])){
      if ( !(flag & QUERY) || ( (flag & QUERY) && (flag & MATE)) ){
	fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
	continue;
      }
    }
    edit = strrchr(fields[6],':')+1;
    tmp = atoi(edit);
    fscanf(alignment_file,"%s %s %s %s %s %s %s %s\n",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
    edit = strrchr(fields[6],':')+1;
    tmp = tmp + atoi(edit);
    if(!strcmp((*locations)[i-1].name,fields[1])){
      if( (*locations)[i-1].ed_dis > tmp){
	(*locations)[i-1].ed_dis = tmp;
	(*locations)[i-1].insert = atoi(fields[2]);
	if ( atoi(fields[5]) < atoi(fields[6]) ){
	  (*locations)[0].start = atoi(fields[5]);
	  (*locations)[0].end = atoi(fields[6])+63;
	}
	else{
	  (*locations)[0].start = atoi(fields[6]);
	  (*locations)[0].end = atoi(fields[5])+63;
	}
      }
      else if( tmp == (*locations)[i-1].ed_dis){
	if( abs((*locations)[i-1].insert - median) > abs( atoi(fields[2]) - median)){
	  (*locations)[i-1].insert = atoi(fields[2]);
	  if ( atoi(fields[5]) < atoi(fields[6]) ){
	    (*locations)[0].start = atoi(fields[5]);
	    (*locations)[0].end = atoi(fields[6])+63;
	  }
	  else{
	    (*locations)[0].start = atoi(fields[6]);
	    (*locations)[0].end = atoi(fields[5])+63;
	  }
	}
      }
    }
    else{
      (*locations) = (alignment *)realloc((*locations),sizeof(alignment)*(i+1));
      (*locations)[i].name = (char *)malloc(sizeof(char)*STRLEN);
      (*locations)[i].chrname = (char *)malloc(sizeof(char)*STRLEN);
      strncpy((*locations)[i].name,fields[1],STRLEN);
      strncpy((*locations)[i].chrname,fields[4],STRLEN);
      (*locations)[i].ed_dis = tmp;
      (*locations)[i].insert = atoi(fields[2]);
      if ( atoi(fields[5]) < atoi(fields[6]) ){
	(*locations)[0].start = atoi(fields[5]);
	(*locations)[0].end = atoi(fields[6])+63;
      }
      else{
	(*locations)[0].start = atoi(fields[6]);
	(*locations)[0].end = atoi(fields[5])+63;
      }
      i++;
    }
  }
  
  fclose(alignment_file);
  *len = i;
}

int main(int argc, char **argv){
  int i,j,k,l;
  char *alignment_filename;
  alignment *locations;
  int loc_len;
  int size;
  int *bin;
  char *command;
  char *fname;
  char *fname2;
  FILE *out;

  if(argc < 2){
    fprintf(stderr,"Usage: %s <alignment_location>\n",argv[0]);
    return 1;
  }
  
  fname = (char *)malloc(sizeof(char *)*STRLEN);
  fname2 = (char *)malloc(sizeof(char *)*STRLEN);
  command = (char *)malloc(sizeof(char *)*STRLEN);
  
  sprintf(fname2,"%s.res1",argv[1]);
  sprintf(command,"awk '{print $1, $2, $3, $4, $8, $9, $12}' %s > %s\n",argv[1],fname2);
  system(command);
  
  sprintf(fname,"%s.res1",argv[1]);
  sprintf(fname2,"%s.res2",argv[1]);
  sprintf(command,"awk '{if ($6 < 0 ) print NR, $1, -1*$6, $2, $3, $4, $5, $7; else print NR, $1, $6, $2, $3, $4, $5, $7;}' %s | sort -k 2,3 > %s\n",fname,fname2);
  system(command);

  bin = (int *)malloc(sizeof(int)*25);
  for(i=0;i<25;i++)
    bin[i]=0;

  alignment_filename = (char *)malloc(sizeof(char)*STRLEN);
  strncpy(alignment_filename,fname2,STRLEN);
  sprintf(fname,"%s.res3",argv[1]);

  out = fopen(fname,"w");
  if(out==NULL) {
    printf("Error: can't open file %s.\n",fname);
    exit(0);
  }
  getlocations(alignment_filename,&locations,&loc_len);

  for(i=0;i<loc_len;i++){
    fprintf(out,"%s %d %d\n",locations[i].chrname,locations[i].start,locations[i].end);
  }
  fclose(out);
  
  /*
  sprintf(command,"./stats -i %s -revert -ps\n",fname);
  system(command);
  sprintf(command,"gnuplot  %s.gnuplot\n",fname);
  system(command);
  sprintf(command,"ps2pdf  %s.ps\n",fname);
  system(command);
 
  for(i=0;i<25;i++){
    //   if( (locations[i].insert >= MIN) && (locations[i].insert <= MAX) )
    printf("%d %d\n",100+(i*20),bin[i]);
  }
  */
 
  return 0;  
}
