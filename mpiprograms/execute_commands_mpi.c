/*Jeff Kidd
  March 2007
  MPI program to execute list of commands */

#include <sys/types.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define FALSE 0
#define TRUE !FALSE
#define WORKTAG 1
#define DIETAG 2
#define _NULL 0
#define BIG_STRSIZE 2048
#define MED_STRSIZE 512
#define SMALL_STRSIZE 128



/* MPI FUNCTIONS */
static void master(char *,char *);
static void slave();
char* get_next_work_item(char** command_list, int num, int tot);
/* MPI FUNCTIONS */


//Functions related to file processing
char** read_command_list(char* commandsfile, int* num_lines);

//Functions related to file processing



int main(int argc, char **argv)
{


  char *MPIparameters;
  char **MPIargv;
  char *jobid;
  char * commandsfile;
  int myrank;
  int i;
  jobid = (char *) malloc(sizeof(char)*SMALL_STRSIZE);
  commandsfile=(char *) malloc(sizeof(char)*MED_STRSIZE);

  fprintf (stdout, "argc: %d\n", argc);
  fprintf (stdout, "argv1: %s\n", argv[1]);
  fprintf (stdout, "argv2: %s\n", argv[2]);

  strcpy(commandsfile, argv[1]);
  strcpy(jobid,argv[2]);


 /* Initialize MPI */
  MPI_Init(&argc,&argv);
  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (myrank == 0) {
    master(commandsfile,jobid);
  } else {
    printf("slave is started\n");
    slave();
  }

  /* Shut down MPI */
  MPI_Finalize();
  return 0;
}


static void master(char *commandsfile, char *jobid)
{
  int ntasks, rank;
  char *result;
  MPI_Status status;
  int i; //for loop variable
  char *work;
  char *diestring;
  int count;
  int tot_commands;
  char** command_list; // pointer to char*--array of strings--array of commands to execute
  diestring=(char *)malloc(sizeof(char)*SMALL_STRSIZE);
  result=(char *)malloc(sizeof(char)*MED_STRSIZE);
  work=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  /* Find out how many processes there are in the default
     communicator */
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  /* Seed the slaves; send one unit of work to each slave. */
  strcpy(diestring,"XXX"); //if work is null, it means the number of jobs are more than the processors
  printf("MASTER Master received the inputs\ncommands file is %s\n",commandsfile);
  command_list=read_command_list(commandsfile, &tot_commands);
  count=0;
  for (rank = 1; rank < ntasks; ++rank)
  {

    /* Find the next item of work to do */
    printf("MASTER Forloop ranks\n");
    work=get_next_work_item(command_list, count, tot_commands);
    count++; //increment count of how many work requests we have done
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
    } //end if work == _NULL
    else
    {
    	printf("MASTER work is %s \n",work);
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
  } //end for rank=1..ntasks

  printf("MASTER First phase is completed\n");
  /* Loop over getting new work requests until there is no more work
     to be done */

  work=get_next_work_item(command_list, count, tot_commands);
  count++; //increment count of how many work requests we have done
  /* Send it to each rank */
  while (work != _NULL)
  {
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
    work=get_next_work_item(command_list, count, tot_commands);
    count++; //increment count of how many work requests we have done
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

 //clean up
 for(i=0;i<tot_commands;i++) { free(command_list[i]); }
 free(command_list);
 printf("MASTER Program finished succesfully \n");
}


static void slave()
{
  char  work[BIG_STRSIZE];
  MPI_Status status;
  char *command=(char *)malloc(sizeof(char)*BIG_STRSIZE);
  char *result=(char *)malloc(sizeof(char)*MED_STRSIZE);
  char *myjobid=(char *)malloc(sizeof(char)*SMALL_STRSIZE);
  int exit_code=0; //For checking if the system commands finished succesfully
  char *cdcommand=(char *)malloc(sizeof(char)*BIG_STRSIZE);

  sprintf(result,"%s","noworkassigned");
  while (1)
  {
    /* Receive a message from the master */
    MPI_Recv(work, BIG_STRSIZE, MPI_CHAR, 0, MPI_ANY_TAG,
	             MPI_COMM_WORLD, &status);
    /* Check the tag of the received message. */
   printf("\n\n\nSLAVE STARTED \n work is  %s\n",work);
  //either master send me a "DIE" command, or it is the beginning and number of works are less than number of processors
  //no work to be done so I just return
   if ((status.MPI_TAG == DIETAG) )
   {
     printf("DIE SIGNAL SENT TO ME\n");
     MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
     return;
   } //end die tag
   if (!strcmp(work,"XXX"))
   {
	printf("No more work to be done so quitting...\n");
	MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	return;
   } //end empty work
    /* Do the work */
    command=strtok(work,"\n");
    sprintf(result,"%s","didwork");
    fflush(stdout);

    //cd to /tmp directory (local) so that don't overwrite tmp files and cause problems.
    sprintf(cdcommand,"cd /tmp");
    system(cdcommand); //cd to local directory

    system(command); //DO COMMAND HERE
    printf("SLAVE FINISHED, Sending the result \n");
    /* Send the result back */
    MPI_Send(result, MED_STRSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    printf("Successfully sent \n");

  } //end while TRUE
}  //end slave

char* get_next_work_item(char** command_list, int num, int tot)
{
    if(num>=tot) { return _NULL; } //no more work, so return NULL
	return(command_list[num]);
}




char** read_command_list(char* commandsfile, int* num_lines)
{
    char** command_list;
    FILE *commands; //file pointer, containing commands to execute
    char* command;
    char* com;
    commands=fopen(commandsfile, "r");
    if(commands == 0)
	{
	  	   printf("Cannot open commands file\n");
	  	   assert(0);
  	}
    command= (char *) malloc(sizeof(char)*BIG_STRSIZE);
    fgets(command, BIG_STRSIZE, commands); //get first line
    com=strtok(command, "\n"); //tokenize--get rid of newline
    (*num_lines)=atoi(com); //number of lines in the file
    printf("There are %i lines\n",(*num_lines));
    command_list=(char** ) malloc(sizeof(char*)*(*num_lines+1));
    int i;
    for(i=0;i<(*num_lines);i++)
    {
          fgets(command, BIG_STRSIZE, commands);
          com=strtok(command, "\n");
          command_list[i]=(char*) malloc(sizeof(char)*BIG_STRSIZE);
	      strcpy(command_list[i],com);
	}

	fclose(commands); //close the file
	free(command); //cleanup
	return command_list;
}
