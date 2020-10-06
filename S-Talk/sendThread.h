// Module of a sender thread, sending information across the server
// Author: Alex Ho and Behrad Bakhshandeh
// Creation Date: 7/18/2020

#ifndef _SENDTHREAD_H_
#define _SENDTHREAD_H_

#include "list.h"

void freeItem(void* item);
bool isValid(char* msg);
void* sendThread();
void Send_inuit(char* port, char* host, char* target);
void Send_signalInput();
void Send_shutdown();

#endif