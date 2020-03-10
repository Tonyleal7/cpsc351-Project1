#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fsteam>
#include <iostream>
#include "msg.h"    /* For the message struct */
using namespace std;

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

 TODO:
   key_t = key;
	 key = ftok("keyfile.txt", 'a');

	 //equaling it to predefined variable
   shmid = shmget(key,SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT); //Allocates the piece of shared memory
	 //returning the shared identifer associated with key

   if (schmid<0)
	 {
		 perror("shmget");
		 exit (-1);
	 }

	 sharedMemPtr = (char*)shmat(shmid,(void*)0,0);
	 //attaching predefined pointer to the shared memory created above


	 if ((void*)sharedMemPtr) < 0)
	 {
		 perror("shmat");
		 exit(-1);
	 }

	 msqid = mssget(key, 0666 | IPC_CREAT);

}


/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the message */
	int msgSize = 0;

	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

    /* TODO: Receive the message and get the message size. The message will
     * contain regular information. The message will be of SENDER_DATA_TYPE
     * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
     * of the message is not 0, then we copy that many bytes from the shared
     * memory region to the file. Otherwise, if 0, then we close the file and
     * exit.
     *
     * NOTE: the received file will always be saved into the file called
     * "recvfile"
     */

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */

	while(msgSize != 0)
	{
		message msg1;
		if(msgrcv(msqid,&msg1, sizeof(message) - sizeof(long), SENDER_DATA_TYPE,0)<0)

		{
			perror("msgsnd");
			fclose(fp);
			exit(-1);
		}

		msgSize = msg1.size;
		/* If the sender is not telling us that we are done, then get to work */

		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}

			message = msgDone;
			msgDone.mtype = RECV_DONE_TYPE;
      //checking the message size from the sender to make sure that there is still information to read
		  if(msgsnd(msqid, &msgDone, sizeof(message), - sizeof(long), 0) < 0)
			{
				perror("msgsnd");
				exit(-1);
			}
		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	}
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	if(shmdt(sharedMemPtr)<0)
	{
		perror("shmdt");
		exit(-1);
	}

	if (shmctl(shmid, IPC_RMID,NULL ) < 0)
	{
		perror ("shmctl");
		exit(-1);
	}

	if (msgctl(msqid, IPC_RMID, NULL) < 0)
	{
		perror("msgctl");
	}
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{

	signal(SIGINIT, ctrlCSignal);
	//The signal that you want to catch in the program

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();

	//creates the new file and stores everything in it. Unlike the first txt file made manually
	//this file has to be created within the code

	string thelines;
	ifstream myfile ("recvfile");
	if (myfile.is_open())
	{
		cout << thelines << endl;
	}
	myfile.close();

	return 0;

	cleanUp(shmid, msqid, sharedMemPtr);
}
