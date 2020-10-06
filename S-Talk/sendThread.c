// File contains the initialization and shutdown of a sender thread, along with its behaviours
// Authors: Alex Ho and Behrad Bakhsadeh
// Creation Date: 7/18/2020

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "sendThread.h"
#include "inputThread.h"
#include "shutdownManager.h"

#define MSG_MAX_LENGTH 512

pthread_t senderThreadPID;

static char* portNumber;
static char* hostName;
static char* targetPortNumber;
static char* message;

static pthread_mutex_t senderInputSynchronizationMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t senderInputSynchronizationConditionalVar = PTHREAD_COND_INITIALIZER;
static struct addrinfo *res;
static int socketDescriptor;
static int uninitialized = 0;

// frees the memory of an item
void freeItem(void* item) {
    if (item) {
        free(item);
        item = NULL;
    }
}

// checks if the string is valid
bool isValid(char* msg) {
    bool exclamationDetected = false;
    for(int i = 0; i < strlen(msg) - 1; i++) {
        if (msg[i] != '!' && msg[i] != ' ') {
            return true;
        }
        else if (msg[i] == '!' && exclamationDetected == false) {
            exclamationDetected = true;
        }
        else if (msg[i] == '!' && exclamationDetected == true) {
            return true;
        }
    }
    if (exclamationDetected == true) {
        return false;
    }
    return true;
}

// Takes a message off the list, and sends it across the server
void* sendThread() {
    // Set up the connection to the server
	
	struct addrinfo hints, *p;
	
	int numbytes;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(hostName, targetPortNumber, &hints, &res) != 0) {
		printf("failed to get addrinfo senderthread");
        exit(-1);
	}

	// loop through all the results and make a socket
	for(p = res; p != NULL; p = p->ai_next) {
		if ((socketDescriptor = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			printf("Error connecting to socket, Send Thread");
			continue;
		}

		break;
	}

    
    
    
   
    while(1) {
        // Puts thread on wait for input thread
        if (pthread_mutex_lock(&senderInputSynchronizationMutex) != 0) {
            printf("Mutex lock failed: Sender Thread!");
            exit(-1);
        }
        uninitialized = 1;
        if (pthread_cond_wait(&senderInputSynchronizationConditionalVar, &senderInputSynchronizationMutex) != 0) {
            printf("Mutex conditional wait failed, Sender Thread!");
            exit(-1);
        }
        if (pthread_mutex_unlock(&senderInputSynchronizationMutex) != 0) {
            printf("Mutex unlocked failed: Sender Thread!");
            exit(-1);
        }

        // Wakes up and send list over
        if (pthread_mutex_lock(&dynamicStringMutex) != 0) {
            printf("Mutex lock failed: Sender Thread!");
            exit(-1);
        }
        
        message = List_trim(listOfMessages);
        if ((numbytes = sendto(socketDescriptor, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1) {
		printf("Send failure, Sender Thread!");
            exit(-1);
	    }
        decrementListSize();
        
        // printf("sending message over %s \n", message);

        // Check if message is valid
        if (isValid(message)) {
            free(message);
            message = NULL;
        }
        else {
            free(message);
            message = NULL;
            triggerShutdown();
        }
        // unlocks the mutex
        if (pthread_mutex_unlock(&dynamicStringMutex) != 0) {
            printf("Mutex unlocked failed: Sender Thread!");
            exit(-1);
        }
    }
    return NULL;
}

// Initializes a sender thread
void Send_inuit(char* port, char* host, char* target) {
    portNumber = port;
    hostName = host;
    targetPortNumber = target;

    if (pthread_create(&senderThreadPID, NULL, sendThread, NULL) != 0) {
        printf("Error creating the Sender Thread, program shutting down!");
        exit(-1);
    }
}

// Signals the sender thread to wake up (done when the input thread finishes adding an item onto the list)
void Send_signalInput() {
    while(uninitialized != 1);
    if (pthread_mutex_lock(&senderInputSynchronizationMutex) != 0) {
        printf("Mutex lock failed: Input Thread!");
        exit(-1);
    }
    if (pthread_cond_signal(&senderInputSynchronizationConditionalVar) != 0) {
        printf("Mutex signal failed (Input Thread)!");
        exit(-1);
    }
    if (pthread_mutex_unlock(&senderInputSynchronizationMutex) != 0) {
        printf("Mutex unlocked failed: Input Thread!");
        exit(-1);
    }
}
void Send_shutdown() {

    if (pthread_cancel(senderThreadPID) != 0) {
        printf("Error cancelling Thread (Sender Thread)");
        exit(-1);
    }
    if (pthread_join(senderThreadPID, NULL) != 0) {
        printf("Error joining Thread (Sender Thread)");
        exit(-1);
    }

    // Clean up

    // Shutdown Dynmaic String Mutex
    if (pthread_mutex_trylock(&dynamicStringMutex) == 0) {
        if (pthread_mutex_unlock(&dynamicStringMutex) != 0) {
            printf("Mutex unlocked failed: Input Thread!");
        }
        if (pthread_mutex_destroy(&dynamicStringMutex) != 0) {
            printf("Dynamic String Mutex destroyed failed: Sender Thread");
        }
    }
    else {
        if (pthread_mutex_unlock(&dynamicStringMutex) != 0) {
            printf("Mutex unlocked failed: Input Thread!");
        }
        if (pthread_mutex_destroy(&dynamicStringMutex) != 0) {
            printf("Dynamic String Mutex destroyed failed: Sender Thread");
        }
    }

    // Shutdown SenderInput Synchronization Mutex
    if (pthread_mutex_trylock(&senderInputSynchronizationMutex) == 0) {
        if (pthread_mutex_unlock(&senderInputSynchronizationMutex) != 0) {
            printf("Mutex unlocked failed: Input Thread!");
        }
        if (pthread_mutex_destroy(&senderInputSynchronizationMutex) != 0) {  
            printf("Sender Synchronization Mutex destroy failed: Sender Thread!");
        }
    }
    else {
        if (pthread_mutex_unlock(&senderInputSynchronizationMutex) != 0) {
            printf("Mutex unlocked failed: Input Thread!");
        }
        if (pthread_mutex_destroy(&senderInputSynchronizationMutex) != 0) {  
            printf("Sender Synchronization Mutex destroy failed: Sender Thread!");
        }
    }

    // Destroy conditional variable between Sender and Input Thread
    if (pthread_cond_destroy(&senderInputSynchronizationConditionalVar) != 0) {
        printf("Conditional Variable Destroy failed (Sender Thread!");
        exit(-1);
    }

    // Additional Memory Management
    freeaddrinfo(res);
    close(socketDescriptor);
    if (message) {
        free(message);
        message = NULL;
    }
    List_free(listOfMessages, freeItem);
}

