#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "lcd.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_principal;                        // thread id



 
void Thread_principal (void *argument);                   // thread function
 
int Init_Thread_principal (void) {
 
  tid_Thread_principal = osThreadNew(Thread_principal, NULL, NULL);
  if (tid_Thread_principal == NULL) {
    return(-1);
  }
 
  return(0);
}
 
extern osMessageQueueId_t mid_MsgQueue_lcd; 

void Thread_principal (void *argument) {
 
	MSGQUEUE_OBJ_lcd msg_lcd;
	
  while (1) {

		
		//SPRINTF: Meter un texto dentro de un array
		sprintf(msg_lcd.frase0, "     SBM 2023     ");
		sprintf(msg_lcd.frase1, "      00:00:00     ");
		
		osMessageQueuePut(mid_MsgQueue_lcd, &msg_lcd, NULL, 0U);
		
    osThreadYield();                            // suspend thread
  }
}
