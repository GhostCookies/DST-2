#include "tcb.h"

TCB * create_TCB(uint deadline){
    TCB * myTCB = (TCB *)calloc(1, sizeof(TCB));
    if (myTCB == NULL) {
        return NULL;
    }
    return myTCB;
}