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
#define BIG_STRSIZE 2048
#define MED_STRSIZE 512
#define SMALL_STRSIZE 128

//TODO: COPYING THE DATABASES LOCALLY.. NEED TO CHECK IF ENOUGH LOCAL SPACE ON THE NODES
//September 6: GZIP is optional now
//AUGUST 18: Constant char * allocations are updated
//AUGUST 16 UPDATED COMPILING INFORMATION:http://www-unix.mcs.anl.gov/mpi/mpich/docs/mpichman-chp4/node34.htm#Node34

//uses tmp directories that SGE creates, no need to delete because it's already being deleted by SGE
//diff from blastmpi5.c Writes the blastoutput into a temp directory first and then copy the gzipped version into final location, 
//diff from blastmpi5.c gets an extra option from blastbatch JOB ID

//diff from blastmpi4.c , gzip the output and checks if gzipped output is ther
//adv: 1. Space effective 2. If blast is half done, it is never going to have .gzip so partly eliminates the problem of having not complete blast
// still t=here is a problem if the program stopped in the middle of a gzip.
//mpicc -o eray blastmpitemplate6.c
//blastmpitemplate6.c is the working version.



struct _fileliststruct{

	char *name;
	int started;
        int finished;
	int count; //I need to make it one global variable per struct in the future	
};

typedef struct _fileliststruct filelist;

/* MPI FUNCTIONS */
static void master(char *,char *,char *,char *,int);
static void slave();
char *get_next_work_item(char *,char *, char *,char *,filelist **,int);
/* MPI FUNCTIONS */


//Functions related to file processing
int file_select();
extern  int alphasort();
filelist *processtheinputlist_and_initialize(char *inputdir,char *outputdir,int GZIPFLAG);
//Functions related to file processing



int main(int argc, char **argv)
{

//"blastall -p blastn -d build34 -e 0.1" inputdir outdir "MPI parameters"r

/*
 printf("argc is %d \n",argc);
  if( argc != 10 )
   {
      printf("usage: %s  blastparameters     inputdir     outputdir   jobid GZIPFLAG \n",argv[0]);
      exit(0);
   }
*/

  char *blastparameters;
  char *inputdir;
  char *outputdir;
  char *MPIparameters;
  char **MPIargv;
  char *jobid;
  int myrank;
  int i;
  int GZIPFLAG;
  blastparameters = (char *) malloc(sizeof(char)*BIG_STRSIZE);
  inputdir = (char *) malloc(sizeof(char)*BIG_STRSIZE);
  outputdir = (char *) malloc(sizeof(char)*BIG_STRSIZE);
  jobid = (char *) malloc(sizeof(char)*SMALL_STRSIZE);
 
  strcpy(blastparameters,argv[1]);
  strcpy(inputdir,argv[2]);
  strcpy(outputdir,argv[3]);
  strcpy(jobid,argv[4]);
  GZIPFLAG=atoi(argv[5]);

 /* Initialize MPI */

  MPI_Init(&argc,&argv);
  printf("Gzipflag is %d FROM MAIN\n",GZIPFLAG);

  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (myrank == 0) {
    master(inputdir,outputdir,blastparameters,jobid,GZIPFLAG);
  } else {

    printf("slave is started\n");
    slave();
  }
                                                               
  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}


static void master(char *inputdir,char *outputdir,char *blastparameters, char *jobid, int GZIPFLAG)
{
  int ntasks, rank;
  char *result;
  MPI_Status status;
  int i; //for loop variable
  filelist *myfilelist;
  char *work;
  char *diestring;
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


  printf("MASTER Master received the inputs\ninputdir is %s \noutputdir is %s\nblastparameters are %s\n",inputdir,outputdir,blastparameters);

 count = scandir(inputdir, &files, file_select, alphasort);

  myfilelist=(filelist *) malloc(sizeof(filelist)*(count+1));

  myfilelist=processtheinputlist_and_initialize(inputdir,outputdir,GZIPFLAG);


  for (rank = 1; rank < ntasks; ++rank) {

    /* Find the next item of work to do */

    printf("MASTER Forloop ranks\n");  
    work = get_next_work_item(inputdir,outputdir,blastparameters,jobid,&myfilelist,GZIPFLAG);
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
    	printf("MASTER work issssssssssssss %s \n",work);
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
  
  work = get_next_work_item(inputdir,outputdir,blastparameters,jobid,&myfilelist,GZIPFLAG);

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
    work = get_next_work_item(inputdir,outputdir,blastparameters,jobid,&myfilelist,GZIPFLAG);
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
  char *mvop=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  char *gzipcommand=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  char *finaloutputname=(char *)malloc(sizeof(char)*BIG_STRSIZE); //To store final output name to move can end with .bo or .gz
  char *result=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *currentfile=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *myjobid=(char *)malloc(sizeof(char)*SMALL_STRSIZE);
  int gzipflag;

  int exit_code=0; //For checking if the system commands finished succesfully

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
 

    /* Do the work */
    gzipflag=atoi(strtok(work,"\n"));
    command=strtok(_NULL,"\n");
    result=strtok(_NULL,"\n");
    currentfile=strtok(_NULL,"\n");
//NOW GZIP THE RESULTSSSSSSSSSSSSSSSS
  printf("Gzipflag is %d FROM SLAVE\n",gzipflag);
fflush(stdout);
    myjobid=strtok(_NULL,"\n");
    sprintf(cdcommand,"cd /tmp/%s/",myjobid);
    system(cdcommand); //cd to local directory
    system(command); //DO THE BLAST HERE


if (gzipflag==1)
{
    sprintf(gzipcommand,"gzip /tmp/%s/%s.bo",myjobid,currentfile);
    printf("%s\n",gzipcommand);
    exit_code=system(gzipcommand);
    if( exit_code != 0 )
        printf("ERROR gzip didn't work right. The command was: %s \n", gzipcommand);

}




if (gzipflag ==1)
{
   sprintf(mvop,"mv /tmp/%s/%s.bo.gz %s.bo.gz",myjobid,currentfile,result);

}
else
{
   sprintf(mvop,"mv /tmp/%s/%s.bo %s.bo",myjobid,currentfile,result);
}
   printf("DEBUG: %s\n",mvop);
   exit_code=system(mvop);
   if( exit_code != 0 )
        printf("ERROR mv didn't work right. The command was: %s \n", mvop);


    printf("SLAVE FINISHED, Sending the result \n");
    /* Send the result back */
    MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    printf("Successfully sent \n");

  }
}



char *get_next_work_item(char *inputdir,char *outputdir,char *blastparameters,char *jobid,filelist **myfilelist, int GZIPFLAG)
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

count = (*myfilelist)[0].count;
printf("Counter is %d\n",count);
//JOBID.1.all.q
sprintf(myjobid,"%s.1.eee.q",jobid);
//printf("Job id is  %s\n",jobid);
                                                                                                                      
 for (i=0;i<count;i++) 
 {
	//printf("I is %d\n",i);

	if (((*myfilelist)[i].finished==0) && ((*myfilelist)[i].started==0)) //if that file hasn't been touched by other processor yet
 	{

		strcpy(currentfile,(*myfilelist)[i].name); //get the file name
		sprintf(out,"%d\n%s -i %s/%s -o /tmp/%s/%s.bo\n%s/%s\n%s\n%s",GZIPFLAG,blastparameters,inputdir,currentfile,myjobid,currentfile,outputdir,currentfile,currentfile,myjobid);
//change in blastmpi6.c, I am going to the blast in /tmp file, to the end I am also adding the jobid
		(*myfilelist)[i].started=1; //mark that file as started.
		return out;
        }
}
return _NULL; //no files to go but how about the ones that are started but not finished
//printf("get next is done\n");

}
filelist *processtheinputlist_and_initialize(char *inputdir,char *outputdir, int GZIPFLAG)
{ 
//scandir routine is from www.cs.cf.ac.uk/Dave/C/node20.html
  int count,i;
  struct direct **files;
  filelist *myfilelist;
  char *outfile;
  DIR *dfd;
  FILE *outputfilepointer;
  outfile=(char *)malloc(sizeof(char)*MED_STRSIZE);   
    
    
    //check if the dir exists??? ADD MORE ERROR CHECKING HERE!!!!!!
  printf("Current Working Directory = %s\n",inputdir);
        
         
  if ((dfd=opendir(inputdir))==NULL)
  {
        printf("Can't access %s \nSo I am quitting\n",inputdir);
        exit(0); //
                
  }
  count = scandir(inputdir, &files, file_select, alphasort);
   
  printf("Number of files in the list are  %d \n",count);
  myfilelist=(filelist *) malloc(sizeof(filelist)*(count+1));
  
 
 for (i=0;i<count;i++)
 {
     myfilelist[i].name=(char *)malloc(sizeof(char)*MED_STRSIZE);
     strcpy(myfilelist[i].name,files[i]->d_name);
     if (GZIPFLAG==0)
     {  
   		sprintf(outfile,"%s/%s.bo",outputdir,myfilelist[i].name);  
     }
     else
     {
   	sprintf(outfile,"%s/%s.bo.gz",outputdir,myfilelist[i].name);  

     }

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
  sleep(100);
  closedir(dfd);
 return myfilelist;
  
} 
int file_select(struct direct *entry)
                                                                                                                                                             
{
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
                return (FALSE);
        else
                return (TRUE);
}


























//alo
//alo
//alo
//alo
//alo
//alo
//alo
//alo
//alo
//alo
//alo
//alo
//alo

