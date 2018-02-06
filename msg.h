#ifndef _MSG_H_
#define _MSH_H_
#include "kernel.h"

void deleteMessage(mailbox * mbox, msg * msg, int type);
mailbox * addToMailbox(mailbox * mbox, msg * msg, int type);
#endif