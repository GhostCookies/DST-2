#include "msg.h"

msg* create_MSG(){
     msg * myMSG = (msg *)calloc(1, sizeof(msg));
    if (myMSG == NULL) {
        return NULL;
    }
     
    return myMSG;
}
