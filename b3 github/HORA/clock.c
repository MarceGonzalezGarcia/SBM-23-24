#include "cmsis_os2.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Timer HORA
 *---------------------------------------------------------------------------*/

osTimerId_t tim_hora;                            // timer id
static uint32_t exec_hora;                       // argument for the timer call back function

int horas, minutos, segundos;
 
// Periodic Timer Function
static void Timer2_Callback (void const *arg) {
	
  segundos++;
  
  if(segundos>59){
    segundos = 0;
    minutos++;
		
    if(minutos>59){
      minutos=0;
      horas++;
			
      if(horas>23){
        horas=0;
      } 
    }   
  }		
}
 
// Create and Start timers
int Init_Timer_hora (void) {
  osStatus_t status;                            // function return status
  exec_hora = 2U;
  tim_hora = osTimerNew((osTimerFunc_t)&Timer2_Callback, osTimerPeriodic, &exec_hora, NULL);
  if (tim_hora != NULL) {  // Periodic timer created
    status = osTimerStart(tim_hora, 1000U); 		//1 segundo de resolución del reloj           
    if (status != osOK) {
      return -1;
    }
  }
  return NULL;
}


/*----------------------------------------------------------------------------
 *      Thread HORA
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_hora;                        // thread id
void Thread_hora (void *argument);                   // thread function
 
int Init_Thread_hora (void) {
  tid_Thread_hora = osThreadNew(Thread_hora, NULL, NULL);
  if (tid_Thread_hora == NULL) {
    return(-1);
  }
  return(0);
}
 
void Thread_hora (void *argument) {
	
	Init_Timer_hora();
 
  while (1) {
		
    osThreadYield();                            // suspend thread
  }
}