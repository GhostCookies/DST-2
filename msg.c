#include "msg.h"

void deleteMessage(msg *pObj)
{
	pObj->pPrevious->pNext = pObj->pNext;
	pObj->pNext->pPrevious = pObj->pPrevious;
	free(pObj);
        
}
mailbox * addToMailbox(mailbox * mbox, msg * msg){
    msg->pPrevious=mbox->pTail->pPrevious
    mbox->pTail->pPrevious->pNext=msg;
    msg->pNext=mbox->pTail;
    mbox->pTail->pPrevious=msg;
}