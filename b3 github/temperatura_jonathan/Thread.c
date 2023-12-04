#include "cmsis_os2.h" 
#include "Driver_I2C.h" 
#define I2C_ADDR 0x48

void i2c_signalevent(uint32_t dev_num );
void Timer1_Callback_1(void *arg);
float temp(void);
unsigned short read16(const uint8_t );

/* I2C driver instance */
extern ARM_DRIVER_I2C            Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;
 int cnt=0;
 
 
void init_i2c (void){
 I2Cdrv->Initialize (i2c_signalevent);
 I2Cdrv->PowerControl(ARM_POWER_FULL);
 I2Cdrv->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);
}

void i2c_signalevent (uint32_t dev_num ){
 if (dev_num & ARM_I2C_EVENT_TRANSFER_DONE){
 cnt++;
 }


}

float temp(void){

 short value;

    //Read the 11-bit raw temperature value
    value = read16(0x00) >> 5;

    //Sign extend negative numbers
    if (value & (1 << 10))
        value |= 0xFC00;

    //Return the temperature in °C
    return value * 0.125;
}


unsigned short read16(const uint8_t  reg){
    //Create a temporary buffer
    uint8_t  buff[2];

    //Select the register
    I2Cdrv->MasterTransmit (I2C_ADDR, &reg, 1, true);
 
  /* Wait until transfer completed */
  while (I2Cdrv->GetStatus().busy);
  /* Check if all data transferred */
 // if (I2Cdrv->GetDataCount () != len) return -1;
 
  I2Cdrv->MasterReceive (I2C_ADDR, buff, 2, false);
 
  /* Wait until transfer completed */
  while (I2Cdrv->GetStatus().busy);
  /* Check if all data transferred */
 // if (I2Cdrv->GetDataCount () != len) return -1;
  return (buff[0] << 8) | buff[1];
    
}
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
osMessageQueueId_t temp_Msg;                    // cola   id 
osThreadId_t tid_Thread;                        // thread id
osTimerId_t timsoft1;                           // timer  id
 
void Thread (void *argument);                   // thread function
 
int Init_Thread (void) {
  temp_Msg = osMessageQueueNew(3, sizeof(uint8_t), NULL);
	  if (temp_Msg == NULL) {
    return(-1);
  }
  tid_Thread = osThreadNew(Thread, NULL, NULL);
	  if (tid_Thread == NULL) {
    return(-1);
  }
	timsoft1 = osTimerNew(Timer1_Callback_1, osTimerPeriodic, NULL, NULL);
   if (timsoft1 == NULL) {
    return(-1);
  }

  return(0);
}
 
void Thread (void *argument) {
 init_i2c();
  osTimerStart(timsoft1, 1000U);
	
  while (1) {
 
  }
}


float temperatura_seg;

void Timer1_Callback_1(void *arg){

		 temperatura_seg= temp();
    osMessageQueuePut(temp_Msg, &temperatura_seg, 0U, 0U); //introduce en la cola la temperatura
}	