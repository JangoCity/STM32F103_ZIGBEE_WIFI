#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "myiic.h"
#include "24cxx.h"
#include "rtc.h"
#include "spi.h"
#include "flash.h"
#include "touch.h"
#include "mmc_sd.h"
#include "tm1638.h"
#include "ff.h"
#include "integer.h"
#include "diskio.h"
#include "text.h"
#include "fontupd.h"
#include "stdio.h"
#include "string.h"
//#include "picdecoder.h"

#include "rtc.h" 
#include "bmp180.h"


const u8 *COMPILED_DATE=__DATE__;//��ñ�������
const u8 *COMPILED_TIME=__TIME__;//��ñ���ʱ��
const u8* Week[7]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};


extern const unsigned char gImage_1[38400];
extern const unsigned char gImage_2[38400];
extern const unsigned char gImage_3[38400];
extern const unsigned char gImage_4[38400];


void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(216,0,"RST");//��ʾ��������
  POINT_COLOR=RED;//���û�����ɫ 
}


int main(void)
{
	u32 fontcnt;		  
	u8 i,j;
	FATFS fs;
	u8 fontx[2];//gbk��
	u32 sector_size;
	u8 key;
	u8 t=0;	
	////////
	u8 t_kdat=0;
	u8 temp_keydat;
	u8 keyDat=0;
	///////
	const char filedir[]="0:/PICTURE";
	
	SystemInit();
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();
	
	uart_init1(115200);
	uart_init2(9600);
	USART2_Send("UART2_OK",8);
	USART1_Send("UART1_OK",8);
	
 	LED_Init();
	LCD_Init();
	RTC_Init();
	init_TM1638();
	AT24CXX_Init();		//IIC��ʼ��
	SPI_Flash_Init();   //SPI FLASH ��ʼ�� 
	//f_mount(0, &fs); //��ʼ������mount
	POINT_COLOR=RED;
  
	//LCD_ShowString(70,50,"SY-HMI-3.0TP");	 
  
	//SPI_Flash_Write((u8 *)gImage_1, 0, 38400);	
	//SPI_Flash_Write((u8 *)gImage_2, 38400, 38400);		
	//SPI_Flash_Write((u8 *)gImage_3, 38400 * 2, 38400);	
	//SPI_Flash_Write((u8 *)gImage_4, 38400 * 3, 38400);		
	
	LCD_ShowPic();
	
	POINT_COLOR=BLACK;
	//POINT_COLOR=GREEN;
	LCD_DrawRectangle(1,1,239,319);
	LCD_DrawRectangle(10,10,230,310);
	LCD_Fill(10,120,120,175,GREEN);  LCD_Fill(120,120,230,175,GREEN);
	LCD_ShowString(40,140,"mode1");	LCD_ShowString(150,140,"mode2");	
	LCD_Fill(10,175,120,230,GREEN);  LCD_Fill(120,175,230,230,GREEN);
	LCD_ShowString(40,195,"mode3");	LCD_ShowString(150,195,"mode4");	
	
	LCD_DrawRectangle(10,120,120,175); LCD_DrawRectangle(120,120,230,175);
	LCD_DrawRectangle(10,175,120,230); LCD_DrawRectangle(120,175,230,230);
	
	//LCD_ShowString(70,15,"utZIGBEE BASE");	
//	LCD_ShowString(30,60,"TEMP:12C");	LCD_ShowString(140,60,"BMP:2100P");	
	
  LCD_ShowString(50,245,"Zigbee Connecting.....");
	delay_ms(1000);		
	BMP180_Test();		
	Tm1638_Test();	 			    	 
	
	POINT_COLOR=RED;
 	POINT_COLOR=BLUE;  





	while(1) //����GBK�ӿ�
	{
		
		//bmp180Convert();
		DATA_Diplay();
		////�����б�
    keyDat=Read_key();
		keyDat=keyDat-16;
		if(keyDat!=0x37)
		{
			if(keyDat!=t_kdat)
				{
					temp_keydat=keyDat & 0x0f;
					LED8_Display(3,temp_keydat);
					temp_keydat=(keyDat & 0xf0)>>4;
					LED8_Display(2,temp_keydat);						
   /////��������
			switch(keyDat)	
			{
				case 0x01:
						//USART2_Send(S1_Wave,6);
					break;
						//USART2_Send(S2_Wave,6);
				case 0x02:
						//USART2_Send(S3_Wave,6);
					break;
				case 0x03:
						//USART2_Send(Mp3_Stop,4);
					break;
				case 0x04:
						//USART2_Send(S5_Wave,6);
					break;
				case 0x05:
						LCD_Fill(10,100,120,175,RED); 
						LCD_ShowString(40,125,"Wave1");	
				    LCD_DrawRectangle(10,100,120,175);
					break;
				case 0x06:
						LCD_Fill(10,100,120,175,GREEN); 
						LCD_ShowString(40,125,"mode1");	
				    LCD_DrawRectangle(10,100,120,175);
					break;
			 case 0x08:
					//	USART2_Send(Mp3_Play,4);
					break;
				 default:
			}				
				}
				t_kdat=keyDat;
  }
  ///////////////////////////////////////

		
		fontcnt=0;
		for(i=0x81;i<0xff;i++)
		{		
			fontx[0]=i;
			POINT_COLOR=BLACK;
			//LCD_ShowNum(138,150,i,3,16);//��ʾ������ֽ�    
			for(j=0x40;j<0xfe;j++)
			{
				if(j==0x7f)continue;
				fontcnt++;
			 	fontx[1]=j;
				//Show_Font(138,190,fontx,16,0);	  
				t=200;
				while(t--)//��ʱ,ͬʱɨ�谴��
				{										  
					delay_ms(1);
				}
				
		if(t!=timer.sec)
		{
			t=timer.sec;
			LCD_ShowNum(138,270,timer.w_year,4,16);	
			//LCD_ShowString(154,270,"-");			
			LCD_ShowNum(178,270,timer.w_month,2,16);	
			//LCD_ShowString(178,270,"-");			
			LCD_ShowNum(202,270,timer.w_date,2,16);	 
		switch(timer.week)
			//switch(7)
			{
				case 0:
					LCD_ShowString(50,280,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(50,280,"Monday   ");
					break;
				case 2:
					LCD_ShowString(50,280,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(50,280,"Wednesday");
					break;
				case 4:
					LCD_ShowString(50,280,"Thursday ");
					break;
				case 5:
					LCD_ShowString(50,280,"Friday   ");
					break;
				case 6:
					LCD_ShowString(50,280,"Saturday ");
					break;  
			}
			LCD_ShowNum(138,290,timer.hour,2,16);	
      LCD_ShowString(154,290,":");			
			LCD_ShowNum(170,290,timer.min,2,16);	
			LCD_ShowString(186,290,":");			
			LCD_ShowNum(202,290,timer.sec,2,16);

			//LED=!LED;
			}   
		}			 		   
	}
}
	
	/*Touch_Init();
	delay_ms(1500);
	Load_Drow_Dialog();	
	
	while(1)
	{	
		if(Pen_Point.Key_Sta==Key_Down)//������������
		{
			Pen_Int_Set(0);//�ر��ж�
			do
			{
				Convert_Pos();
				Pen_Point.Key_Sta=Key_Up;
				if(Pen_Point.X0>216&&Pen_Point.Y0<16)Load_Drow_Dialog();//���
				else 
				{
					Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0);//��ͼ	    
					GPIOA->ODR|=1<<0;    //PA0 ����	   
				}
			}while(PEN==0);//���PENһֱ��Ч,��һֱִ��
			Pen_Int_Set(1);//�����ж�
		}else delay_ms(10);
		if(key==1)//KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE);//����
		  Touch_Adjust();  //��ĻУ׼ 
			Save_Adjdata();	 
			Load_Drow_Dialog();
		}
		i++;
		if(i==20)
		{
			i=0;
			LED=!LED;
		}
	}*/
}

