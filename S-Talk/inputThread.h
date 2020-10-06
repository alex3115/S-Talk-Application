// Module of a listener thread for user inputs
// Author: Alex Ho and Behrad Bakhshandeh
// Creation Date: 7/18/2020

#ifndef _INPUTTHREAD_H_
#define _INPUTTHREAD_H_

#include "list.h"
#include <pthread.h>

extern pthread_mutex_t dynamicStringMutex;
extern List* listOfMessages;

void* inputThread();
void Input_inuit(List* messageStorage);
void Input_shutdown();
void decrementListSize();

#endif