/* NAME : ASHWIN RAMAN and RAMNARAYAN KRISHNAMURTHY]
 * DESCRIPTION: Firmware source code to initiate and use the USART.
 * DATE : 5/2/2015
 */

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>
#include <misc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stm32f4xx_adc.h>

int ConvertedValue = 0; //Converted value readed from ADC

void RCC_Configuration(void)
{
  /* --------------------------- System Clocks Configuration -----------------*/
  /* USART3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
}

/**************************************************************************************/

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*-------------------------- GPIO Configuration ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect USART pins to AF */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
}

/**************************************************************************************/

void USART3_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

  /* USARTx configuration ------------------------------------------------------*/
      USART_InitStructure.USART_BaudRate = 9600; //baudrate setting
   	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
   	  USART_InitStructure.USART_Parity = USART_Parity_No;
   	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   	  USART_Init(USART3, &USART_InitStructure);

   	  USART_Cmd(USART3,ENABLE);
}

/**************************************************************************************/

void putch(unsigned char c)
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
			    USART_SendData(USART3, c);
}

void putstr(char *ss)
{
	while(*ss)
	{
		putch(*ss);
		ss++;
	}
}
