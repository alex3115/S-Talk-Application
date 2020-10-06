#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>



#include "receiver.h"
#include "list.h"
#include "output.h"
#include "shutdownManager.h"

#define DYNAMIC_LEN 512
#define MSG_MAX_LEN 512



static pthread_t threadPID;


List *Listmsg=NULL;
pthread_mutex_t ListMutex = PTHREAD_MUTEX_INITIALIZER;
static char* receiveport=NULL;
static char* sendip=NULL;
static char* sendport=NULL;

static int sockfd;

static char *MsgReceived=NULL;


void* receiveThread(void* arg)
{
    int numbytes;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(receiveport));
    addr.sin_addr.s_addr = INADDR_ANY;

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error connecting to server socket, Receiver Thread!");
        exit(-1);
    }
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("Error binding to server socket! Receiver Thread");
        exit(-1);
    }


	while (1) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		struct sockaddr_in sinRemote;
		unsigned int sin_length = sizeof(sinRemote);
        
	    // Dynamically allocate a message
        MsgReceived=malloc(MSG_MAX_LEN);
        if ((numbytes = recvfrom(sockfd, MsgReceived, MSG_MAX_LEN , 0, (struct sockaddr *)&sinRemote, &sin_length)) == -1) {
            printf("failed to recieve message\n");
        }
	
        // Do something amazing with the received message!
        pthread_mutex_lock(&ListMutex);
        {
            
            List_append(Listmsg,MsgReceived);

        }
        pthread_mutex_unlock(&ListMutex);


        //wakeup output thread
        signal_cond();

	}
    

    // NOTE NEVER EXECUTES BECEAUSE THREAD IS CANCELLED
	return NULL;
}







void Receiver_init( char* port,char* ipaddress, char* senderport,List * receiveList)
{
    receiveport = port;
    sendip = ipaddress;
    sendport = senderport;

    Listmsg = receiveList;

    if(pthread_create( &threadPID, NULL,receiveThread,NULL)!=0)
        {
            printf("failed to create receiver thread\n");
        }
}


void Receiver_shutdown(void)
{
    // Cancel thread
    pthread_cancel(threadPID);
    
    // Waits for thread to finish
    pthread_join(threadPID, NULL);


    close(sockfd);
    free(MsgReceived);
    MsgReceived = NULL;

}

