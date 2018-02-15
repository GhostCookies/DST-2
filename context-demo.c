// main.c
#include "kernel.h"

void task1(void);
void task2(void);

void isr_off(void){

}

void timer0_start(){
  
}

void isr_on(void){

}

mailbox* mBox;

int main(void)
{
  init_kernel();
  mBox = create_mailbox(1, sizeof(int));
  create_task(task1, 100);
  create_task(task2, 150);
  run();
}

void task1(void)
{
    int a = 420;
    printf("tjenis");
    send_wait(mBox, &a);
}

void task2(void)
{
    int a;
    receive_wait(mBox, &a);
    printf("penis %d", a);
}

