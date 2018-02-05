#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
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
/*TODO*/        listobj * objl = create_listobj(10); // num = nTCnt, unsure of its value
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
/*TODO*/            listobj * objl = create_listobj(10);
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
    listobj* tempRun = extract(readyList->pHead->pNext);
    free(tempRun->pTask);
    free(tempRun->pMessage);
    free(tempRun);
    running = readyList->pHead->pNext->pTask;
    LoadContext();
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
		return FAIL;
	}
    mailList->pHead = (msg * )calloc(1,sizeof(msg));    //Allocate memory for the Mailbox
    if(mailList->pHead == NULL){
        return NULL;
    }
    mailList->pTail = (msg * )calloc(1,sizeof(msg));    //Allocate memory for the Mailbox
    if(mailList->pTail==NULL){
        return NULL;
    }
    msg* msgobj = (msg*) calloc(1,sizeof(msg)); //Allocate memory for the msg obj
    if(msgobj == NULL){
        return FAIL;
    }
    
     
    mailList->pHead->pPrevious = mailList->pHead;   //Initialize Mailbox structure
    mailList->pHead->pNext = mailList->pTail;       //Initialize Mailbox structure
    mailList->pTail->pPrevious = mailList->pHead;   //Initialize Mailbox structure
    mailList->pTail->pNext = mailList->pTail;       //Initialize Mailbox structure
    
    mailList->pHead->pNext = msgobj; 
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
    if(mBox==NULL){
        return FAIL;
    }
    volatile bool firstTime = TRUE;
    isr_off();  //Disable interrupt
    
    SaveContext();
    if(firstTime){
        firstTime = FALSE;
        if(mBox->pHead->pNext!=mBox->pTail && abs(mBox->nMessages)!=mBox->nMaxMessages){
            *memccpy(mBox->pHead->pNext->pData,pData,sizeof(pData)); //Copy senderís data to the data area of the receivers message
            insertDeadline(readyList,mBox->pHead->pNext->pBlock);   //Move receiving task to Readylist
            deleteMessage(mBox->pHead->pNext);  //Remove receiving taskís Message struct from the mailbox
        }
        else{
            msg* msgobj = (msg*) calloc(1,sizeof(msg)); //Allocate a Message structure
            if(msgobj==NULL){
                return NULL;
            }
            msgobj->pData=pData;    //Set data pointer
            addToMailbox(mBox,msgobj);  //add message to mailbox
            l_obj* sendingTask = mBox->pHead->pNext->pBlock;
            extract(sendingTask);
/*TODO*/            insertDeadline(waitingList,sendingTask);
        }
        LoadContext();
    }
    
    else{
        if(running->DeadLine <= TICK){
            isr_off();  //Disable interrupt
            free(mBox->pHead->pNext->pBlock);  //Remove send Message
            isr_on();
            return DEADLINE_REACHED;
        }
        else
            return OK;
    }
}

exception receive_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off(); //Disable interrupt
    SaveContext(); //Save context
    if(firstTime){ //IF first execution THEN
        firstTime = FALSE; //Set: not first execution any more
        msg* mess = mBox->pHead->pNext;
        if(mess =! mBox->pTail){//IF send Message is waiting THEN
            *memcpy(pData, mess->pData, sizeof(mess->pData));//Copy senders data to receiving tasks data area
            mess->pNext->pPrevious = mess->pPrevious;//Remove sending tasks Message struct from the Mailbox
            mess->pPrevious->pNext = mess->pNext;
            if(mess->pBlock != NULL){//IF message was of wait type THEN
                insertDeadline(readyList, mess->pBlock);//Move sending task to readyList
            }
            else{
                free(mess->pData);//Free senders data area
            }
        }
        else{
            msg* msgobj = (msg*) calloc(1,sizeof(msg));//Allocate a Message structure
            mBox->pHead->pNext = msgobj;//Add Message to the Mailbox
            mBox->pTail->pPrevious = msgobj;
            msgobj->pPrevious = mBox->pHead;
            msgobj->pNext = mBox->pTail;
            listobj* objec = extract(readylist->pHead->pNext);//Move receiving task from readyList to waitingList
/*TODO*/            //insertWait instead of insertDeadline(waitList, objec);
        }
        LoadContext();//Load context
    }
    else{
        if(TICK > (readylist->pHead-pNext->pBlock->DeadLine)){//IF deadline is reached THEN
            isr_off();//Disable interrupt
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
            free(mBox->pHead->pNext->pPrevious);//Remove recieve Message
            mBox->pHead->pNext->pPrevious = mBox->pHead;
            isr_on();//Enable interrupt
            return DEADLINE_REACHED;
        }
        else{
            return OK;
        }
    }
}
exception send_no_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off();
    SaveContext();
    if(firstTime){
        firstTime = FALSE;
        msg* mess = mBox->pHead->pNext;
        if(mess != mBox->pTail){//IF receiving task is waiting THEN
            *memcpy(mess->pData,pData, sizeof(pData));//Copy data to receiving tasks data area
            mess->pPrevious->pNext = mess->pNext;//Remove receiving tasks Message struct from the Mailbox
            mess->pNext->pPrevious = mess->pPrevious;
            insertDeadline(readylist,mess->pBlock);//Move receiving task to readyList
/*TODO*/            //free(mess);
            LoadContext();
        }
        else{
            msg* msgobj = (msg*) calloc(1,sizeof(msg));//Allocate a Message structure
            msgobj->pData = pData;//Copy Data to the Message
            if(mBox->nMaxMessages == mBox->nMessages){//Mailbox is full
                mBox->pHead->pNext = mBox->pHead->pNext->pNext;//Remove the oldest Message struct
                mBox->pHead->pNext->pPrevious = mBox->pHead;
            }
            addToMailbox(mBox, msgobj);//Add Message to the Mailbox
        }
    }
    return OK;
}
exception receive_no_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    volatile exception status = FAIL;
    isr_off();
    SaveContext();  
    if(firstTime){
        firstTime = FALSE;
        msg* mess = mBox->pHead->pNext;
        if(mess != mBox->pTail){//IF send Message is waiting THEN
            status = OK;
            *memcpy(pData, mess->pData, sizeof(mess->pData));//Copy senders data to receiving tasks data area
            mBox->pHead->pNext = mess->pNext;//Remove sending tasks Message struct from Mailbox
            mess->pNext->pPrevious = mBox->pHead;
            if(mess->pBlock != NULL){//IF Message was of wait type THEN 
                insertDeadline(readylist, mess->pBlock); //Move sending task to readyList
            }
            else{
                free(pData);//Free senders data area

            }
        }
        LoadContext();
    }
    return status;//Return status on received Message (not 0!)
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
