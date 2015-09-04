/* NAME : ASHWIN RAMAN and RAMNARAYAN KRISHNAMURTHY]
 * DESCRIPTION: Firmware source code to initiate and use the ADC on STM32F4.
 * DATE : 5/2/2015
 */

#include "usart.h"

void RCC_Configuration_adc(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

/**************************************************************************************/

void GPIO_Configuration_adc(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* ADC Channel 11 -> PC1 */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**************************************************************************************/

void ADC_Configuration(void)
{
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;      // Used for number of samples. Assign clock frequency
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_8Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //12 bit ADC channel
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 1 Channel
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // Conversions Triggered  ..
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // Manual
  ADC_InitStructure.ADC_ExternalTrigConv = DISABLE;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//..
  ADC_Init(ADC1, &ADC_InitStructure);



  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
}

int adc_convert()
{
	/* ADC1 regular channel 11 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_15Cycles); // PC1

	ADC_SoftwareStartConv(ADC1);					//Start the conversion
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));  //checking status flag if available for conversion
	return ADC_GetConversionValue(ADC1); 			//Return the converted data
}

