#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "kernel.h"
#include "dlist.h"
#include "tcb.h"
#include "msg.h"
#include "kernel_hwdep.h"

#define EXPIRED_DEADLINE 2 //If deadline is reached for a message

uint TICK;
bool S_MODE=FALSE; //IF FALSE not in start-up mode IF TRUE in start-up mode
list * waitingList;
list * readyList;
list * timerList;
TCB * Running;

void idleTask(){
    while(TRUE){}
}

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
    if(readyList->pHead->pNext->pTask->DeadLine!=UINT_MAX){
        listobj* tempRun = extract(readyList->pHead->pNext);
        free(tempRun->pTask);
        free(tempRun->pMessage);
        free(tempRun);
        Running = readyList->pHead->pNext->pTask;
        LoadContext();
    }
}

void run(void){
    timer0_start(); //Initialize interrupt timer
    S_MODE=FALSE;   //Set the kernel in Running mode
    isr_on();  //Enable interrupts
    LoadContext();  //Load context
}
// COMMUNICATION

mailbox* create_mailbox(uint nMaxMessages, uint nDataSize){
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
     
    mailList->pHead->pPrevious = mailList->pHead;   //Initialize Mailbox structure
    mailList->pHead->pNext = mailList->pTail;       //Initialize Mailbox structure
    mailList->pTail->pPrevious = mailList->pHead;   //Initialize Mailbox structure
    mailList->pTail->pNext = mailList->pTail;       //Initialize Mailbox structure
    mailList->nDataSize=nDataSize;  //Set The size of one Message in the Mailbox 
    mailList->nMaxMessages=nMaxMessages;  //Set the maximum number of Messages the Mailbox can hold.
    mailList->nMessages = 0;
    mailList->nBlockedMsg = 0;
    return mailList;
}

exception remove_mailbox(mailbox* mBox){
    if(mBox->nMessages == 0){    //Checks if mBox is empty
        free(mBox->pTail);  //if empty free memory for the mailbox
        free(mBox->pHead);
        free(mBox);
        return OK;
    }
    else
        return NOT_EMPTY;
}

exception send_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off();  //Disable interrupt
    SaveContext();
    if(firstTime){
        firstTime = FALSE;
        msg* nMsg = mBox->pHead->pNext;

        while(nMsg->Status == EXPIRED_DEADLINE){
            deleteMessage(mBox, nMsg, (mBox->nMessages > 0) - (mBox->nMessages < 0));
            nMsg = mBox->pHead->pNext;
        }

        if(mBox->nMessages < 0){
            memcpy(nMsg->pData,pData,sizeof(pData)); //Copy sender's data to the data area of the receivers message
            listobj* receivingTask = extract(nMsg->pBlock);
            insertDeadline(readyList, receivingTask);   //Move receiving task to Readylist
            deleteMessage(mBox, nMsg, RECEIVER);  //Remove receiving task's Message struct from the mailbox
        }
        else{
            msg* mObj;
            mObj = (msg*) calloc(1,sizeof(msg)); //Allocate a Message structure
            if(mObj==NULL){
                return FAIL;
            } 
            mObj->pData=pData;    //Set data pointer
            mObj->pBlock = readyList->pHead->pNext; //add Running task to message
            mObj->pBlock->pMessage = mObj; //add message to Running task
            addToMailbox(mBox,mObj, SENDER);  //add message to mailbox
            listobj* sendingTask = mObj->pBlock;
            extract(sendingTask);
            insertDeadline(waitingList,sendingTask);
        }
        LoadContext();
    }
    else{
        if(mBox->nMessages > 0){
            if(TICK >= mBox->pTail->pPrevious->pBlock->pTask->DeadLine){
                isr_off();  //Disable interrupt
                deleteMessage(mBox, mBox->pTail->pPrevious, SENDER);//Remove send Message
                isr_on();
                return DEADLINE_REACHED;
            }
        }
    return OK;
    }
return FAIL;
}

exception receive_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off(); //Disable interrupt
    SaveContext(); //Save context
    if(firstTime){ //IF first execution THEN
        firstTime = FALSE; //Set: not first execution any more
        msg* nMsg = mBox->pHead->pNext;

        while(nMsg->Status == EXPIRED_DEADLINE){
            deleteMessage(mBox, nMsg, (mBox->nMessages > 0) - (mBox->nMessages < 0));
            nMsg = mBox->pHead->pNext;
        }

        if(mBox->nMessages > 0){//IF send Message is waiting THEN
            memcpy(pData, nMsg->pData, sizeof(nMsg->pData));//Copy senders data to receiving tasks data area
            deleteMessage(mBox, nMsg, SENDER); //Remove sending tasks message struct from mailbox
            if(nMsg->pBlock != NULL){//IF message was of wait type THEN
                listobj* sendingTask = extract(nMsg->pBlock);
                insertDeadline(readyList, sendingTask);//Move sending task to readyList
            }
            else{
                free(nMsg->pData);//Free senders data area
            }
        }
        else{
            msg* msgobj = (msg*) calloc(1,sizeof(msg));//Allocate a Message structure
            msgobj->pBlock = readyList->pHead->pNext; //add Running task to message
            msgobj->pBlock->pMessage = msgobj; //add message to Running task
            addToMailbox(mBox,msgobj, RECEIVER);//Add Message to the Mailbox
            listobj* objec = extract(msgobj->pBlock);//Move receiving task from readyList to waitingList
            insertDeadline(waitingList, objec);
        }
        LoadContext();//Load context
    }
    else{
        if(mBox->nMessages < 0){
            if(TICK >= mBox->pTail->pPrevious->pBlock->pTask->DeadLine){//IF deadline is reached THEN
                isr_off();//Disable interrupt
                deleteMessage(mBox, mBox->pTail->pPrevious, RECEIVER);
                isr_on();//Enable interrupt
                return DEADLINE_REACHED;
            }
        }
    return OK;
    }
return FAIL;
}
exception send_no_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off();
    SaveContext();
    if(firstTime){
        firstTime = FALSE;
        msg* nMsg = mBox->pHead->pNext;

        while(nMsg->Status == EXPIRED_DEADLINE){
            deleteMessage(mBox, nMsg, (mBox->nMessages > 0) - (mBox->nMessages < 0));
            nMsg = mBox->pHead->pNext;
        }

        if(mBox->nMessages < 0){//IF receiving task is waiting THEN
            memcpy(nMsg->pData,pData, sizeof(pData));//Copy data to receiving tasks data area
            listobj* receivingTask = extract(nMsg->pBlock);
            deleteMessage(mBox, nMsg, RECEIVER);//Remove receiving tasks Message struct from the Mailbox
            insertDeadline(readyList,receivingTask);//Move receiving task to readyList
            LoadContext();
        }
        else{
            msg* msgobj = (msg*) calloc(1,sizeof(msg));//Allocate a Message structure
            if(msgobj == NULL){
                return FAIL;    
            }
            msgobj->pData = pData;//Copy Data to the Message
            addToMailbox(mBox, msgobj, SENDER);//Add Message to the Mailbox
        }
    }
    return OK;
}
exception receive_no_wait(mailbox* mBox, void* pData){
    volatile bool firstTime = TRUE;
    isr_off();
    SaveContext();  
    if(firstTime){
        firstTime = FALSE;
        msg* nMsg = mBox->pHead->pNext;

        while(nMsg->Status == EXPIRED_DEADLINE){
            deleteMessage(mBox, nMsg, (mBox->nMessages > 0) - (mBox->nMessages < 0));
            nMsg = mBox->pHead->pNext;
        }

        if(mBox->nMessages > 0){//IF send Message is waiting THEN
            memcpy(pData, nMsg->pData, sizeof(nMsg->pData));//Copy senders data to receiving tasks data area
            if(nMsg->pBlock != NULL){//IF Message was of wait type THEN 
                listobj* sendingTask = extract(nMsg->pBlock);
                insertDeadline(readyList, sendingTask); //Move sending task to readyList
            }
        deleteMessage(mBox, nMsg, SENDER); // free senders data area
        }
        else{
            return FAIL;
        }
        LoadContext();
    }
    return OK;//Return status on received Message
}

//TIMING
exception wait(uint nTicks){
    volatile bool firstTime = TRUE;
    isr_off();  //Disable interrupt
    SaveContext();  //Save context
    if(firstTime){ // IF first execution THEN
        firstTime = FALSE;  //Set: not first execution any more
        listobj * objl = extract(readyList->pHead->pNext);
        objl->nTCnt = TICK + nTicks;
        insertTimer(timerList, objl);//Place Running task in the Timerlist
        LoadContext(); //Load context
    }
    if(TICK >= Running->DeadLine){
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
    return Running->DeadLine;
}
void set_deadline(uint nNew){
    volatile bool firstTime = TRUE;
    isr_off();
    SaveContext();
    if(firstTime){
        firstTime = FALSE;
        Running->DeadLine = (nNew + TICK);
        //TODO Reschedule Readylist
        listobj* runningTask = extract(readyList->pHead->pNext);
        insertDeadline(readyList, runningTask);
        LoadContext();
    } 
}

void TimerInt(void){
    TICK++;
    while((timerList->pHead->pNext->nTCnt <= TICK) && (timerList->pHead->pNext != timerList->pTail)){
        listobj* toReadyList = extract(timerList->pHead->pNext);
        insertDeadline(readyList,toReadyList);
    }
    while((waitingList->pHead->pNext->pTask->DeadLine <= TICK) && (waitingList->pHead->pNext != waitingList->pTail)){
        listobj* toReadyList = extract(waitingList->pHead->pNext);
        toReadyList->pMessage->Status = EXPIRED_DEADLINE; // 2 = timer exprired
        insertDeadline(readyList,toReadyList);
    }
}

//INTERRUPT
extern void isr_off(void){
    set_isr(ISR_OFF);   //Turns off interrupts
}

extern void isr_on(void){
    set_isr(ISR_ON);    //Turns on intterrupts
}
