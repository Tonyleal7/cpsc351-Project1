#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>	// std::ofstream
#include <string>
#include "msg.h"    // for the message struct
using namespace std;	//std::cout as cout

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile.txt";

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
  	  so manually or from the code).*/

	/* 2. Use ftok("keyfile.txt", 'a') in order to generate the key. */
	key_t key = ftok("keyfile.txt", 'a');
	if(key < 0){
		perror("ftok");
		exit(-1); // abnormal termination
	}
    else printf("Key Validated \n");
    
	/* 3. Use the key in the TODO's below. Use the same key for the queue
	  and the shared memory segment. This also serves to illustrate the difference
	  between the key and the id used in message queues and shared memory. The id
	  for any System V object (i.e. message queues, shared memory, and sempahores)
	  is unique system-wide among all System V objects. Two objects, on the other hand,
	  may have the same key. */

	/* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE,0644 | IPC_CREAT);

	// validate shared memory id
	if(shmid < 0){
		perror("shmget");
		exit(-1); // abnormal termination
	}
    else printf("Shared memory validated\n");

	/* TODO: Attach to the shared memory */
	sharedMemPtr =shmat(shmid, (void*) 0, 0);

	/* validate shared memory pointer */
	if(sharedMemPtr < (void*)0){
		perror("shmat");
		exit(-1);
	}
    else printf("Shared memory pointer validated \n");

	/* TODO: Create a message queue */
	msqid = msgget(key,0644 | IPC_CREAT);

	/* validate message queue */
	if(msqid < 0)
	{
		perror("msgget");
		exit(-1);
	}
    else printf("Message queue validated\n");

	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
}


/**
 * The main loop
 */
 message sndMsg;
	message rcvMsg;
void mainLoop()
{
	/* The size of the message */
	int msgSize = 0;



	/* Open the file for writing */
	FILE* fptr = fopen(recvFileName, "w");

	/* Error checks */
	if(!fptr)
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
	* "recvfile" */

	/* Keep receiving until the sender set the size to 0, indicating that
	* there is no more data to send */

    
    
    
	do
	{
        if(msgrcv(msqid, &sndMsg, sizeof(sndMsg), SENDER_DATA_TYPE, 0) < 0){
            perror("msgsnd");
            exit(-1);
        }
        else {
            printf("Received the message and message size \n");
        }
		 msgSize = sndMsg.size;

		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fptr) < 0)
			{
				perror("fwrite");
			}
            else {
                printf("Shared memory to file \n");
                
            }

			/* TODO: Tell the sender that we are ready for the next file chunk.
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case). */
			sndMsg.mtype = RECV_DONE_TYPE;
			sndMsg.size = 0;

			if(msgsnd(msqid, &sndMsg, 0, 0) < 0){
				perror("msgsnd");
			}
            else {
                printf("Ready for the next file chunk \n");
            }
            
           

		}
		/* We are done */
		else
		{
            printf("File Closed \n");
			/* Close the file */
			fclose(fptr);
		}
    } while (msgSize != 0);
    
    
}

/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	if(shmdt(sharedMemPtr) < 0){
		perror("shmdt");
		exit(-1);
	}
    else printf("detached from shared memory \n");

	/* TODO: Deallocate the shared memory chunk */
	if(shmctl(shmid, IPC_RMID, NULL) < 0){
		perror("shmctl");
		exit(-1);
	}
    else printf("deallocated the shared memory chunk \n");

	/* TODO: Deallocate the message queue */
	if(msgctl(msqid, IPC_RMID, NULL) < 0){
		perror("msgctl");
		exit(-1);
	}
    printf("dellocated the message queue \n");
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

	/* TODO: Install a singnal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();

	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
	cleanUp(shmid, msqid, sharedMemPtr);
	return 0;
}
