#include <stdio.h>
#include <string.h>
#include <zlib.h>

#define MAXLEN 50000
#define MAXNAME 200

main(int argc, char **argv){
  char name[MAXNAME];
  char seq[MAXLEN];
  char qual[MAXLEN];
  char plus[MAXNAME];
  int i;
  FILE *qualf;
  FILE *fasta; FILE *in;
  char fname[100];
  char fastaname[100];
  char qname[100];
  int GZ=0;
  float FILTER=1.0;
  int qualout=1;
  int ret;
  char line[100];
  int seqlen = 0;
  long quals[100];
  long basequals[100];
  int seqcnt=0;
  long a,c,g,t,n;
  int orient=0;
  char suffix[4];
  int thisq;
  char dummy[100];
  char ch;
  int ncnt;
  
  a=0; c=0;g=0;t=0;n=0;
  suffix[0]=0;

  for (i=1;i<argc;i++){
    if (!strcmp(argv[i], "-i"))
      strcpy(fname, argv[i+1]);
    else if (!strcmp(argv[i], "-q"))
      strcpy(qname, argv[i+1]);
    
    else if (!strcmp(argv[i], "-gz"))
      GZ=1;
    else if (!strcmp(argv[i], "-filter"))
      FILTER=atof(argv[i+1]);
    /*
    else if (!strcmp(argv[i], "-nq"))
      qualout=0;
    else if (!strcmp(argv[i], "-suf"))
      sprintf(suffix, "/%s", argv[i+1]);
    */
  }

  /*
  memset(quals, 0, sizeof(long)*100);
  memset(basequals, 0, sizeof(long)*100);
  */


  if (!GZ){
    in = fopen(fname,"r");
    if (strcmp(qname, "fillin"))
      qualf = fopen(qname, "r");
    
    sprintf(fastaname,"%s.fq", fname);
    fasta = fopen(fastaname, "w");
  }
  else{
    in = gzopen(fname,"r");
    if (strcmp(qname, "fillin"))
      qualf = gzopen(qname, "r");
    
    sprintf(fastaname,"%s.fq.gz", fname);
    fasta = gzopen(fastaname, "w");
  }
  /*
  if (qualout){
    sprintf(qname,"%s.qual", fname);
    qualf = fopen(qname, "w");
  }

  */


  if (!GZ){

    fscanf(in, "%c", &ch);
    
    while (fscanf(in, "%s", name) > 0){
      fgets(dummy, 1000, in);
      
      ch=0; i=0; ncnt = 0;
      while(ch!='>'){
	if (fscanf(in, "%c", &ch) > 0){
	  if (ch!='>' && !isspace(ch)) {
	    seq[i++]=ch;
	    if (ch=='n' || ch=='N' || ch=='.') ncnt++;
	  }
	}
	else{
	  break;
	}
      }
      seq[i]=0;
      
      //printf("name:%s\n", name);
      //if (seqlen == 0)
      seqlen = strlen(seq);
      
      if ( (float)ncnt / (float)(seqlen) <= FILTER)
	fprintf(fasta,"@%s\n%s\n+\n", name, seq);
      seqcnt++;
      //      fscanf(qualf, "%s\n", plus);  

      if (strcmp(qname, "fillin")){
	fscanf(qualf, "%s", plus);
	fgets(dummy, 1000, qualf);
      }

      
      if (!strstr(plus, name)){
	printf("not right %s\t--\t%s\n", plus, name);
	exit(0);
      }

      for (i=0; i<seqlen; i++){
	if (strcmp(qname, "fillin"))
	  fscanf(qualf,"%d", &thisq);
	else
	  thisq = 40;

	if ((float)ncnt / (float)(seqlen) <= FILTER)
	  fprintf(fasta, "%c", (char)(thisq+33));
      }
      
      fprintf(fasta, "\n");
    }
    
    
    fclose(fasta);
  
  }

  else{

    /* not done yet */

    fscanf(in, "%c", &ch);
    
    while (fscanf(in, "%s", name) > 0){
      fgets(dummy, 1000, in);
      
      ch=0; i=0;
      while(ch!='>'){
	if (fscanf(in, "%c", &ch) > 0){
	  if (ch!='>' && !isspace(ch)) seq[i++]=ch;
	}
	else{
	  break;
	}
      }
      seq[i]=0;
      
      //printf("name:%s\n", name);
      //if (seqlen == 0)
      seqlen = strlen(seq);
      fprintf(fasta,"@%s\n%s\n+\n", name, seq);
      seqcnt++;
      //      fscanf(qualf, "%s\n", plus);  
      if (strcmp(qname, "fillin")){
	fscanf(qualf, "%s", plus) ;
	fgets(dummy, 1000, qualf);
      }
      
      if (!strstr(plus, name)){
	printf("not right %s\t--\t%s\n", plus, name);
	return ; 
      }
      for (i=0; i<seqlen; i++){
	if (strcmp(qname, "fillin"))
	  fscanf(qualf,"%d", &thisq);
	else
	  thisq = 40;
	fprintf(fasta, "%c", (char)(thisq+33));
      }
      
      fprintf(fasta, "\n");
    }
    
    
    fclose(fasta);


  }

}
