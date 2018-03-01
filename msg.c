#include "msg.h"

void deleteMessage(mailbox * mBox, msg * pMsg, int type){
	pMsg->pPrevious->pNext = pMsg->pNext;
	pMsg->pNext->pPrevious = pMsg->pPrevious;
    mBox->nMessages -= type;    //Decrement/increment the nMessages depending if the mail is sender/receiver
    free(pMsg->pData);
	free(pMsg);
}
mailbox * addToMailbox(mailbox * mBox, msg * pMsg, int type){
    if(abs(mBox->nMessages) == mBox->nMaxMessages){ //Checks if the mBox is full if it is full delets the last message added
        deleteMessage(mBox, mBox->pHead->pNext, type);
    }
    pMsg->pPrevious=mBox->pTail->pPrevious;
    mBox->pTail->pPrevious->pNext=pMsg;
    pMsg->pNext=mBox->pTail;
    mBox->pTail->pPrevious=pMsg;
    mBox->nMessages += type;
}