#ifndef _MSG_H_
#define _MSH_H_
#include "kernel.h"

void deleteMessage();
mailbox * addToMailbox(mailbox * mbox, msg * msg);
#endif