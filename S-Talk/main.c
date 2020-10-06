// Constructs a s-talk application that sends and receivers through and from the server
// Authors: Alex Ho and Behrad Bakhsadeh
// Creation Date: 07/18/2020

#include <pthread.h>
#include <stdio.h>

#include "list.h"
#include "inputThread.h"
#include "shutdownManager.h"
#include "sendThread.h"
#include "receiver.h"
#include "output.h"

int main(int argc,char* argv[]) {
    List* lom = List_create();
    List* lom2 = List_create();
    
    printf("Starting.....");
    
    Input_inuit(lom);
    Send_inuit(argv[1], argv[2], argv[3]);
    Receiver_init(argv[1], argv[2], argv[3], lom2);
    Output_init();
    
    waitForShutdown();

    Input_shutdown();
    Send_shutdown();
    Receiver_shutdown();
    Output_shutdown();
}
