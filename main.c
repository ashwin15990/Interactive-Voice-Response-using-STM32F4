/* INTERACTIVE VOICE RESPONCE */

/* NAME : ASHWIN RAMAN and RAMNARAYAN KRISHNAMURTHY]
 * DESCRIPTION: MAIN.C File that calls all the firmware source codes.
 * DATE : 5/2/2015
 */

#include "arm_math.h"  //imported arm_math.h file. The file and corresponding DSP libraries were imported from STM32cubeF4
#include "EXT0_interrupt.h"

#define SAMPLES		512
#define FFT_SIZE	SAMPLES/2

float Input[SAMPLES];
float Output[FFT_SIZE];

void LED_GPIO_initialize()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef led_gpio;

	//						Green		Orange		Red			Blue
	led_gpio.GPIO_Pin= 		GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	led_gpio.GPIO_Mode=		0x01;//01=Output
	led_gpio.GPIO_Speed=	0x03;//03=100Mhz
	led_gpio.GPIO_OType=	0x00;//00=Push Pull
	led_gpio.GPIO_PuPd=		0x00;//00=No Pull

	GPIO_Init(GPIOD,&led_gpio);
	GPIO_PinLockConfig(GPIOD,GPIO_Pin_12);
	GPIO_PinLockConfig(GPIOD,GPIO_Pin_13);
	GPIO_PinLockConfig(GPIOD,GPIO_Pin_14);
	GPIO_PinLockConfig(GPIOD,GPIO_Pin_15);
}

void motor_GPIO_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef motor_gpio; //configuring PC6 for motor control

	motor_gpio.GPIO_Pin= GPIO_Pin_6;
	motor_gpio.GPIO_Mode=0x01;//01=Output
	motor_gpio.GPIO_Speed=0x03;//03=100Mhz
	motor_gpio.GPIO_OType=0x00;//00=Push Pull
	motor_gpio.GPIO_PuPd=GPIO_PuPd_DOWN;//00=No Pull

	GPIO_Init(GPIOC,&motor_gpio);
	//GPIO_PinLockConfig(GPIOC,GPIO_Pin_6);
}

void Delay(unsigned int c)
{
	unsigned int i;
	for(i=0;i<c;i++)
		asm("nop");
}

void int_to_string_convert(unsigned char num[], unsigned int val)
{
	unsigned char quotient ,i=0;
	unsigned int div = 1000;
	while(i<4)
	{
		quotient = val/div;
		num[i] = quotient + 48;

		val = val % div;
		div = div/10;
		i++;
	}
	num[i]=0;
}

void print_buf(unsigned char buf[])
{
	unsigned char i = 0;
	while(buf[i]!=0)
	{
		putch(buf[i]);
		i++;
	}
}

unsigned char rem_conditional(unsigned char rem)
{
	if(rem<36)
		return 1;
	else if(rem>=36 && rem<72)
		return 3;
	else if(rem>=72 && rem<108)
		return 7;
	else if(rem>=108 && rem<144)
		return 0x0f;
	else if(rem>=144 && rem<180)
		return 0x1f;
	else if(rem>= 180 && rem<216)
		return 0x3f;
	else if(rem>=216 && rem<255)
		return 0x7f;
}

void Draw_equalizer(unsigned int value, unsigned char index)
{
	unsigned char k=0,rem=0, A=0;
	value = value *60; //Random amplification factor of each bin to print on LCD. *Subject to change *

	// we are trying to write the value to the LCD in the form of value = k*256 + A
    // A depends on the conditional statements as used below.

	k= value/256;          //k will vary from 0-8
	rem = value - (256*k);

	if(rem!=0)
		A = rem_conditional(rem);

	GLCD_drawline(127-index, A,	k);
	//index passed is 127-k because our x=0 is the 128th pixel
}

int main(void)
{
	unsigned int i,j,disp,rotations,x=0; //disp will contain index corresponding to the word to be displayed
	arm_cfft_radix4_instance_f32 S;

	float maxValue;				// Max FFT value is stored here
	uint32_t maxIndex;			//	 Index in Output array where max value is

	unsigned char maxV[10];
	unsigned char maxI[10];
	unsigned char fft_val[10];
	unsigned char sum1[5];
	unsigned char envelope[10]={};

	interrupt_initialize();	//interrupt initializing
	motor_GPIO_init();

	//USART Configuration setup
    RCC_Configuration();
    GPIO_Configuration();
    USART3_Configuration();

    //ADC Configuration Setup
    RCC_Configuration_adc(); //APBH2 Clock setting. Needed to configure the ADC sapling rates
    GPIO_Configuration_adc(); //GPIO PC.0 is configured as ADC input
    ADC_Configuration();     //ADC configuration. Sampling rate, 8/12bit adc etc. etc.

    /* GLCD INITIALIZATION */
	LCD_GPIO_init();
	//lcd_varinit();
	DelayMs(1);
	RST1;
	DelayMs(1);
	RST0;
	DelayMs(1);
	RST1;
	DelayMs(1);
	GLCD_Init();              //Initialize GLCD
	DelayMs(5);
	GLCD_clear();
	/* End of Initialization */


	LED_GPIO_initialize(); //Initialize GPIOD for LED Port pins
    //////// FFT CODE SECTION ************* DONOT TOUCH/CHANGE ********************** /////

    unsigned char status = 0,k=0;
    unsigned int add = 0, d;

    chip_select(1);
    GLCD_Comd(0xb8 | 4); 	//selecting page
    GLCD_Comd(0x40 | 0);	//select column

    for(i=0;i<49;i++)
    		GLCD_Data(font_mode1[i]);



    putch('\n'); putch('\r');
    print_buf("\n\r MODE : ");
    putch(mode+0x30);
    print_buf("\n\n\r");

    for(i = 0; i<100;	i++)
    	for(d = 0; d<65536;	d++)
    		asm("nop");

    GLCD_clear();
    while(1)
    {
    	//GPIO_Write(GPIOD,0x1000);
    	for (i = 0; i < SAMPLES; i += 2)
    	{
    				Delay(21); //120us Delay

    				// Real part, must be between -1 and 1
    				Input[(uint16_t)i] = (float)((float)adc_convert() - (float)2048.0) / (float)2048.0;
    				// Imaginary part
    				Input[(uint16_t)(i + 1)] = 0;
    	}

		arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);

		// Process the data through the CFFT/CIFFT module
		arm_cfft_radix4_f32(&S, Input);

		// Process the data through the Complex Magnitude Module for calculating the magnitude at each bin
		arm_cmplx_mag_f32(Input, Output, FFT_SIZE);

		// Calculates maxValue and returns corresponding value
		arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);

		int_to_string_convert(maxV, maxValue);
		int_to_string_convert(maxI, maxIndex);

		putch('A');putch(':');putch(' ');print_buf(maxV);putch(' ');putch('B');putch(':');putch(' ');print_buf(maxI);
		putch('\n');
		putch('\r');

		/* if in speech recognition, --- >code section for selective Speech Recognition  */
		if(mode == 2)
		{
			//print_buf("\n\rMODE 2");
			if(maxIndex>=1)
				status=1;

			if (status==1)
			{
				if(maxIndex>=128) maxIndex = 255 - maxIndex;
				add = add + maxIndex;
				k++;
			}

			if(k==10)
			{
				status = 0;
				k = 0;
				disp=0;

				int_to_string_convert(sum1,add);
				print_buf('\n\r');
				print_buf(sum1);

				// ADD YOUR CODE **HERE** FOR SPEECH RECOGNITON ALGORITHM INSTEAD OF COMPARISIONS IF NEEDED
				// the compare values below are the signature average maximum frequency (bin) values
				if(add<=3) continue;

				if (add >=0 && add<= 13)
					{
						print_buf("\n\rGREEN\n\r");
						GPIO_Write(GPIOD,0x1000);
						disp = 2;
					}

				else if (add >25 && add <= 43)
					{
						print_buf("\n\rRED\n\r");
						GPIO_Write(GPIOD,0x4000);
						disp = 4;
					}

				else if (add > 43 && add <= 90)
					{
						print_buf("\n\rORANGE\n\r");
						GPIO_Write(GPIOD,0x2000);
						disp = 5;
					}

				else if (add > 14 && add<= 25)
					{
						print_buf("\n\rBLUE\n\r");
						GPIO_Write(GPIOD,0x8000);
						disp = 8;
					}
				else if (add> 90 && add<=210)
					{
						print_buf("\n\rCIRCULATE\n\r");

						for (rotations = 0; rotations <5; rotations ++)
						{
						GPIO_Write(GPIOD,0x1000);  			//green
						for(i = 0; i<3;	i++)
								for(d = 0; d<65536;	d++)
										asm("nop");

						GPIO_Write(GPIOD,0x2000);  			//orange
						for(i = 0; i<3;	i++)
								for(d = 0; d<65536;	d++)
										asm("nop");


						GPIO_Write(GPIOD,0x4000);  			//red
						for(i = 0; i<3;	i++)
								for(d = 0; d<65536;	d++)
										asm("nop");

						GPIO_Write(GPIOD,0x8000);  			//Blue
						for(i = 0; i<3;	i++)
							      for(d = 0; d<65536;d++)
							      		asm("nop");
						}

						// *****  Code to circulate ************
						disp = 9;
					}
				add = 0;
			}
			//if in speech recog mode
			GLCD_character_write(disp);
			/* end of if statement for speech recognition code */
    	}

		if(mode == 3) //MOTOR CONTROL MODE
		{
			//print_buf("\n\rMODE 2");
			//disp =0;
			if(maxIndex>=1)
				status=1;

			if (status==1)
			{
				if(maxIndex>=128) maxIndex = 255 - maxIndex;
				add = add + maxIndex;
				k++;
			}

			if(k==10)
			{
				status = 0;
				k = 0;
				disp = 0;
				int_to_string_convert(sum1,add);
				print_buf('\n\r');
				print_buf(sum1);

				// ADD YOUR CODE **HERE** FOR SPEECH RECOGNITON ALGORITHM INSTEAD OF COMPARISIONS IF NEEDED
				if(add<=3) continue;

				else if (add > 90 && add <=150)
					{
						print_buf("\n\rSTOP\n\r");
						GPIO_Write(GPIOD,0x0000);
						GPIO_ResetBits(GPIOC,GPIO_Pin_6);
						disp = 6;
					}
				else if (add >= 180 && add <= 300)
					{
						print_buf("\n\rSTART\n\r");
						GPIO_Write(GPIOD,0x0000);
						GPIO_SetBits(GPIOC,GPIO_Pin_6);
						disp = 7;
					}
				add = 0;
			}
			//if in speech recog mode
			GLCD_character_write(disp);
			/* end of if statement for speech recognition code */
    	}

		if(mode == 4) //CAMERA AUTHENTICATION MODE
		{
			if(maxIndex>=1)
				status=1;

			if (status==1)
			{
				if(maxIndex>=128) maxIndex = 255 - maxIndex;
				add = add + maxIndex;
				k++;
			}

			if(k==10)
			{
				status = 0;
				k = 0;

				int_to_string_convert(sum1,add);
				print_buf('\n\r');
				print_buf(sum1);

				if(add<=3) continue;

				else if (add >= 60 && add <=175)
					{
						print_buf("\n\rREGISTER\n\r");
						GPIO_Write(GPIOD,0x0000);
						// **** CODE TO TOGGLE A GPIO PIN *****
					}
				else if (add >= 15 && add <= 35)
					{
						print_buf("\n\rLOGIN\n\r");
						GPIO_Write(GPIOD,0x0000);
						// **** CODE TO TOGGLE A GPIO PIN *****
					}
				add = 0;
			}
			//if in speech recog mode

			//***** IF REQUIRED ******
			//GLCD_character_write(disp);
			// end of if statement for speech recognition code
    	}

		/* code section for printing the frequency profile of waveform */
		//if(x%5==0)
		//{
			for (i = 0; i<FFT_SIZE/2;i++)       		//Capturing bins 0-127
			{
				Draw_equalizer(Output[i],i);
			}
			//if(maxValue>=3)
			if(mode!=1)
				GLCD_clear();
		//}
		//x++;
    }
}
