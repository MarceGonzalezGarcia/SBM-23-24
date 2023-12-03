#include "lcd.h"
#include "Arial12x12.h"
#include "cmsis_os2.h"  


/* Private variables ---------------------------------------------------------*/

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
	
TIM_HandleTypeDef htim7;
GPIO_InitTypeDef GPIO_InitStruct_SPI;	

ARM_SPI_STATUS stat;
static unsigned char buffer[512];

uint16_t positionL1 = 0;
uint16_t positionL2 = 256;
	
//int i ;
	

/* Private functions ---------------------------------------------------------*/

void Init_SPI(void){	
	SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_CPOL1_CPHA1|ARM_SPI_MSB_LSB|ARM_SPI_DATA_BITS(8),20000000);	
	
	//Instanciamos los pines GPIO
	
	//RESET
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStruct_SPI.Pin = GPIO_PIN_6;
		GPIO_InitStruct_SPI.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_SPI.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_SPI);
	//A0
		__HAL_RCC_GPIOF_CLK_ENABLE();
		GPIO_InitStruct_SPI.Pin = GPIO_PIN_13;
		GPIO_InitStruct_SPI.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_SPI.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct_SPI);
	//CS
		__HAL_RCC_GPIOD_CLK_ENABLE();
		GPIO_InitStruct_SPI.Pin = GPIO_PIN_14;
		GPIO_InitStruct_SPI.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_SPI.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_SPI);
		
		//Inicialmente los pines de RESTE, A0, CS :
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);//RESET
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET);//A0
	
}

void LCD_reset(void)
{
//USAMOS TIMER 7 PARA APAGAR PANTALLA Y ESPERAR UN TIEMPO	DE 1MS
	
		delay(1); // Esperamos un milisegundo
		//Apagamos el lcd
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);//RESET
		delay(1000);
		//Encendemos el lcd
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);//RESET	
}
	
void delay(uint32_t n_microsegundos)
{
			__HAL_RCC_TIM7_CLK_ENABLE();
		
		htim7.Instance = TIM7;
		//htim7.Init.Prescaler = 39999 ; //Prescaler=84M/40000 =2100
		//htim7.Init.Period = (2100/(1/n_microsegundos))-1; 
		htim7.Init.Prescaler = 83 ;
		htim7.Init.Period = n_microsegundos-1; 
		
		//Habilitar tratado de interrupciones de la HAL
		HAL_TIM_Base_Init(&htim7);	
		HAL_TIM_Base_Start_IT(&htim7);
		
		
		 // Esperar a que se active el flag del registro de Match
		while(TIM7->CNT);
		
		HAL_TIM_Base_Stop(&htim7);
		HAL_TIM_Base_DeInit(&htim7);
}
 
void LCD_wr_data(unsigned char data)//Escribe un dato en lcd.
{
	 //CS = 0 y A0=1;
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);//CS
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);//A0

	 //Escribir un dato (data) usando la funcion SPIDrv->Send();
		SPIdrv->Send(&data,sizeof(data));//Tama?o en bytes

	 // Esperar a que se libere el bus SPI;
		 do{
				stat = SPIdrv->GetStatus();
		 }while(stat.busy);
		 
	 // Seleccionar CS = 1;
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
}


void LCD_wr_cmd(unsigned char cmd)//Escribe un comando en el lcd
{
    //CS = 0 y A0 = 0;
   	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);//CS
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET);//A0
	
    // Escribir un comando (cmd) usando la funcion SPIDrv->Send(?);
    SPIdrv->Send(&cmd,sizeof(cmd));//Tama?o en bytes
	
    // Esperar a que se libere el bus SPI;
		 do{
			 stat = SPIdrv->GetStatus(); 
		 }while(stat.busy);
		 
	 // Seleccionar CS = 1;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
}

void LCD_init(void){
	//Display off
	LCD_wr_cmd(0xAE);
	//Polarizacion lcd a 1/9
	LCD_wr_cmd(0xA2);
	//Direccionamiento ram
	LCD_wr_cmd(0xA0);
	//Salidas com 
	LCD_wr_cmd(0xC8);
	//Resistencias internas
	LCD_wr_cmd(0x22);
	//Power on
	LCD_wr_cmd(0x2F);
	//Display en la linea 0
	LCD_wr_cmd(0x40);
	//Dislpay ON
	LCD_wr_cmd(0xAF);
	//Contraste
	LCD_wr_cmd(0x81);
	//Valor contraste
	LCD_wr_cmd(0x16);
	//Display all points normal
	LCD_wr_cmd(0xA4);
	//LCD display normal
	LCD_wr_cmd(0xA6);	
}

void LCD_update(void)
{
	int i;
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direccion a 0
	LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direccion a 0
	LCD_wr_cmd(0xB0); // Pagina 0

	for(i=0;i<128;i++){
		LCD_wr_data(buffer[i]);
	}

	
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direccion a 0
	LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direccion a 0
	LCD_wr_cmd(0xB1); // Pagina 1

	for(i=128;i<256;i++){
		LCD_wr_data(buffer[i]);
	}

	
	LCD_wr_cmd(0x00); 
	LCD_wr_cmd(0x10); 
	LCD_wr_cmd(0xB2); //Pagina 2
	
	for(i=256;i<384;i++){
		LCD_wr_data(buffer[i]);
	}

	
	LCD_wr_cmd(0x00); 
	LCD_wr_cmd(0x10); 
	LCD_wr_cmd(0xB3); // Pagina 3

	for(i=384;i<512;i++){
		LCD_wr_data(buffer[i]);
	}
}


void LCD_symbolToLocalBuffer_L1(uint8_t symbol){ // el simbolo que pasemos por parametro es lo que se escriba en dl display
	
	uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');		//Avanza a la linea donde se encuentra la letra que quiero representar
															// todas las letras de arial 12x12 ocupan 25 bytes (1 columna = 1 byte)
  
  for(i=0; i<12; i++){				//Vamos a escribir las 12 primeras columnas de la letra
    
    value1 = Arial12x12 [offset + i*2 +1];		// valor1= arial[1,3,5,7,9,....]
    value2 = Arial12x12 [offset + i*2 +2];		// valor2= arial[2,4,6,8,10,....]
		
  // if((i+positionL1)<120){						// se limita a que escriba solo en las dos primera pagina
    buffer [i + positionL1] = value1; 				//pagina 0 escribimos los valores impares de arial
    buffer [i + 128 + positionL1] = value2;		//pagina 1 escribimos los valores pares de arial
	//}
	}
		
  positionL1= positionL1 + Arial12x12[offset];// positionL1 variable que permite escribir varias palabras sin sobreescribir, indica la columna donde se debe comenza a escribir
		
}

void LCD_symbolToLocalBuffer_L2(uint8_t symbol){ // el simbolo que pasemos por parametro es lo que se escriba en dl display

	
	uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');		//Avanza a la linea donde se encuentra la letra que quiero representar
															// todas las letras de arial 12x12 ocupan 25 bytes (1 columna = 1 byte)
  
  for(i=0; i<12; i++){				//Vamos a escribir las 12 primeras columnas de la letra
    
    value1 = Arial12x12 [offset + i*2 +1];		// valor1= arial[1,3,5,7,9,....]
    value2 = Arial12x12 [offset + i*2 +2];		// valor2= arial[2,4,6,8,10,....]
		
  // if((i+positionL1)<120){						// se limita a que escriba solo en las dos primera pagina
    buffer [i + positionL2] = value1; 				//pagina 0 escribimos los valores impares de arial
    buffer [i + 128 + positionL2] = value2;		//pagina 1 escribimos los valores pares de arial
	//}
	}
		
  positionL2= positionL2 + Arial12x12[offset];// positionL1 variable que permite escribir varias palabras sin sobreescribir, indica la columna donde se debe comenza a escribir
		
}

void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
	
  if (line == 0){
    LCD_symbolToLocalBuffer_L1(symbol);
  }else if (line == 1)
    LCD_symbolToLocalBuffer_L2(symbol);   
}


void EscribeFrase(char *frase, uint8_t line)
{
	uint8_t i;

	if(line == 0){
		//LCD_Clean_Mitad_Arriba();									//Borra las paginas 0 y 1 antes de empezar a escribir lo nuevo
		for(i=0; i<strlen(frase); i++){
			if(positionL1+12 < 127)
				LCD_symbolToLocalBuffer_L1(frase[i]);
		}
	}
	if(line == 1){
		//LCD_Clean_Mitad_Abajo();									//Borra las paginas 2 y 3 antes de empezar a escribir lo nuevo
		for(i=0; i<strlen(frase); i++){
			if(positionL2+12 < 383)
				LCD_symbolToLocalBuffer_L2(frase[i]);
		}
	}
	positionL2 = 256;
	positionL1 = 0;
	LCD_update();
}

//Limpiar LCD
void LCD_Clean(void)
{
	memset(buffer,0,512);
	LCD_update();
}


void LCD_Clean_Mitad_Arriba(void){
	
	memset(buffer,0,256);
	LCD_update();
	
}

void LCD_Clean_Mitad_Abajo(void){
	
	memset(buffer,256,512);
	LCD_update();
	
}





 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_lcd;                        // thread id

#define MSGQUEUE_LCD_OBJECTS 16                     // number of Message Queue Objects
 

osMessageQueueId_t mid_MsgQueue_lcd;                // message queue id
 
void Thread_lcd (void *argument);                   // thread function
 
int Init_Thread_lcd(void) {
 
	mid_MsgQueue_lcd = osMessageQueueNew(MSGQUEUE_LCD_OBJECTS, sizeof(MSGQUEUE_OBJ_lcd), NULL);	
	
  tid_Thread_lcd = osThreadNew(Thread_lcd, NULL, NULL);

  return(0);
}
 


void Thread_lcd (void *argument) {						//Recibo los elementos de la cola
	
	  MSGQUEUE_OBJ_lcd msg;
		osStatus_t status;
 
  while (1) {
		
    status = osMessageQueueGet(mid_MsgQueue_lcd, &msg, NULL, osWaitForever);   
		
    if (status == osOK) {
			
			EscribeFrase(msg.frase0, 0);
			EscribeFrase(msg.frase1, 1);
			
		}
    osThreadYield();                            // suspend thread
  }
}
