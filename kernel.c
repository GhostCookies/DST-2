#include <time.h>
#include "kernel.h"
#include "dlist.c"
uint TICK;
bool MODE=FALSE; //IF FALSE not in start-up mode IF TRUE in start-up mode
// TASK ADMINISTRATION
int init_kernel(void){
    set_ticks(0);
    
    
    /*
    Set tick counter to zero - DONE
    Create necessary data structures - ????
    Create an idle task
    Set the kernel in start up mode
    Return status
     */
}
exception create_task(void (* body)(), uint d){
    TCB* objt = createTcbObj(d);  //Allocate memory for TCB & Set deadline in TCB
    objt->PC = body;  //Set the TCB's PC to point to the task body
    objt->SP= &(objt->StackSeg[STACK_SIZE-1]); //Set TCBís SP to point to the stack segment
    if(MODE){
        //readylist and update order in list
        return 0; //Return status
    }
    else{
        //TODO
        isr_off();
        SaveContext();
        if()
    }
    
}
void terminate(void){
    
}
void run(void){
    
}
// COMMUNICATION

mailbox* create_mailbox(uint nMessages, uint nDataSize){
    
}
int no_messages(mailbox* mBox){
    
}
exception send_wait(mailbox* mBox, void* pData){
    
}

exception receive_wait(mailbox* mBox, void* pData){
    
}
exception nd_no_wait(mailbox* mBox, void* pData){
    
}
int receive_no_wait(mailbox* mBox, void* pData){
    
}

//TIMING
exception wait(uint nTicks){
    
}
void set_ticks(uint no_of_ticks){
    TICK = no_of_ticks;
}
uint ticks(void){
    return TICK;
}
uint deadline(void){
    
}
void set_deadline(uint nNew){
    
}

//INTERRUPT

extern void isr_on(void){
    
}
extern void SaveContext(void){
    
}
extern void LoadContext(void){
    
}