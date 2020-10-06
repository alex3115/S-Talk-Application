#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <signal.h>
#include <unistd.h>



#include "receiver.h"
#include "list.h"
#include "output.h"
#include "shutdownManager.h"
#include "sendThread.h"

#define DYNAMIC_LEN 512
#define MSG_MAX_LEN 512

static pthread_t threadPID2;
static pthread_cond_t wakeupsignal= PTHREAD_COND_INITIALIZER;
static pthread_mutex_t wakeupmutex= PTHREAD_MUTEX_INITIALIZER;

static char* Msg=NULL;

void* outputThread(void *arg)
{
   while(1)
   {    
        if (pthread_mutex_lock(&wakeupmutex) != 0) {
            printf("Mutex lock failed: output Thread!");
        }
        if (pthread_cond_wait(&wakeupsignal, &wakeupmutex) != 0) {
            printf("Mutex conditional wait failed, output Thread!");
        }
        if (pthread_mutex_unlock(&wakeupmutex) != 0) {
            printf("Mutex unlocked failed: output Thread!");
        }

        //thread wakes up
        if (pthread_mutex_lock(&ListMutex) != 0) {
            printf("Mutex lock failed: Sender Thread!");
        }
        
        Msg=List_trim(Listmsg);
        
      // printf("puuting it into screen %s \n",Msg);
        //fputs(Msg,stdout);

        if (isValid(Msg)) {
            fputs(Msg,stdout);
            fflush(stdout);
            if (Msg){
                free(Msg);
                Msg = NULL;
            }
        }
        else {
            fputs("! received, program shutting down", stdout);
            fflush(stdout);
            triggerShutdown();   
        }
         
        if (pthread_mutex_unlock(&ListMutex) != 0) {
            printf("Mutex unlocked failed: Sender Thread!");
        }

    }

    return NULL;
}



void signal_cond()
{
    
    if (pthread_mutex_lock(&wakeupmutex) != 0) {
        printf("Mutex lock failed: output Thread!");
    }
    if (pthread_cond_signal(&wakeupsignal) != 0) {
        printf("Mutex signal failed (output Thread)!");
    }
    if (pthread_mutex_unlock(&wakeupmutex) != 0) {
        printf("Mutex unlocked failed: output Thread!");
    }


}



void Output_init(void)
{

	
    if(pthread_create(&threadPID2,NULL,outputThread,NULL)!=0)
    {
        printf("could not create output thread\n");
    }

}

void Output_shutdown(void)
{
    // Cancel thread
    pthread_cancel(threadPID2);

    // Waits for thread to finish
    pthread_join(threadPID2, NULL);

    
    // Cleanup memory
   
    // Shutdown ListMutex 
    if (pthread_mutex_trylock(&ListMutex) == 0) {
        if (pthread_mutex_unlock(&ListMutex) != 0) {
            printf("Mutex unlocked failed: Output Thread!");
        }
        if (pthread_mutex_destroy(&ListMutex) != 0) {
            printf("List Mutex destroyed failed: Output Thread!");
        }
    }
    else {
        if (pthread_mutex_unlock(&ListMutex) != 0) {
            printf("Mutex unlocked failed: Output Thread!");
        }
        if (pthread_mutex_destroy(&ListMutex) != 0) {
            printf("List Mutex destroyed failed: Output Thread!");
        }
    }

    // Shut down WakeupMutex
    if (pthread_mutex_trylock(&wakeupmutex) == 0) {
        if (pthread_mutex_unlock(&wakeupmutex) != 0) {
            printf("Mutex unlocked failed: Output Thread!");
        }
        if (pthread_mutex_destroy(&wakeupmutex) != 0) {
            printf("Wakeup Mutex destroyed failed: Output Thread!");
        }
    }
    else {
        if (pthread_mutex_unlock(&wakeupmutex) != 0) {
            printf("Mutex unlocked failed: Output Thread!");
        }
        if (pthread_mutex_destroy(&wakeupmutex) != 0) {
            printf("Wakeup Mutex destroyed failed: Output Thread!");
        }
    }
    // Destroy Receive and Output synchronization conditional variable
    if (pthread_cond_destroy(&wakeupsignal) != 0) {
        printf("Conditional Variable Destroy failed (Output Thread!");
    }

    // Additional memory management
    if (Msg) {
        free(Msg);
        Msg = NULL;
    }
    List_free(Listmsg, freeItem);
}
