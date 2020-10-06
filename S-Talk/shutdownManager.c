// Manages the shutdown process of the s-talk program
// Author: Alex Ho and Behrad Bakhsadeh
// Creation Date: 7/18/2020

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "shutdownManager.h"


static pthread_mutex_t shutDownMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t shutDownCond = PTHREAD_COND_INITIALIZER;

// Waits for the mutex to shutdown
void waitForShutdown() {
    if (pthread_mutex_lock(&shutDownMutex) != 0) {
        printf("Mutex lock failed: shutdownManager");
        exit(-1);
    }
    if (pthread_cond_wait(&shutDownCond, &shutDownMutex) != 0) {
        printf("Mutex conditional wait failed, shutdownManager");
        exit(-1);
    }
    if (pthread_mutex_unlock(&shutDownMutex) != 0) {
        printf("Mutex unlocked failed: Sender Thread!");
        exit(-1);
    }
}

// Triggers a shutdown signal to initialize shutdown sequence
void triggerShutdown() {
    
    // Wakes up shutdown initialization
    if (pthread_mutex_lock(&shutDownMutex) != 0) {
        printf("Mutex lock failed: shutdownManager");
        exit(-1);
    }
    if (pthread_cond_signal(&shutDownCond) != 0) {
        printf("Mutex conditional signal failed, shutdownManager");
        exit(-1);
    }
    if (pthread_mutex_unlock(&shutDownMutex) != 0) {
        printf("Mutex unlocked failed: Sender Thread!");
        exit(-1);
    }
    // To synchronize, and let the waitforShutDown function to exit its mutex
    sleep(100);
    
    // Clean up and destroy the mutexes and conditional variables for shutdown
    if (pthread_mutex_destroy(&shutDownMutex) != 0) {
        printf("Mutex Destroy failed (shutdownManager!)");
        exit(-1);
    }
    if (pthread_cond_destroy(&shutDownCond) != 0) {
        printf("Conditional Variable Destroy failed (shutdownManager!");
        exit(-1);
    }
}
