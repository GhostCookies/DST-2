#include <time.h>
#include <stdio.h>
#include "kernel.h"
#include "dlist.h"
#include "tcb.h"
#include "kernel_hwdep.h"
uint TICK;
bool S_MODE=FALSE; //IF FALSE not in start-up mode IF TRUE in start-up mode
list waitingList;
list readyList;
list timerList;
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
    TCB* objt = create_TCB(d);  //Allocate memory for TCB & Set deadline in TCB
    if(objt == NULL){
        return 0;
    }
    objt->PC = body;  //Set the TCB's PC to point to the task body
    objt->SP= &(objt->StackSeg[STACK_SIZE-1]); //Set TCBís SP to point to the stack segment
    if(S_MODE){
        insert//readylist and update order in list
        return 1; //Return status
    }
    else{
        //TODO
        isr_off();
        SaveContext();
        
    }
    
}
void terminate(void){
    
    S_MODE=TRUE;
    isr_on();
}
void run(void){
    //TODO 
    //Initialize interrupt timer
    S_MODE=FALSE;   //Set the kernel in running mode
    isr_on();  //Enable interrupts
    LoadContext();  //Load context
}
// COMMUNICATION

mailbox* create_mailbox(uint nMessages, uint nDataSize){    //DONE????
    mailbox* mailList = (mailbox *)calloc(1, sizeof(mailbox)); //Allocate memory for the Mailbox
    if (mailList == NULL) {
		return 0;
	}
    msg* msgobj = (msg *)calloc(1,sizeof(msg));  //Allocate memory for msg
    if(msgobj == NULL){
        return 0;
    }
    mailList->pHead = msgobj;  
    if (mailList->pHead == NULL) {
		free(mailList);
		return NULL;
    }
    mailList->pTail = msgobj;  
    if(mailList->pTail==NULL){
        free(mailList->pHead);
        free(mailList);
    }
    mailList->pHead->pPrevious = mailList->pHead;   //Initialize Mailbox structure
    mailList->pHead->pNext = mailList->pTail;   //Initialize Mailbox structure
    mailList->pTail->pPrevious = mailList->pHead;   //Initialize Mailbox structure
    mailList->pTail->pNext = mailList->pTail;   //Initialize Mailbox structure
    mailList->nDataSize=nDataSize;  //Initialize Mailbox structure
    mailList->nMessages=nMessages;  //Initialize Mailbox structure
    return mailList;
}
exception remove_mailbox(mailbox* mBox){
    
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
    isr_off();  //Disable interrupt
    SaveContext();  //Save context
    //TODO
    /*
     * IF first execution THEN
            Set: ìnot first execution any moreî
            Place running task in the Timerlist
            Load context
        ELSE
     *       IF deadline is reached THEN
                    Status is DEADLINE_REACHED
                ELSE
            Status is OK
        ENDIF
    ENDIF
    Return status
     */
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
extern void isr_off(void){
    set_isr(ISR_OFF);   //Turns off interrupts
}

extern void isr_on(void){
    set_isr(ISR_OFF);    //Turns on intterrupts
}
extern void SaveContext(void){
    
}
extern void LoadContext(void){
    
}
