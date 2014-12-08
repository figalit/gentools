#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE !FALSE
#define WORKTAG 1
#define DIETAG 2
#define _NULL 0
//rmasker_mpi43.c older version stops every 946 files?
//rmasker_mpi42.c make sure it can continue a half done repeatmasker job, more solid file access mechanism(open,close handles
//rmasker_mpi4.c opendir/closedir functions fixed
//rmasker_mpi4.c checks only maskout and masked files to see if that file was finished processing
//rmasker_mpi3.c more file access check
//rmasker_mpi2.c
	//More solid version
//To compile:mpicc -o rmasker_mpi3.c


struct _fileliststruct{

	char *name; //name of the file
	int started; //0 means file haven't touched yet,1 means either it is still processing or finished
	int finished; //1 means file has finished, 0 means it is either still processing or not touched yet
 };

typedef struct _fileliststruct filelist;

///GLOBAL VARIABLES
//These are stable anyway, so there is no harm in defining them global
char maskoutdir[512];
char maskcatdir[512];
char masktbldir[512];
char maskeddir[512];
char workingdir[512];
int INPUTSIZECOUNT;
//



/* MPI FUNCTIONS */

static void master(char *,char *,char *);
static void slave(void);
char *get_next_work_item(char *,char *,filelist **);
/* MPI FUNCTIONS   */

//Functions related to file processing
int file_select();
extern  int alphasort();
filelist *processtheinputlist_and_initialize(char *inputdir);



int main(int argc, char **argv)
{


  char *rmaskerparameters;
  char *inputdir;
  char *outputdir;
  char **MPIargv;
  int myrank;

  rmaskerparameters = (char *) malloc(sizeof(char)*1000);
  inputdir = (char *) malloc(sizeof(char)*1000);
  outputdir = (char *) malloc(sizeof(char)*1000);

  strcpy(rmaskerparameters,argv[1]);
  strcpy(inputdir,argv[2]);
  strcpy(outputdir,argv[3]);


 /* Initialize MPI */

  MPI_Init(&argc,&argv);


  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (myrank == 0) {
   	 master(inputdir,outputdir,rmaskerparameters);
  } else {
	//system("cd /var/tmp");
 	printf("slave is started\n");
   	slave();
  }

  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}


static void master(char *inputdir,char *outputdir,char *rmaskerparameters)
{
  int ntasks, rank;
  char *result;
  MPI_Status status;
  filelist *myfilelist;
  char *work;
  char *diestring;
  char *command;
  DIR *dirpointer1,*dirpointer2;

  /* Find out how many processes there are in the default
     communicator */
 diestring=(char *)malloc(sizeof(char)*512);
 result=(char *)malloc(sizeof(char)*512);
 work=(char *)malloc(sizeof(char)*2048);
 command=(char *)malloc(sizeof(char)*2048);



  if (((dirpointer1=opendir(inputdir))==NULL)||((dirpointer2=opendir(outputdir))==NULL) )
  {
	printf("At least one of the input directories given as input was not accessible\n");
	exit(0);

  }
  closedir(dirpointer1);
  closedir(dirpointer2);

  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  /* Seed the slaves; send one unit of work to each slave. */

  strcpy(diestring,"XXX"); //if work is null, it means the number of jobs are more than the processors

//Creating the names of  mask_out,mask_tbl,mask_cat,masked directories
  sprintf(maskeddir,"%s/masked",outputdir);
  sprintf(maskcatdir,"%s/mask_cat",outputdir);
  sprintf(masktbldir,"%s/mask_tbl",outputdir);
  sprintf(maskoutdir,"%s/mask_out",outputdir);
  sprintf(workingdir,"%s/workingdir",outputdir);

//

//CREATING THE FOLDERS IF NECESSARY, IF THEY DON'T ALREADY EXISTS
  if ((dirpointer1=opendir(maskeddir))==NULL)
  {
	sprintf(command,"mkdir %s",maskeddir);
	system(command);
	printf("mkdir %s\n",maskeddir);
  }
  else
 	closedir(dirpointer1);

  if ((dirpointer1=opendir(maskcatdir))==NULL)
  {
	sprintf(command,"mkdir %s",maskcatdir);
	system(command);
	printf("mkdir %s\n",maskcatdir);
  }
  else
	  closedir(dirpointer1);

  if ((dirpointer1=opendir(masktbldir))==NULL)
  {
	sprintf(command,"mkdir %s",masktbldir);
	system(command);
	printf("mkdir %s\n",masktbldir);
  }
  else
	  closedir(dirpointer1);

  if ((dirpointer1=opendir(maskoutdir))==NULL)
  {
	sprintf(command,"mkdir %s",maskoutdir);
	system(command);
	printf("mkdir %s\n",maskoutdir);
  }
  else
	  closedir(dirpointer1);

  if ((dirpointer1=opendir(workingdir))==NULL)
  {
	sprintf(command,"mkdir %s",workingdir);
	system(command);
	printf("mkdir %s\n",workingdir);
  }
  else
  	closedir(dirpointer1);


//CREATED ALL THE FOLDERS NECESSARY
  myfilelist=processtheinputlist_and_initialize(inputdir);


  for (rank = 1; rank < ntasks; ++rank) {

    /* Find the next item of work to do */

    printf("MASTER FORLOOP RANK Number %d \n",rank);
    work = get_next_work_item(inputdir,rmaskerparameters,&myfilelist);
    /* Send it to each rank */
    if (work == _NULL)
    {
        MPI_Send(diestring,             /* message buffer */
                2048,                 /* one data item */
                MPI_CHAR,           /* data item is an integer */
                rank,              /* destination process rank */
                DIETAG,           /* user chosen message tag */
                MPI_COMM_WORLD);   /* default communicator */
        printf("No work to do, DIEstring sent \n");



    }
    else
    {
    	printf("MASTER work is %s \n",work);
    	MPI_Send(work,             /* message buffer */
       		2048,                 /* one data item */
             	MPI_CHAR,           /* data item is an integer */
             	rank,              /* destination process rank */
             	WORKTAG,           /* user chosen message tag */
             	MPI_COMM_WORLD);   /* default communicator */
     	strcpy(work,"\0"); //initialize it
    }
  }

  printf("MASTER First phase is completed\n");
  /* Loop over getting new work requests until there is no more work
     to be done */

  work = get_next_work_item(inputdir,rmaskerparameters,&myfilelist);

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
    work = get_next_work_item(inputdir,rmaskerparameters,&myfilelist);


  }
  /* There's no more work to be done, so receive all the outstanding
     results from the slaves. */

  for (rank = 1; rank < ntasks; ++rank) {
    printf("Rank %d is done\n",rank);
    MPI_Recv(result, 512, MPI_CHAR, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &status);
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
  char *command;

   char *currentfile;
   char *maskeddirfile,*maskcatfile,*maskoutfile,*masktblfile;
   char *mymaskeddir,*mymaskcatdir,*mymaskoutdir,*mymasktbldir;
   char *myworkingdir;
   FILE *temp;

  currentfile=(char *)malloc(sizeof(char)*2048);
  myworkingdir=(char *)malloc(sizeof(char)*2048);
  maskeddirfile=(char *)malloc(sizeof(char)*2048);
  maskcatfile=(char *)malloc(sizeof(char)*2048);
  maskoutfile=(char *)malloc(sizeof(char)*2048);
  masktblfile=(char *)malloc(sizeof(char)*2048);

  mymaskeddir=(char *)malloc(sizeof(char)*2048);
  mymaskcatdir=(char *)malloc(sizeof(char)*2048);
  mymaskoutdir=(char *)malloc(sizeof(char)*2048);
  mymasktbldir=(char *)malloc(sizeof(char)*2048);


  command=(char *)malloc(sizeof(char)*2048);
  result=(char *)malloc(sizeof(char)*512);

  //owork=(char *)malloc(sizeof(char)*2048);
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
     return;
    }
   if (!strcmp(work,"XXX"))
    {
	printf("No more work to be done so quitting...\n");
	return;
    }


    /* Do the work */


    chdir("/var/tmp");
    command=strtok(work,"\n");
    system(command);
    currentfile=strtok(_NULL,"\n");
    myworkingdir=strtok(_NULL,"\n");
    mymaskeddir=strtok(_NULL,"\n");
    mymaskcatdir=strtok(_NULL,"\n");
    mymaskoutdir=strtok(_NULL,"\n");
    mymasktbldir=strtok(_NULL,"\n");


   printf("My working dir is %s\n",myworkingdir);
   sprintf(maskeddirfile,"%s/%s.masked",myworkingdir,currentfile);
   sprintf(maskcatfile,"%s/%s.cat",myworkingdir,currentfile);
   sprintf(maskoutfile,"%s/%s.out",myworkingdir,currentfile);
   sprintf(masktblfile,"%s/%s.tbl",myworkingdir,currentfile);

//   printf("%s\n%s\n%s\%s\n",maskeddirfile,maskcatfile,maskoutfile,masktblfile);
   if ((temp=fopen(maskeddirfile,"r"))==NULL)

   {
		printf("Can't move  %s. The file doesn't exists\n",maskeddirfile);

   }
   else
   {
	  fclose(temp);
	  sprintf(command,"mv %s %s/",maskeddirfile,mymaskeddir);
   	  system(command);

   }

   if ((temp=fopen(maskcatfile,"r"))==NULL)
   {
   	printf("Can't move  %s. The file doesn't exists\n",maskcatfile);

   }
   else
   {
	fclose(temp);
	 sprintf(command,"mv %s %s/",maskcatfile,mymaskcatdir);
       system(command);

   }

   if ((temp=fopen(maskoutfile,"r"))==NULL)
   {
   	printf("Can't move  %s. The file doesn't exists\n",maskoutfile);

   }
   else
   {
	fclose(temp);
	sprintf(command,"mv %s %s/",maskoutfile,mymaskoutdir);
    	system(command);

   }

   if ((temp=fopen(masktblfile,"r"))==NULL)
   {
   	printf("Can't move  %s. The file doesn't exists\n",masktblfile);
   }
   else
   {
	fclose(temp);
	sprintf(command,"mv %s %s/",masktblfile,mymasktbldir);
    	system(command);
   }
//CHECK IF FILES EXIST HERE BEFORE MOVING@!!!!!!!!!!!!!
   printf(" SLAVE FINISHES \n\n");

    /* Send the result back */
    MPI_Send(result, 512, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

  }
}



char *get_next_work_item(char *inputdir,char *rmaskerparameters,filelist **myfilelist)
{
  /* Fill in with whatever is relevant to obtain a new unit of work
     suitable to be given to a slave. */
//Input: filedir
//Input: outdir
//TODO: DO I REALLY HAVE TO OPENTHE INPUTDIR AGAIN, ALL I NEED IS IN MYFILELIST
  char *currentfile;
  char *out;
  int i=0;
  int count;
  struct direct **files;
  DIR *dirpointer1;

  currentfile=(char *)malloc(sizeof(char)*512);
  out=(char *)malloc(sizeof(char)*2048);
  count=INPUTSIZECOUNT;
    for (i=0;i<count;i++)
  {

	if (((*myfilelist)[i].finished==0) && ((*myfilelist)[i].started==0)) //if that file hasn't been touched by other processor yet
 	{

		strcpy(currentfile,(*myfilelist)[i].name); //get the file name
		sprintf(out,"%s %s/%s -dir %s \n%s\n%s\n%s\n%s\n%s\n%s",rmaskerparameters,inputdir,currentfile,workingdir,currentfile,workingdir,maskeddir,maskcatdir,maskoutdir,masktbldir);
		(*myfilelist)[i].started=1; //mark that file as started.
		//!!!!!!: WHAT IF THAT FILE IS STARTED AND NEVER FINISHES STARTED FLAG IS GOING TO REMAIN 1,?? Right now only solution is to restart the rmasker_batch
		return out;
      }   
  }

return _NULL; //no files to go but how about the ones that are started but not finished


}

filelist *processtheinputlist_and_initialize(char *inputdir)
{
//scandir routine is from www.cs.cf.ac.uk/Dave/C/node20.html
  int count,i;
  struct direct **files;
  filelist *myfilelist;
  char *outfile1,*outfile2;
  DIR *dfd;
  FILE *tempfp1,*tempfp2;

  outfile1=(char *)malloc(sizeof(char)*512);
  outfile2=(char *)malloc(sizeof(char)*512);


    //check if the dir exists??? ADD MORE ERROR CHECKING HERE!!!!!!
  printf("Current Working Directory = %s\n",inputdir);

  
  if ((dfd=opendir(inputdir))==NULL)
  {
	printf("Can't access %s \nSo I am quitting\n",inputdir);
	exit(0); //

  }
  count = scandir(inputdir, &files, file_select, alphasort);

  INPUTSIZECOUNT=count;
  printf("Number of files in the list are  %d \n",count);
  myfilelist=(filelist *) malloc(sizeof(filelist)*(count+1));


 for (i=0;i<count;i++)
 {
     myfilelist[i].name=(char *)malloc(sizeof(char)*512);
     strcpy(myfilelist[i].name,files[i]->d_name);

   sprintf(outfile1,"%s/%s.masked",maskeddir,myfilelist[i].name);
   sprintf(outfile2,"%s/%s.out",maskoutdir,myfilelist[i].name);


//Only masked and maskout files are enough to see if that is procesed or not. If the sequence has all N's, it only creates masked and mask_out files
	//printf("%s\n%s\n%s\n%s\n",outfile1,outfile2,outfile3,outfile4);
  if (((tempfp1=fopen(outfile1,"r"))==NULL)||((tempfp2=fopen(outfile2,"r"))==NULL))
  {
 //if any of those files are missing, that means that repeatmasker part hasn't completed,so I initialize the flags;

	  printf("PRE PROCESSING : this file  %s  hasn't processed yet\n",files[i]->d_name);
   	  myfilelist[i].finished=0;
          myfilelist[i].started=0;
  }
  else //If all the output is there, it means I don't need to do some repeatmask anymore, so I flag them.
  {
	fclose(tempfp1);
        fclose(tempfp2);
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






