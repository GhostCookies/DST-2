#include "msg.h"

msg* create_MSG(void* pData){
     msg * myMSG = (msg *)calloc(1, sizeof(msg));
    if (myMSG == NULL) {
        return NULL;
    }
     myMSG->pData = pData;
     
    return myMSG;
}
