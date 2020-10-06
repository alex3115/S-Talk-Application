

#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include "list.h"
#include <pthread.h>

 extern pthread_mutex_t ListMutex;
 extern List *Listmsg;

// Start background receive thread
void Receiver_init(char* port,char* ipaddress, char* senderport,List * receiveList);
//void Receiver_init(char* rxMessage);


// Stop background receive thread and cleanup
void Receiver_shutdown(void);








#endif
