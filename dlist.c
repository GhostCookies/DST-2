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
    listobj* pMarker = mylist->pHead;
    if(mylist->pHead==mylist->pTail){
        mylist->pHead->nTCnt=prObj->nTCnt;
        return;
    }
    else if((pMarker->nTCnt) >= (prObj->nTCnt)){
            prObj->pNext = pMarker->pNext;
            prObj->pPrevious = pMarker;
            pMarker->pNext = prObj;
            prObj->pNext->pPrevious = prObj;
    }
    else{
        while(pMarker->pNext != mylist->pTail && pMarker->pNext->nTCnt < prObj->nTCnt){
            pMarker=pMarker->pNext;
        }
        prObj->pNext=pMarker->pNext;
        if(pMarker->pNext!=mylist->pTail){
            prObj->pNext->pPrevious=prObj;
        }
        pMarker->pNext=prObj;
        prObj->pPrevious=pMarker;   
    }
}

listobj * extract(listobj *pObj)
{
	pObj->pPrevious->pNext = pObj->pNext;
	pObj->pNext->pPrevious = pObj->pPrevious;
	pObj->pNext = pObj->pPrevious = NULL;
	return (pObj);
}

