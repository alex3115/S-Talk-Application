// File contains the initialization and shutdown of an inputThread, along with its behaviors
// Authors: Alex Ho and Behrad Bakhshadeh
// Creation Date: 7/18/2020

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "sendThread.h"
#include "inputThread.h"
#include "shutdownManager.h"

#define MSG_MAX_LENGTH 512

pthread_t inputThreadPID;

static char* temporaryString = NULL;
pthread_mutex_t dynamicStringMutex = PTHREAD_MUTEX_INITIALIZER;
List* listOfMessages = NULL;
static int getListSize = 0;

// Gets input from user and creates a list item with the input
void* inputThread() {
    while(1) {
        temporaryString = malloc(MSG_MAX_LENGTH);
        while (getListSize != 0);
        fgets(temporaryString, MSG_MAX_LENGTH, stdin);
        
        // lock the thread, accessing list
        if (pthread_mutex_lock(&dynamicStringMutex) != 0) {
            printf("Mutex lock failed: Input Thread!");
            exit(-1);
        }
        // appends the message into a node
        if (List_append(listOfMessages, temporaryString) == -1) {
            printf("List Append Failed, shutting down");
            free(temporaryString);
            temporaryString = NULL;
            exit(-1);
        }
        getListSize++;
        // unlocks the mutex
        if (pthread_mutex_unlock(&dynamicStringMutex) != 0) {
            printf("Mutex unlocked failed: Input Thread!");
            exit(-1);
        }
        // wakes up the sender thread
        Send_signalInput();
    }
    return NULL;
}

// Initializes the input listener thread
void Input_inuit(List* messageStorage) {
    listOfMessages = messageStorage;
    //printf("hello im in input\n");
    if (pthread_create(&inputThreadPID, NULL, inputThread, NULL) != 0) {
        printf("Error creating the Input Thread, program shutting down!");
        exit(-1);
    }
    
    
}

// Shutsdown the input thread
void Input_shutdown() {
    
    if (pthread_cancel(inputThreadPID) != 0) {
        printf("Error cancelling Thread (InputThread)");
        exit(-1);
    }
    if (pthread_join(inputThreadPID, NULL) != 0) {
        printf("Error joining Thread (InputThread)");
        exit(-1);
    }

    // Cleanup
    if (temporaryString) {
        free(temporaryString);
        temporaryString = NULL;
    }
}

// sets the list size of the list of messages
void decrementListSize() {
    getListSize--;
}


