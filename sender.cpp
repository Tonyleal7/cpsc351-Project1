
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	key_t = key;
	key = ftok("keyfile.txt", 'a');
	if (key<0)
	{
		perror("ftok"); //produces a message on a standard error describing the last error encountered
		//during a call to a system
		exit(-1);// when the function call hits negative 1 it exists
	}

   shmid = shmget (key,SHARED_MEMORY_CHUNK_SIZE,0666 | IPC_CREAT);

	 if (shmid < 0)
	 {
	 perror("shmget");
	 exit (-1);
   }

	 sharedMemPtr = (char*)shmat(shmid,(void*)0,0);

	 if (((void*)sharedMemPtr)<0){
		 perror("shmat");
		 exit (-1);
	 }

  msqid = msgget(key, 0666 |IPC_CREAT);
	if (msqid <0)
	{
		perror("msgget");
		exit(-1);
	}

}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	if (shmdt(sharedMemPtr)<0)
	{
		perror("shmdt");
		exit (-1);
	}
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");


	/* A buffer to store message we will send to the receiver. */
	message sndMsg;

	/* A buffer to store message received from the receiver. */
	message rcvMsg;

	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory.
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}


		/* TODO: Send a message to the receiver telling him that the data is ready
 		 * (message of type SENDER_DATA_TYPE)
 		 */

		 sndMsg.mtype = SENDER_DATA_TYPE;

		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us
 		 * that he finished saving the memory chunk.
 		 */
		 if (msgsnd(msqid, &sndMsg, sizeof(message)- sizeof(long),0)<0)
		 {
			 perror ("msgsend");
			 exit (-1);
		 }

		 do {
			 msgrcv(msqid,&rcvMsg, sizeof(message)-sizeof(long),RECV_DONE_TYPE,0);

		 }while (rcvMsg.mtype != RECV_DONE_TYPE);

	 }

  sndMsg.size = 0;

	if(msgsnd(msqid, &sndMsg,sizeof(message)-sizeof(long),0)<0)
	{
		perror("msgsnd");
		exit(-1);


	}

	fclose(fp);


	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0.
	  */


	/* Close the file */


}



int main(int argc, char** argv)
{

	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}

	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);

	/* Send the file */
	send(argv[1]);

	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
