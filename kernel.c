#include <time.h>
#include <stdio.h>
#include <limits.h>
#include "kernel.h"
#include "dlist.h"
#include "tcb.h"
#include "msg.c"
#include "kernel_hwdep.h"
uint TICK;
bool S_MODE=FALSE; //IF FALSE not in start-up mode IF TRUE in start-up mode
list * waitingList;
list * readyList;
list * timerList;
TCB * running;

// TASK ADMINISTRATION
int init_kernel(void){
    set_ticks(0);
    waitingList = create_list();
    if(waitingList==NULL){
        return FAIL;
        }
    readyList = create_list();
        if(readyList==NULL){
        return FAIL;
        }
    timerList = create_list();
        if(timerList==NULL){
        return FAIL;
        }
        //create idle task
        create_task(idleTask,UINT_MAX);
        S_MODE = TRUE;
        return OK;

    /*
    Set tick counter to zero - DONE
    Create necessary data structures - ????
    Create an idle task
    Set the kernel in start up mode
    Return status
     */
}
exception create_task(void (* body)(), uint d){
    volatile bool firstTime = TRUE;
    TCB* objt = create_TCB(d);  //Allocate memory for TCB & Set deadline in TCB
    if(objt == NULL){
        return FAIL;
    }
    objt->PC = body;  //Set the TCB's PC to point to the task body
    objt->SP= &(objt->StackSeg[STACK_SIZE-1]); //Set TCBís SP to point to the stack segment
    if(S_MODE){
        listobj * objl = create_listobj(10); // num = nTCnt, unsure of its value
        if(objl == NULL){
            return FAIL;
        }
        objl->pTask = objt;
        insertDeadline(readyList, objl);//readylist and update order in list
        return OK; //Return status
    }
    else{
        //TODO
        isr_off();
        SaveContext();
        if(firstTime){
            firstTime = FALSE;
            listobj * objl = create_listobj(10);
            if(objl == NULL){
                return FAIL;
            }
            objl->pTask = objt;
            insertDeadline(readyList, objl);//readylist and update order in list
            LoadContext();
        }   
        
    }
   return OK; 
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

mailbox* create_mailbox(uint nMessages, uint nDataSize){
    mailbox* mailList = (mailbox *)calloc(1, sizeof(mailbox)); //Allocate memory for the Mailbox
    if (mailList == NULL) {
		return 0;
	}
    msg* msgobj = (msg*) calloc(1,sizeof(msg)); //Allocate memory for the msg obj
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
    mailList->pHead->pPrevious = mailList->pHead;   
    mailList->pHead->pNext = mailList->pTail;   
    mailList->pTail->pPrevious = mailList->pHead;   
    mailList->pTail->pNext = mailList->pTail;   
    mailList->nDataSize=nDataSize;  //Set The size of one Message in the Mailbox 
    mailList->nMessages=nMessages;  //Set the maximum number of Messages the Mailbox can hold.
    return mailList;
}
exception remove_mailbox(mailbox* mBox){
    if(mBox->pHead->pNext==mBox->pTail){    //Checks if mBox is empty
        free(mBox->pTail);  //if empty free memory for the mailbox
        free(mBox->pHead);
        free(mBox);
        return OK;
    }
    else
        return NOT_EMPTY;
}
int no_messages(mailbox* mBox){
    
}
exception send_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off();  //Disable interrupt
    
    SaveContext();
    if(firstTime){
        firstTime = FALSE;
        if(mBox->pHead->pNext!=mBox->pTail){
            mBox->pHead->pNext->pData=pData;
            insertDeadline(readyList,mBox->pHead->pNext->pBlock);
            deleteMessage(mBox->pHead->pNext);
        }
        else{
            msg* msgobj = (msg*) calloc(1,sizeof(msg));
            msgobj->pData=pData;
            mailbox->
        }
        
    }
}

exception receive_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off(); //Disable interrupt
    SaveContext(); //Save context
    if(firstTime){ //IF first execution THEN
        firstTime = FALSE; //Set: not first execution any more
        if(){
            //Copy senders data to receiving tasks data area
            //Remove sending tasks Message struct from the Mailbox
            if(){//IF message was of wait type THEN
            //Move sending task to readyList
            }
            else{
                //Free senders data area
            }
        }
        else{
            //Allocate a Message structure
            //Add Message to the Mailbox
            //Move receiving task from readyList to waitingList
        }
        LoadContext();//Load context
    }
    else{
        if(){//IF deadline is reached THEN
            isr_off();//Disable interrupt
            //Remove recieve Message
            isr_on();//Enable interrupt
            return DEADLINE_REACHED;
        }
        else{
            return OK;
        }
    }
}
exception send_no_wait(mailbox* mBox, void* pData){
    
}
int receive_no_wait(mailbox* mBox, void* pData){
    
}

//TIMING
exception wait(uint nTicks){
    volatile bool firstTime = TRUE;
    isr_off();  //Disable interrupt
    SaveContext();  //Save context
    if(firstTime){ // IF first execution THEN
        firstTime = FALSE;  //Set: not first execution any more
        listobj * objl = extract(readyList->pHead->pNext);
        objl->nTCnt = nTicks;
        insertTimer(waitingList, objl);//Place running task in the Timerlist
        LoadContext(); //Load context
    }
    if(TICK >= running->DeadLine){
        return DEADLINE_REACHED;
    }
    return OK; 
}

void set_ticks(uint no_of_ticks){
    TICK = no_of_ticks;
}
uint ticks(void){
    return TICK;
}
uint deadline(void){
    return running->DeadLine;
}
void set_deadline(uint nNew){
    running->DeadLine = nNew;
}
void idleTask(){
    while(TRUE){}
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
