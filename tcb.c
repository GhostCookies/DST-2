#include "tcb.h"

TCB * create_TCB(uint dl){
    TCB * myTCB = (TCB *)calloc(1, sizeof(TCB));
    if (myTCB == NULL) {
        return NULL;
    }
    myTCB->DeadLine = dl;
    return myTCB;
}