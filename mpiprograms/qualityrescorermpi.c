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

//Template was from blastmpi5.c

struct _fileliststruct{

	char *name;
	int started;
        int finished;
};

typedef struct _fileliststruct filelist;

/* Local functions */

static void master(char *,char *,char *);
static void slave(void);
char *get_next_work_item(char *,char *, char *,filelist **);

//Functions related to file processing
int file_select();
extern  int alphasort();
filelist *processtheinputlist_and_initialize(char *inputdir,char *outputdir);



int main(int argc, char **argv)
{
//# What if directory is half done then what happens, check if corresponding out exists first
//# master creates the new directory, others write their output there.
//# Argument parsing is going to be in the sge side or this side
//# Crash can happen in one of the processors, then the others take over and finish?
//# Crash can happen in the SGE, than some of them are finished, when we come back do we restart?
// 3-> we check first, if the output exists or not

/*
blast_quality_rescorer354.pl -in batch000000.bo.gz.parse  -out batch000000.bo.gz.parse.out -minquality 30 -qual2db
/home/zhaoshi/macaque/Oct-04/Macaque.qual  -header -columns 4:5:6:0:1:2  -inputtype blastparser -alignments 15:14
-qdecode_program base2qual_compact_acgt -globalfast
*/

  char *rescoreparameters;
  char *inputdir;
  char *outputdir;
  char *MPIparameters;
  char **MPIargv;

  int myrank;
  int i;

  rescoreparameters = (char *) malloc(sizeof(char)*1000);
  inputdir = (char *) malloc(sizeof(char)*1000);
  outputdir = (char *) malloc(sizeof(char)*1000);

  strcpy(rescoreparameters,argv[1]);
  strcpy(inputdir,argv[2]);
  strcpy(outputdir,argv[3]);


 /* Initialize MPI */

  MPI_Init(&argc,&argv);


  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (myrank == 0) {
    master(inputdir,outputdir,rescoreparameters);
  } else {

    printf("slave is started\n");
    slave();
  }

  /* Shut down MPI */

  MPI_Finalize(); 




  return 0;
}


static void master(char *inputdir,char *outputdir,char *rescoreparameters)
{ 
  int ntasks, rank;
  char *result;
  MPI_Status status;
  int i; //for loop variable
  filelist *myfilelist;
  char *work;
  char *diestring; 
  struct direct **files;
  int count;
  /* Find out how many processes there are in the default
     communicator */
 diestring=(char *)malloc(sizeof(char)*512);
 result=(char *)malloc(sizeof(char)*512);
 work=(char *)malloc(sizeof(char)*2048);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  /* Seed the slaves; send one unit of work to each slave. */
	strcpy(diestring,"XXX"); //if work is null, it means the number of jobs are more than the processors


  printf("MASTER Master received the inputs\ninputdir is %s \noutputdir is %s\nrescoreparameters are %s\n",inputdir,outputdir,rescoreparameters);
   
   count = scandir(inputdir, &files, file_select, alphasort);

  myfilelist=(filelist *) malloc(sizeof(filelist)*(count+1));

 
  myfilelist=processtheinputlist_and_initialize(inputdir,outputdir);


  for (rank = 1; rank < ntasks; ++rank) {

    /* Find the next item of work to do */

    printf("MASTER Forloop ranks\n");
    work = get_next_work_item(inputdir,outputdir,rescoreparameters,&myfilelist);
    /* Send it to each rank */
    if (work == _NULL)
    {
        MPI_Send(diestring,             /* message buffer */
                2048,                 /* one data item */
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
       		2048,                 /* one data item */
             	MPI_CHAR,           /* data item is an integer */
             	rank,              /* destination process rank */
             	WORKTAG,           /* user chosen message tag */
             	MPI_COMM_WORLD);   /* default communicator */
     	strcpy(work,"\0"); //initialize it
     	printf("MASTER sent success \n");
    }
 //fflush(stdout);

  }

  printf("MASTER First phase is completed\n");
  /* Loop over getting new work requests until there is no more work
     to be done */

  work = get_next_work_item(inputdir,outputdir,rescoreparameters,&myfilelist);
  printf("MASTER Second phase is completed\n");

  while (work != _NULL) {

    /* Receive results from a slave */

    MPI_Recv(result,           /* message buffer */
             512,                 /* one data item */
             MPI_CHAR,        /* of type double real */
             MPI_ANY_SOURCE,    /* receive from any sender */
             MPI_ANY_TAG,       /* any type of message */
             MPI_COMM_WORLD,    /* default communicator */
             &status);          /* info about the received message */

    //process the result, update &myfilelist
    /* Send the slave a new work unit */

    MPI_Send(work,             /* message buffer */
             2048,                 /* one data item */
             MPI_CHAR,           /* data item is an integer */
             status.MPI_SOURCE, /* to who we just received from */
             WORKTAG,           /* user chosen message tag */
             MPI_COMM_WORLD);   /* default communicator */

    /* Get the next unit of work to be done */
    work = get_next_work_item(inputdir,outputdir,rescoreparameters,&myfilelist);
    //fflush(stdout);


  }
  printf("MASTER While is skipped\n");
  printf("still here\n");
  /* There's no more work to be done, so receive all the outstanding
     results from the slaves. */

  for (rank = 1; rank < ntasks; ++rank) {
    printf("Rank %d is done\n",rank);
    MPI_Recv(result, 512, MPI_CHAR, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  	 //fflush(stdout);

  }
  printf("MASTER All the results are collected, now sending die signals to processors\n");
  /* Tell all the slaves to exit by sending an empty message with the
     DIETAG. */
  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Send(diestring, 2048, MPI_CHAR, rank, DIETAG, MPI_COMM_WORLD);
  }
 printf("MASTER Program finished succesfully \n");
}


static void slave(void)
{
  char  work[2048];
  char *result;
  MPI_Status status;
  char *command=(char *)malloc(sizeof(char)*2048);
  char *gzipcommand=(char *)malloc(sizeof(char)*2048);
  result=(char *)malloc(sizeof(char)*512);
  //owork=(char *)malloc(sizeof(char)*2048);

 sprintf(result,"%s","noworkassigned");
  while (1) {

    /* Receive a message from the master */

    MPI_Recv(work, 2048, MPI_CHAR, 0, MPI_ANY_TAG,
	             MPI_COMM_WORLD, &status);

    /* Check the tag of the received message. */

   printf("\n\n\nSLAVE STARTED \n work is  %s\n",work);
  //either master send me a "DIE" command, or it is the beginning and number of works are less than number of processors
  //no work to be done so I just return
   if ((status.MPI_TAG == DIETAG) ) {
     printf("DIE SIGNAL SENT TO ME\n");
     MPI_Send(result, 512, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

     return;
    }
   if (!strcmp(work,"XXX"))
    {
	printf("No more work to be done so quitting...\n");
	MPI_Send(result, 512, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	return;

    }


    /* Do the work */

    command=strtok(work,"\n");
    system(command);
    result=strtok(_NULL,"\n");
//NOW GZIP THE RESULTSSSSSSSSSSSSSSSS
    sprintf(gzipcommand,"gzip %s.qscore",result);
   printf("%s\n",gzipcommand);
   system(gzipcommand);
   printf(" SLAVE FINISHES \n\n");

    printf("Sending the result \n");
    /* Send the result back */
    MPI_Send(result, 512, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    printf("Successfully sent \n");

  }
}



char *get_next_work_item(char *inputdir,char *outputdir,char *rescoreparameters,filelist **myfilelist)
{
  /* Fill in with whatever is relevant to obtain a new unit of work
     suitable to be given to a slave. */
//Input: filedir
//Input: outdir

  char *currentfile;
  char *out;
  int i,count;
  struct direct **files;

currentfile=(char *)malloc(sizeof(char)*512);
out=(char *)malloc(sizeof(char)*2048);

count = scandir(inputdir, &files, file_select, alphasort);


 for (i=0;i<count;i++)
 {

	if (((*myfilelist)[i].finished==0) && ((*myfilelist)[i].started==0)) //if that file hasn't been touched by other processor yet
 	{

		strcpy(currentfile,files[i]->d_name); //get the file name
		sprintf(out,"%s -in %s/%s -o %s/%s.qscore\n%s/%s",rescoreparameters,inputdir,currentfile,outputdir,currentfile,outputdir,currentfile);
		(*myfilelist)[i].started=1; //mark that file as started.
		return out;
        }
}
return _NULL; //no files to go but how about the ones that are started but not finished


}
filelist *processtheinputlist_and_initialize(char *inputdir,char *outputdir)
{
//scandir routine is from www.cs.cf.ac.uk/Dave/C/node20.html
  int count,i;
  struct direct **files;
  filelist *myfilelist;
  char *outfile;
  DIR *dfd;
  FILE *outputfilepointer;
  outfile=(char *)malloc(sizeof(char)*512);


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
     myfilelist[i].name=(char *)malloc(sizeof(char)*512);
     strcpy(myfilelist[i].name,files[i]->d_name);

   sprintf(outfile,"%s/%s.qscore.gz",outputdir,myfilelist[i].name);

  printf("%s\n",outfile);
  if ((outputfilepointer=fopen(outfile,"r"))==NULL)
  {
          printf("PRE PROCESSING : this file  %s  hasn't processed yet\n",files[i]->d_name);

        myfilelist[i].finished=0;
        myfilelist[i].started=0;
  }
  else
  {
	fclose(outputfilepointer);
        printf("PRE PROCESSING: this file  %s  already processed\n",files[i]->d_name);
        myfilelist[i].finished=1;
        myfilelist[i].started=1;
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






