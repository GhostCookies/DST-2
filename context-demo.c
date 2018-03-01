// main.c
#include "kernel.h"
#define TEST_PATTERN_1 0xAA
#define TEST_PATTERN_2 0x55

mailbox *mb;
void task1(void);
void task2(void);
void task11(void);
void task22(void);
void task33(void);
void task34(void);
void isr_off(void){

}

void isr_on(void){

}
void timer0_start(){
}
mailbox* mBox;
int nTest1=0, nTest2=0, nTest3=0;
int main(void)
{
  
  int test = 2;
  if(test==3){
    init_kernel();
    mb = create_mailbox(1000, sizeof(int));
    create_task(task33,300);
    create_task(task34,200);
  }
  if(test == 1){ //Test 1
    init_kernel();
    mBox = create_mailbox(1, sizeof(int));
    create_task(task1, 100);
    create_task(task2, 150);
    run();
  }
  
  if(test == 2){ //Test 2
      int nTest3 =0;
      int nTest2 =0;
      int nTest1 =0;
      if (init_kernel() != OK ) {   /* Memory allocation problems */
        while(1);
      }
     if (create_task( task11, 2000 ) != OK ) {   /* Memory allocation problems */
        while(1);
     }
     if (create_task( task22, 4000 ) != OK ) {   /* Memory allocation problems */
     while(1);
     }
     if ( (mb=create_mailbox(1,sizeof(int))) == NULL) { /* Memory allocation problems */
        while(1);
     }
     run(); /* First in readylist is task1 */
    }
   run();
}

void task1(void)
{
    int a = 420;
    //int a = 1;
    printf("tjenis");
    send_wait(mBox, &a);
    terminate();
}

void task2(void)
{
    int a;
    receive_wait(mBox, &a);
    printf("penis %d", a);
    terminate();
}

void task11(void){
    int nData_t1 = TEST_PATTERN_1;
    wait(10); /* task2 börjar köra */
    if( no_messages(mb) != 1 )
      terminate(); /* ERROR */
    if(send_wait(mb,&nData_t1) == DEADLINE_REACHED)
        terminate(); /* ERROR */
    wait(10); /* task2 börjar köra */
    /* start test 2 */
    nData_t1 = TEST_PATTERN_2;
    if(send_wait(mb,&nData_t1) == DEADLINE_REACHED)
      if( no_messages(mb) != 0 ) 
        terminate(); /* ERROR */
    wait(10); /* task2 börjar köra */
    /* start test 3 */
    if(send_wait(mb,&nData_t1)==DEADLINE_REACHED) {
        nTest3 = 1;
        if (nTest1*nTest2*nTest3) {
        /* Blinka lilla lysdiod */
        /* Test ok! */
          printf("Najs");
    }
        terminate(); /* PASS, no receiver */
    }
    else
    {
        terminate(); /* ERROR */
    }
}
void task22(void){
    int nData_t2 = 0;
    if(receive_wait(mb,&nData_t2) == DEADLINE_REACHED) /* t1 kör nu */
        terminate(); /* ERROR */
    if( no_messages(mb) != 0 )
        terminate(); /* ERROR */
    
    if (nData_t2 == TEST_PATTERN_1) 
      nTest1 = 1;
        wait(20); /* t1 kör nu */
        /* start test 2 */
        
        if( no_messages(mb) != 1 )
            terminate(); /* ERROR */
    if(receive_wait(mb,&nData_t2) == DEADLINE_REACHED) /* t1 kör nu */
        terminate(); /* ERROR */
    if( no_messages(mb) != 0 )  //Ändrad från 1 till 0?
        terminate(); /* ERROR */
    if (nData_t2 == TEST_PATTERN_2) 
      nTest2 = 1;
        /* Start test 3 */
        terminate();
}

void task33(void){
  int nData_t3=1;
  send_wait(mb,&nData_t3);
  //while(1);
  terminate();
}
void task34(void){
  int nData_t4=2;
  receive_wait(mb,&nData_t4);
  terminate();
}