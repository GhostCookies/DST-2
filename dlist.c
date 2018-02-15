// dlist.c

#include "dlist.h"

list * create_list()
{
	list * mylist = (list *)calloc(1, sizeof(list));
	if (mylist == NULL) {
		return NULL;
	}

	mylist->pHead = (listobj *)calloc(1, sizeof(listobj));
	if (mylist->pHead == NULL) {
		free(mylist);
		return NULL;
	}

	mylist->pTail = (listobj *)calloc(1, sizeof(listobj));
	if (mylist->pTail == NULL) {
		free(mylist->pHead);
		free(mylist);
		return NULL;
	}
	mylist->pHead->pPrevious = mylist->pHead;
	mylist->pHead->pNext = mylist->pTail;
	mylist->pTail->pPrevious = mylist->pHead;
	mylist->pTail->pNext = mylist->pTail;
	return mylist;
}


listobj * create_listobj(int num)
{
	listobj * myobj = (listobj *)calloc(1, sizeof(listobj));
	if (myobj == NULL)
	{
		return NULL;
	}
	myobj->nTCnt = num;
	return (myobj);
}

void insert(list * mylist, listobj * pObj)
{
	// insert first in list
	listobj *pMarker;
	pMarker = mylist->pHead;

	/* Position found, insert element */
	pObj->pNext = pMarker->pNext;
	pObj->pPrevious = pMarker;
	pMarker->pNext = pObj;
	pObj->pNext->pPrevious = pObj;
}

void insertDeadline(list * mylist, listobj * pObj){// insert first in list
	listobj *pMarker;
	pMarker = mylist->pHead->pNext;
	//Insert catch for NULL?
	while(pMarker != mylist->pTail){
		 if(pObj->pTask->DeadLine >= pMarker->pTask->DeadLine){
			pMarker = pMarker->pNext;
		 }
		 else{
			 break;
		 }
	}
	/* Position found, insert element */
	pObj->pNext = pMarker;
	pObj->pPrevious = pMarker->pPrevious;
	pMarker->pPrevious = pObj;
	pObj->pPrevious->pNext = pObj;
}

void insertTimer(list* mylist,listobj* prObj){
    listobj* pMarker = mylist->pHead->pNext;
    if(pMarker==mylist->pTail){     //If list is empty insert to first node
        prObj->pNext = pMarker;
	prObj->pPrevious = pMarker->pPrevious;
	pMarker->pPrevious = prObj;
	prObj->pPrevious->pNext = prObj;
        return;
    }
    else{
        while(pMarker!=mylist->pTail){
            if(prObj->nTCnt >= pMarker->nTCnt){ //Checks if the prObj has a smaller ntCnt then the next position in the timerlist
                pMarker=pMarker->pNext;
            }
            else
                break;
        }
        prObj->pNext = pMarker;
	prObj->pPrevious = pMarker->pPrevious;
	pMarker->pPrevious = prObj;
	prObj->pPrevious->pNext = prObj;
    }
}

listobj * extract(listobj *pObj)
{
	pObj->pPrevious->pNext = pObj->pNext;
	pObj->pNext->pPrevious = pObj->pPrevious;
	pObj->pNext = pObj->pPrevious = NULL;
	return (pObj);
}

