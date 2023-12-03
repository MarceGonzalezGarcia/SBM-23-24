#ifndef __LCD_H
#define __LCD_H

#include "stdio.h"
#include "string.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"

//Funciones que nos permiten configurar el lcd

 void Init_SPI(void);
 void LCD_reset(void);
 void LCD_wr_data(unsigned char data);
 void LCD_wr_cmd(unsigned char cmd);
 void LCD_init(void);
 void LCD_update(void);
 void LCD_Clean_Mitad_Arriba(void);
 void LCD_Clean_Mitad_Abajo(void);
 void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
 void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
 void LCD_Clean(void);
 void symbolToLocalBuffer(uint8_t linea, uint8_t symbol);//Escribe en la linea 1 o en la linea 2
 void delay(uint32_t n_microsegundos);
 void EscribeFrase(char *frase, uint8_t line);
 
 void Thread_lcd (void *argument);                   // thread function
 int Init_Thread_lcd (void);
 
 typedef struct {                                // object data type
  char frase0 [32];															
  char frase1 [32];		
} MSGQUEUE_OBJ_lcd;
  
 
 
 #endif