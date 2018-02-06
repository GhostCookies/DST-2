#include "msg.h"

void deleteMessage(mailbox * mBox, msg * pMsg, int type){
	pMsg->pPrevious->pNext = pMsg->pNext;
	pMsg->pNext->pPrevious = pMsg->pPrevious;
    mBox->nMessages -= type;
    free(pMsg->pData);
	free(pMsg);
}
mailbox * addToMailbox(mailbox * mBox, msg * pMsg, int type){
    if(abs(mBox->nMessages) == mBox->nMaxMessages){
        deleteMessage(mBox, mBox->pHead->pNext, type);
    }
    pMsg->pPrevious=mBox->pTail->pPrevious;
    mBox->pTail->pPrevious->pNext=pMsg;
    pMsg->pNext=mBox->pTail;
    mBox->pTail->pPrevious=pMsg;
    mBox->nMessages += type;
}