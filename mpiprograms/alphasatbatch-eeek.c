#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define FALSE 0
#define TRUE !FALSE
#define WORKTAG 1
#define DIETAG 2
#define _NULL 0
#define BIG_STRSIZE 4096
#define MED_STRSIZE 1024
#define SMALL_STRSIZE 256




struct _fileliststruct{

  char *name;
  int started;
  int finished;
  int count;
};

typedef struct _fileliststruct filelist;

/* MPI FUNCTIONS */
static void master(char *,char *,char *,char *);
static void slave();
char *get_next_work_item(char *,char *, char *,char *,filelist **);
/* MPI FUNCTIONS */


//Functions related to file processing
int file_select();
extern  int alphasort();
filelist *processtheinputlist_and_initialize(char *inputdir,char *outputdir, char *);
//Functions related to file processing



int main(int argc, char **argv)
{

  //"runfast3  -genome /mnt/gpfs/blastdb/PFASTDB/HUMAN/BUILD35/build35.idx -search discordantall.fa.in -out a"
  //blastparameters- >-genome /mnt/gpfs/blastdb/PFASTDB/HUMAN/BUILD35/build35.idx
  // -search discordantall.fa.in -out a

  char *blastparameters;
  char *inputdir;
  char *outputdir;
  char *jobid;
  int myrank;
  
  blastparameters = (char *) malloc(sizeof(char)*BIG_STRSIZE);
  inputdir = (char *) malloc(sizeof(char)*BIG_STRSIZE);
  outputdir = (char *) malloc(sizeof(char)*BIG_STRSIZE);
  jobid = (char *) malloc(sizeof(char)*SMALL_STRSIZE);
 
  strcpy(blastparameters,argv[1]);
  strcpy(inputdir,argv[2]);
  strcpy(outputdir,argv[3]);
  strcpy(jobid,argv[4]);

  /* Initialize MPI */

  MPI_Init(&argc,&argv);

  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (myrank == 0) {
    master(inputdir,outputdir,blastparameters,jobid);
  } else {

    printf("slave is started\n");
    slave();
  }
                                                               
  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}


static void master(char *inputdir,char *outputdir,char *blastparameters, char *jobid)
{
  int ntasks, rank;
  char *result;
  MPI_Status status;
  
  filelist *myfilelist;
  char *work;
  char *diestring;
  DIR *dfd;

  int count;
  struct direct **files;
  /* Find out how many processes there are in the default
     communicator */
  diestring=(char *)malloc(sizeof(char)*SMALL_STRSIZE);
  result=(char *)malloc(sizeof(char)*MED_STRSIZE);
  work=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
 
  /* Seed the slaves; send one unit of work to each slave. */
  strcpy(diestring,"XXX"); //if work is null, it means the number of jobs are more than the processors 

 
  printf("MASTER Master received the inputs\ninputdir is %s \noutputdir is %s\nparameters are %s\n",inputdir,outputdir,blastparameters);
 
 
  if ((dfd=opendir(inputdir))==NULL)
    {
      printf("Can't access %s \nSo I am quitting\n",inputdir);
      exit(0); //

    }
  closedir(dfd);

  count=scandir(inputdir,&files,file_select,alphasort);

  myfilelist=(filelist *) malloc(sizeof(filelist)*(count+1));
  myfilelist=processtheinputlist_and_initialize(inputdir,outputdir, blastparameters);


  for (rank = 1; rank < ntasks; ++rank) {

    /* Find the next item of work to do */

    printf("MASTER Forloop ranks\n");  
    work = get_next_work_item(inputdir,outputdir,blastparameters,jobid,&myfilelist);
    /* Send it to each rank */
    if (work == _NULL)
      { 
        MPI_Send(diestring,             /* message buffer */
		 BIG_STRSIZE,                 /* one data item */
		 MPI_CHAR,           /* data item is an integer */
		 rank,              /* destination process rank */
		 DIETAG,           /* user chosen message tag */
		 MPI_COMM_WORLD);   /* default communicator */
        printf("DIEstring sent \n");



      }
    else 
      {
    	printf("MASTER work  %s \n",work);
    	MPI_Send(work,             /* message buffer */
		 BIG_STRSIZE,                 /* one data item */
		 MPI_CHAR,           /* data item is an integer */
		 rank,              /* destination process rank */
		 WORKTAG,           /* user chosen message tag */
		 MPI_COMM_WORLD);   /* default communicator */
     	strcpy(work,"\0"); //initialize it
     	printf("MASTER sent success \n");
      }
    fflush(stdout);

  }

  printf("MASTER First phase is completed\n");
  /* Loop over getting new work requests until there is no more work
     to be done */
  
  work = get_next_work_item(inputdir,outputdir,blastparameters,jobid,&myfilelist);

  while (work != _NULL) {

    /* Receive results from a slave */

    MPI_Recv(result,           /* message buffer */
             MED_STRSIZE,                 /* one data item */
             MPI_CHAR,        /* of type double real */
             MPI_ANY_SOURCE,    /* receive from any sender */
             MPI_ANY_TAG,       /* any type of message */
             MPI_COMM_WORLD,    /* default communicator */
             &status);          /* info about the received message */

    //process the result, update &myfilelist
    /* Send the slave a new work unit */

    MPI_Send(work,             /* message buffer */
             BIG_STRSIZE,                 /* one data item */
             MPI_CHAR,           /* data item is an integer */
             status.MPI_SOURCE, /* to who we just received from */
             WORKTAG,           /* user chosen message tag */
             MPI_COMM_WORLD);   /* default communicator */

    /* Get the next unit of work to be done */
    work = get_next_work_item(inputdir,outputdir,blastparameters,jobid,&myfilelist);
    fflush(stdout);

    
  }
  printf("MASTER While is skipped\n");
  printf("still here\n");
  /* There's no more work to be done, so receive all the outstanding
     results from the slaves. */

  for (rank = 1; rank < ntasks; ++rank) {
    printf("Rank %d is done\n",rank);	
    MPI_Recv(result, MED_STRSIZE, MPI_CHAR, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    fflush(stdout);

  }
  printf("MASTER All the results are collected, now sending die signals to processors\n");
  /* Tell all the slaves to exit by sending an empty message with the
     DIETAG. */
  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Send(diestring, BIG_STRSIZE, MPI_CHAR, rank, DIETAG, MPI_COMM_WORLD);
  }
  printf("MASTER Program finished succesfully \n");
}


static void slave()
{
  char  work[BIG_STRSIZE];
  MPI_Status status;
  char *command=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  char *cdcommand=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  //  char *mvop=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  // char *finaloutputname=(char *)malloc(sizeof(char)*BIG_STRSIZE); //To store final output name to move can end with .bo or .gz
  char *result=(char *)malloc(sizeof(char)*MED_STRSIZE);
  //char *currentfile=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *outdir=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *mvop=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *myjobid=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *currentfile=(char *)malloc(sizeof(char)*MED_STRSIZE);

  //int exit_code=0; //For checking if the system commands finished succesfully

  sprintf(result,"%s","noworkassigned");
  while (1) {

    /* Receive a message from the master */
    MPI_Recv(work, BIG_STRSIZE, MPI_CHAR, 0, MPI_ANY_TAG,
	     MPI_COMM_WORLD, &status);

    /* Check the tag of the received message. */

    printf("\n\n\nSLAVE STARTED \n work is  %s\n",work);
    //either master send me a "DIE" command, or it is the beginning and number of works are less than number of processors
    //no work to be done so I just return 
    if ((status.MPI_TAG == DIETAG) ) {
      printf("DIE SIGNAL SENT TO ME\n"); 
      MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

      return;
    }
    if (!strcmp(work,"XXX"))
      {
	printf("No more work to be done so quitting...\n");
	MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	return;

      }

    /*
    //runfast3  -genome /mnt/gpfs/blastdb/PFASTDB/HUMAN/BUILD35/build35.idx -search discordantall.fa.in -out a
    strcpy(currentfile,(*myfilelist)[i].name); //get the file name
    sprintf(out,"/mnt/gpfs/local/inhousebin/runfast3 %s -search %s/%s -out %s\n%s\n",blastparameters,currentfile,outputdir,outputdir);
    */
 

    /* Do the work */
    command=strtok(work,"\n");
    outdir=strtok(_NULL,"\n");
    myjobid=strtok(_NULL,"\n");
    currentfile=strtok(_NULL,"\n");

    sprintf(cdcommand,"cd /tmp/%s/",myjobid);
    printf("Currentfile %s\n",currentfile);
    fflush(stdout);

    system(command); //DO THE ALIGNMENT HERE

    sprintf(mvop,"mv /tmp/%s/%s*log %s",myjobid,currentfile,outdir);
    printf("Move command: %s\n",mvop);
    system(mvop);
    sprintf(mvop,"mv /tmp/%s/%s*matrix %s", myjobid,currentfile,outdir);
    printf("Move command: %s\n",mvop);
    system(mvop);
    sprintf(mvop,"mv /tmp/%s/%s*histogram %s",myjobid,currentfile,outdir);
    printf("Move command: %s\n",mvop);
    system(mvop);

    printf("Outdir is %s, Work is %s \n",outdir,work);
    printf("SLAVE FINISHED, Sending the result \n");
    /* Send the result back */
    MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    printf("Successfully sent \n");

    //free(command);
    //free(cdcommand);
    //free(result);
    //free(outdir);
  }
}



char *get_next_work_item(char *inputdir,char *outputdir,char *blastparameters,char *jobid,filelist **myfilelist)
{
  /* Fill in with whatever is relevant to obtain a new unit of work
     suitable to be given to a slave. */
  //Input: filedir
  //Input: outdir

  char *currentfile;
  char *out;
  int i,count;
  char *myjobid;

  currentfile=(char *)malloc(sizeof(char)*MED_STRSIZE);
  out=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  myjobid=(char *)malloc(sizeof(char)*SMALL_STRSIZE);
  sprintf(myjobid,"%s.1.all.q",jobid);

  count = (*myfilelist)[0].count;

  for (i=0;i<count;i++) 
    {

      if (((*myfilelist)[i].finished==0) && ((*myfilelist)[i].started==0)) //if that file hasn't been touched by other processor yet
	{


	  //inputdir has the batches;
	  //blastparameters will have the finalhor.fa
	  //we need -out parameter
	  //we need -m parameter for monomer file
	  //we need -h parameter for hor file WILL BE PASSED WITH BLASTPARAMETER

	  strcpy(currentfile,(*myfilelist)[i].name); //get the file name
	  sprintf(out,"%s -m %s/%s -o /tmp/%s/\n%s\n%s\n%s\n",blastparameters,inputdir,currentfile,myjobid,outputdir,myjobid,currentfile);
	  (*myfilelist)[i].started=1; //mark that file as started.
	  return out;
	}
    }

  //free(currentfile);
  //free(out);
  //free(myjobid);
  return _NULL; //no files to go but how about the ones that are started but not finished
}
filelist *processtheinputlist_and_initialize(char *inputdir,char *outputdir, char *blastparameters)
{ 
  //scandir routine is from www.cs.cf.ac.uk/Dave/C/node20.html
  int count,i;
  struct direct **files;
  filelist *myfilelist;
  char *outfile;
  DIR *dfd;
  FILE *outputfilepointer;
  char **argv;
  char horfile[120];
  char *horhor;	
  char mismatch[5];

  argv = (char **) malloc (sizeof(char *) * 5);
  for (i=0;i<5;i++)
    argv[i] = (char *) malloc (sizeof(char ) * 120);

  outfile=(char *)malloc(sizeof(char)*MED_STRSIZE);   
    
  sscanf(blastparameters, "%s %s %s %s %s", argv[0], argv[1], argv[2], argv[3], argv[4]);
    
  for (i=1;i<5;i++){
    if (!strcmp(argv[i], "-h"))
      strcpy(horfile, argv[i+1]);
    else if (!strcmp(argv[i], "-d"))
      strcpy(mismatch, argv[i+1]);
  }

  horhor = horfile;
  for (i=strlen(horfile)-1;i>=0;i--)
	if (horfile[i] == '/') break;
  horhor = horhor+i+1;

  //check if the dir exists??? ADD MORE ERROR CHECKING HERE!!!!!!
  printf("Current Working Directory = %s\n",inputdir);
        
         
  if ((dfd=opendir(inputdir))==NULL)
    {
      printf("Can't access %s \nSo I am quitting\n",inputdir);
      exit(0); //
                
    }
  closedir(dfd);

  count = scandir(inputdir, &files, file_select, alphasort);


  printf("Number of files in the list are  %d \n",count);
  myfilelist=(filelist *) malloc(sizeof(filelist)*(count+1));
  
 
  for (i=0;i<count;i++)
    {
      myfilelist[i].name=(char *)malloc(sizeof(char)*MED_STRSIZE);
      strcpy(myfilelist[i].name,files[i]->d_name);
      // calkan
      sprintf(outfile,"%s/%s-%s.m%s.log",outputdir,myfilelist[i].name, horhor, mismatch);  

      printf("outfile is %s\n",outfile);
      if ((outputfilepointer=fopen(outfile,"r"))==NULL)
	{
	  printf("PRE PROCESSING : this file  %s  hasn't processed yet\n",files[i]->d_name);
	  myfilelist[i].finished=0;
	  myfilelist[i].started=0;
	  myfilelist[i].count=count;
       
	}
      else
	{ 
	  fclose(outputfilepointer);
	  printf("PRE PROCESSING: this file  %s  already processed\n",files[i]->d_name);
	  myfilelist[i].finished=1;
	  myfilelist[i].started=1;
	  myfilelist[i].count=count;

	}  
                
    }  


  fflush(stdout);

  for (i=0;i<5;i++) free(argv[i]);
  free(argv);

  //free(outfile);  
  return myfilelist;
  
} 
int file_select(struct direct *entry)
                                                                                                                                                             
{
  if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
    return (FALSE);
  else
    return (TRUE);
}






