/* NAME : ASHWIN RAMAN and RAMNARAYAN KRISHNAMURTHY]
 * DESCRIPTION: Firmware source code to initiate and use the Graphic LCD.
 * DATE : 5/2/2015
 */

#include "adc.h"
#include "font.h"

// Source : Looked up many driver functions for Graphic LCD interfacing with 8051 and used the ideas for for ARM
void GLCD_Init();
void GLCD_Data(unsigned char);
void GLCD_Comd(unsigned char);
void DelayMs(int);
unsigned char mode = 1;

unsigned char c = 0;

#define CS2_ON		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
#define CS2_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_0);

#define CS1_ON		GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define CS1_OFF		GPIO_SetBits(GPIOA,GPIO_Pin_1);

#define RST0	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
#define RST1	GPIO_SetBits(GPIOA,GPIO_Pin_2);

#define RW0		GPIO_ResetBits(GPIOA,GPIO_Pin_3);
#define RW1		GPIO_SetBits(GPIOA,GPIO_Pin_3);

#define RS0		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
#define RS1		GPIO_SetBits(GPIOA,GPIO_Pin_4);

#define EN0		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
#define EN1		GPIO_SetBits(GPIOA,GPIO_Pin_5);

void LCD_GPIO_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef lcd_gpio;


	lcd_gpio.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	lcd_gpio.GPIO_Mode=0x01;//01=Output
	lcd_gpio.GPIO_Speed=0x03;//03=100Mhz
	lcd_gpio.GPIO_OType=0x00;//00=Push Pull
	lcd_gpio.GPIO_PuPd=0x00;//00=No Pull

	GPIO_Init(GPIOE,&lcd_gpio);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_0);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_1);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_2);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_3);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_4);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_5);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_6);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_7);

	lcd_gpio.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init(GPIOA,&lcd_gpio);
//	GPIO_PinLockConfig(GPIOB,GPIO_Pin_12);
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_0);
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_1);
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_2);
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_3);
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_4);
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_5);

}

int i;


void LCD_Delay(unsigned int x)
{
	unsigned int i;
	for(i=0;i<x;i++)
		asm("nop"); 			//1 NOP corresponds to
}

void LCD_EN()
{
	EN0;
	LCD_Delay(100);
	EN1;
	LCD_Delay(100);
}

void GLCD_Comd(unsigned char cmnd)
{
    GPIO_Write(GPIOE,cmnd);
    RS0;               //make it RS to Low
    RW0;               //make it RW to low
    EN1;            //enbale high
    DelayMs(10);
    EN0;            //enable low
}


void GLCD_Data(unsigned char dat)
{
	GPIO_Write(GPIOE,dat);
    RS1;               //make it RS to high
    RW0;               //make it RW to low
    EN1;            //enbale high
    DelayMs(10);
    EN0;            //enbale low
}

void DelayMs(int k)
{
         unsigned int a;
         	 for(a=0;a<=k;a++);
}

void GLCD_Init()
{
	//unsigned char i;
    unsigned char Comd[5]={0xc0,0xb8,0x40,0x3f};//LCD Command list
    chip_select(1);           //send commands to page1
    for(i=0;i<4;i++)
    	GLCD_Comd(Comd[i]);
    chip_select(0);           //send commands to page0
    for(i=0;i<4;i++)
    	GLCD_Comd(Comd[i]);
}

void chip_select(unsigned char Page)
{
    if(Page)
	{
		 CS1_OFF;       //Page 0 LCD IC1
		 CS2_ON;
	}
    else
	 {
		 CS1_ON;   		//Page 1 LCD IC2
		 CS2_OFF;
	 }
}

void GLCD_clear() //Clearing GLCD screen
{
	int Page=0,i=0;
	int Column=0;

	for (Page = 0; Page < 8; Page++)
	    {
	         chip_select(0);                        //Display part of image to Page1
	         GLCD_Comd(0xb8 | Page);
	         GLCD_Comd(0x40);

	         for (Column = 0; Column < 128; Column++)
	         {
	             if (Column == 64)
	            {
	                 chip_select(1);                    //Display part of image to Page0
	                 GLCD_Comd(0xb8 | Page);
	                 GLCD_Comd(0x40);
	            }

	            GLCD_Data(0x00);
	         }
	    }
}

void GLCD_drawline(unsigned int column, unsigned char A, unsigned char k) //Change here for method 1,2 and 3
{
     unsigned char page=0;                       //number of complete bars to be written
     	 	 	 	 	 	 	 	 	 	// OR number of pages to be filled completely in a column (with FF)
     if(column==63)
    	 chip_select(0);                        //Display part of image to Page1
     else if(column==127)
    	 chip_select(1);

     if(k > 0)
     {
		 for(page=0;page<k;page++)
		 // Compute value in form of x = k*256 + A;
		 {
			 GLCD_Comd(0xb8 | page);
			 GLCD_Comd(0x40 | column);
			 GLCD_Data(0xFF); 		//Printing Line in selected page and column.
		 }
     }

     //putch(page+0x30); putch(A+0x30); putch(' ');
	 GLCD_Comd(0xb8 | page);
	 GLCD_Comd(0x40 | column);
	 GLCD_Data(A); 				//Putting Data on GLCD

	 page = page +1;
	 if(mode == 1)
	 {
	 while(page<8)
	 {
		 GLCD_Comd(0xb8 | page);
		 GLCD_Comd(0x40 | column);
		 GLCD_Data(0x00);
		 page++;
	 }
	 }
}


void GLCD_character_write(unsigned char disp)
{

	unsigned char i=0;
	chip_select(0);
	GLCD_Comd(0xb8 | 7); 	//selecting page
	GLCD_Comd(0x40 | 0);	//select column

	switch(disp)
	{
		case 1:
				for(i=0;i<38;i++)
					GLCD_Data(font_yes[i]);
				break;
		case 2:
				for(i=0;i<38;i++)
					GLCD_Data(font_green[i]);
				break;
		case 3:
					for(i=0;i<38;i++)
						GLCD_Data(font_no[i]);
				break;
		case 4:
					for(i=0;i<38;i++)
						GLCD_Data(font_red[i]);
				break;
		case 5:
					for(i=0;i<38;i++)
						GLCD_Data(font_orange[i]);
				break;
		case 6:
					for(i=0;i<38;i++)
						GLCD_Data(font_stop[i]);
				break;
		case 7:
					for(i=0;i<38;i++)
						GLCD_Data(font_start[i]);
				break;
		case 8:
					for(i=0;i<38;i++)
						GLCD_Data(font_blue[i]);
				break;
		case 9:
					GLCD_clear();
				break;
	}

}


