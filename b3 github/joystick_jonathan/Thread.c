#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 

 #define MSGQUEUE_OBJECTS 16 
 void Timer1_Callback_1(void *arg);
 void Timer2_Callback_2(void *arg);

 typedef struct {                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;

MSGQUEUE_OBJ_t msg_env;
osMessageQueueId_t MsgJoy;
osThreadId_t timer1; //PULSACION CORTA
osTimerId_t timer2; //PULSACION LARGA  
osThreadId_t Thjoystick; 
int cuenta=0;
int Init_Thread (void);  
void Thread_Joysitck (void *argument);                   // thread function
static GPIO_InitTypeDef GPIO_struct;



int Init_Thread (void) {
 
 MsgJoy  = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
 timer1 = osTimerNew(Timer1_Callback_1, osTimerOnce, NULL, NULL); 
 timer2 = osTimerNew(Timer2_Callback_2, osTimerPeriodic,NULL, NULL);
 Thjoystick = osThreadNew(Thread_Joysitck, NULL, NULL);
	
	if (MsgJoy == NULL) {
    return(-1);
  }

  return(0);
}
 
void Thread_Joysitck (void *argument) {

HAL_GPIO_Init(GPIOB, &GPIO_struct);	
  while (1) {
     osThreadFlagsWait(1, osFlagsWaitAny , osWaitForever); //recibo el flag enviado en la callback de la pulsación
    osTimerStart(timer1, 50U);

  }
}




void Timer1_Callback_1(void *arg){

		 if( HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_10) == GPIO_PIN_SET ){//Pulsado arriba
    msg_env.Buf[0] = 1;
    osMessageQueuePut(MsgJoy, &msg_env, 0U, 0U);
  }
  else if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_11) == GPIO_PIN_SET ){//Pulsado el derecho
    msg_env.Buf[0] = 2;
    osMessageQueuePut(MsgJoy, &msg_env, 0U, 0U);
  }
  else if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_12) == GPIO_PIN_SET ){//Pulsado abajo
    msg_env.Buf[0] = 4;
    osMessageQueuePut(MsgJoy, &msg_env, 0U, 0U);
  }
  else if( HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_14) == GPIO_PIN_SET){//Pulsado izquierda
    msg_env.Buf[0] = 8;
    osMessageQueuePut(MsgJoy, &msg_env, 0U, 0U);
  }
  else if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){//Pulsado centro

    osTimerStart(timer2, 50U); 
  }
			
}	

void Timer2_Callback_2(void *arg){

		  if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){ 
    cuenta++;
  }else{
   
    if(cuenta>19){  
    
    msg_env.Buf[0] = 32; 
    osTimerStop(timer2);
    cuenta=0;
    osMessageQueuePut(MsgJoy, &msg_env, 0U, 0U);
   
  }else{ //pulsacion corta
    
    msg_env.Buf[0] = 16; 
    osTimerStop(timer2);
    cuenta=0;
    osMessageQueuePut(MsgJoy, &msg_env, 0U, 0U);
    
   }

  }
			
}	
	
