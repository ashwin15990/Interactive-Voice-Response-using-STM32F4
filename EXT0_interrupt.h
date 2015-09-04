/* NAME : ASHWIN RAMAN and RAMNARAYAN KRISHNAMURTHY]
 * DESCRIPTION: Firmware source code to initiate and use the EXTERNAL INTERRUPT.
 * DATE : 5/2/2015
 */
#include "lcd.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"

/* Refernce : my.st.com forum for external intterup handler*/
/**************************************************************************************/


void EXTI0_IRQHandler(void)
{
	unsigned int d,i;
  //if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  //{
	  GLCD_clear();
	  mode = mode+1;
	  if(mode==5) mode=1;

	  putch('\n'); putch('\r');
	  print_buf("\n\r MODE : ");
	  putch(mode+0x30);
	  print_buf("\n\n\r");

	  // Printing Mode on GLCD
	  chip_select(1);
	  GLCD_Comd(0xb8 | 4); 	//selecting page
	  GLCD_Comd(0x40 | 0);	//select column
	  for(i=0;i<49;i++)
	  {
		  if(mode==1)
	      		GLCD_Data(font_mode1[i]);
		  else if(mode==2)
		  	    GLCD_Data(font_mode2[i]);
		  else if(mode==3)
		  		 GLCD_Data(font_mode3[i]);
		  else if(mode==4)
			  	  GLCD_Data(font_mode4[i]);

	  }

	  for(i = 0; i<100;	i++)
	      for(d = 0; d<65536;	d++)
	      		asm("nop");

	  EXTI_ClearITPendingBit(EXTI_Line0);
  //}
}

/**************************************************************************************/

int interrupt_initialize(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIOB clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PB0 pin as input. PB0 is pulled up to detect a falling edge interrupt trigger*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);

  /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
