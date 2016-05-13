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
#include "tm1638.h"
#include "ff.h"
#include "integer.h"
#include "diskio.h"
#include "stdio.h"
#include "string.h"
#include "esp8266.h"
#include "rtc.h" 
#include "bmp180.h"
#include "touch.h" 

bool DisableWarning = false;

const u8 *COMPILED_DATE=__DATE__;//��ñ�������
const u8 *COMPILED_TIME=__TIME__;//��ñ���ʱ��
const u8* Week[7]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

unsigned char const keytab[]={0x01,0x02,0x10,0x0f,0x11,0x12,0x00,0x00,0x03,0x04,0x0e,0x0d,0x13,0x14,0x00,0x00,0x05,0x06,0x0c,0x0b,0x15,0x16,0x00,0x00,0x07,0x08,0x0a,0x09,0x17,0x18,00};	
unsigned char const tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};//�����0��F����ʾ

//����mp3ָ��
u8 prev[4]  = {0x7E, 0x02, 0x02, 0xEF};
u8 next[4]  = {0x7E, 0x02, 0x01, 0xEF};
u8 play[4]  = {0x7E, 0x02, 0x0D, 0xEF};
u8 pause[4] = {0x7E, 0x02, 0x0E, 0xEF};
u8 vol_i[4] = {0x7E, 0x02, 0x04, 0xEF};
u8 vol_d[4] = {0x7E, 0x02, 0x05, 0xEF};
u8 stop[4]  = {0x7E, 0x02, 0x10, 0xEF};
u8 mode_u[4]  = {0x7E, 0x02, 0, 0xEF};			//U��
u8 mode_tf[4] = {0x7E, 0x02, 0x01, 0xEF};   //TF
u8 mode_flash[4] = {0x7E, 0x02, 0x04, 0xEF};//flash
u8 num[4] = {0x7E, 0x02, 0x48, 0xEF};				//ask song num from U pan
u8 point_fire[6] = {0x7E, 0x04, 0x03, 0x00, 0x01, 0xEF};		//fire
u8 point_rain[6] = {0x7E, 0x04, 0x03, 0x00, 0x02, 0xEF};		//rain
u8 point_metal[6] = {0x7E, 0x04, 0x03, 0x00, 0x03, 0xEF};		//metal
u8 point_vibration[6] = {0x7E, 0x04, 0x03, 0x00, 0x04, 0xEF};		//vibration	

//����ZIGBEEָ��
u8 AskAdd[21] = {		//ask node's address
0xFE, 0x0F, 0x29, 0x00 , 0x02 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x01 , 0x00 , 0x02 , 0x00 , 0x05 , 0x00 , 0x14 , 0x00 , 0x15 , 0x22 , 0x00
};

u8 AskStat[14] = {					//AskStat[5:6] = node's address 
0xFE , 0x09 , 0x29 , 0x00 , 0x02 , 0x79 , 0xE1 , 0x00 , 0x01 , 0x04 , 0x01 , 0x04 , 0x02 , 0xB8
};																	

extern const unsigned char gImage_1[38400];
extern const unsigned char gImage_2[38400];
extern const unsigned char gImage_3[38400];
extern const unsigned char gImage_4[38400];
extern uint8_t message[10];
extern  bool OK_flag;
extern  bool CONNECT_flag;
extern  bool NodeAddress;
extern  bool NodeStat;

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(216,0,"RST");//��ʾ��������
  POINT_COLOR=RED;//���û�����ɫ 
}

void rain_no_warning(void)
{
		LCD_Fill(10,120,120,175,GREEN);
		LCD_ShowString(25, 130, "��δ�����", BLACK, GREEN);
		LCD_ShowString(48, 150, "����", BLACK, GREEN);
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void vibration_no_warning(void)
{
		LCD_Fill(120,120,230,175,GREEN);				
		LCD_ShowString(142, 142, "�񶯱���", BLACK, GREEN);	
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void fire_no_warning(void)
{
		LCD_Fill(10,175,120,230,GREEN);
		LCD_ShowString(30, 195, "���ֱ���", BLACK, GREEN);
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void metal_no_warning(void)
{											
		LCD_Fill(120,175,230,230,GREEN);
		LCD_ShowString(142, 195, "��������", BLACK, GREEN);	
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void rain_warning(void)
{
		USART2_Send(point_rain, 6);
		LCD_Fill(10,120,120,175,RED);
		LCD_ShowString(25, 130, "��δ�����", WHITE, RED);
		LCD_ShowString(48, 150, "����", WHITE, RED);
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void vibration_warning(void)
{
		USART2_Send(point_vibration, 6);
		LCD_Fill(120,120,230,175,RED);				
		LCD_ShowString(142, 142, "�񶯱���", WHITE, RED);	
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void fire_warning(void)
{
		USART2_Send(point_fire,6);
		LCD_Fill(10,175,120,230,RED);
		LCD_ShowString(30, 195, "���ֱ���", WHITE, RED);
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}

void metal_warning(void)
{
		USART2_Send(point_metal,6);												
		LCD_Fill(120,175,230,230,RED);
		LCD_ShowString(142, 195, "��������", WHITE, RED);	
							LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
							LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);		
}



int main(void)
{	  
	u8 i,j;
	FATFS fs;
	u8 key;
	u8 sum = 0;	
	u8 tmp_fcs=0,tmp_len,tmp, keyDat ,t_kdat ,temp_keydat;
	u16 t;
	
	SystemInit();
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();
	
	uart_init1(115200);	//ZIGBEE:115200 / WIFI:115200
	uart_init2(9600);		//mp3
	//USART2_Send("UART2_OK",8);
	//USART1_Send(AskAdd, 21);
	
 	LED_Init();
	LCD_Init();

	init_TM1638();
	AT24CXX_Init();		//IIC��ʼ��
	SPI_Flash_Init();   //SPI FLASH ��ʼ�� 
	POINT_COLOR=RED;
	
	//SPI_Flash_Write((u8 *)gImage_1, 0, 38400);	
	//SPI_Flash_Write((u8 *)gImage_2, 38400, 38400);		
	//SPI_Flash_Write((u8 *)gImage_3, 38400 * 2, 38400);	
	//SPI_Flash_Write((u8 *)gImage_4, 38400 * 3, 38400);

	Touch_Init();
	LCD_Clear(WHITE);//����
  Touch_Adjust();  //��ĻУ׼,���Զ�����	
	
	RTC_Init();

#ifdef Flash_Ziku	
	Ziku();
#endif
	Tm1638_Test();		
	LCD_ShowPic();



	POINT_COLOR=BLACK;
	LCD_DrawRectangle(1,1,239,319, BLACK);
	LCD_DrawRectangle(10,10,230,310, BLACK);
	LCD_Fill(10,120,120,175,GREEN);  LCD_Fill(120,120,230,175,GREEN);
	LCD_Fill(10,175,120,230,GREEN);  LCD_Fill(120,175,230,230,GREEN);
	LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
	LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);

	delay_ms(1000);	

	BMP180_Test();			 			    	 

	POINT_COLOR=RED;
 	POINT_COLOR=BLUE;  

	LCD_ShowString(25, 130, "��δ�����", BLACK, GREEN);
	LCD_ShowString(48, 150, "����", BLACK, GREEN);
	LCD_ShowString(30, 195, "���ֱ���", BLACK, GREEN);
	LCD_ShowString(142, 142, "�񶯱���", BLACK, GREEN);
	LCD_ShowString(142, 195, "��������", BLACK, GREEN);	
  LCD_ShowString(50,245,"Zigbee Connecting...", BLACK, WHITE);
	//LCD_ShowString(50,245,"WIFI Connecting.....", BLACK, WHITE);
	

	
	while(1)
	{	
			USART1_Send(AskAdd, 21);	//ask node's address		
#if 1
			t=200;

			///while(t--)//��ʱ,ͬʱɨ�谴��
			while(t > 2)
			{				
				t --;
				keyDat=Read_key();
				if(keyDat!=0x47)
					{
						keyDat=keyDat-16;
						if(t_kdat!=keyDat)
						{
							temp_keydat=keyDat & 0x0f;
							Write_DATA(3<<1,tab[temp_keydat]);
							temp_keydat=(keyDat & 0xf0)>>4;
							Write_DATA(2<<1,tab[temp_keydat]);							
												
							switch(keyDat)	
							{
								case 0x01:	if(!DisableWarning)	rain_warning();	//rain warning										
									break;
								case 0x02:  if(!DisableWarning) vibration_warning();									
									break;
								case 0x03:  if(!DisableWarning) fire_warning();								
									break;
								case 0x04:  if(!DisableWarning) metal_warning();											
									break;
								case 0x05:
										LCD_Fill(10,120,120,175,GREEN);  LCD_Fill(120,120,230,175,GREEN);
										LCD_Fill(10,175,120,230,GREEN);  LCD_Fill(120,175,230,230,GREEN);	
										LCD_DrawRectangle(10,120,120,175, BLACK); LCD_DrawRectangle(120,120,230,175, BLACK);
										LCD_DrawRectangle(10,175,120,230, BLACK); LCD_DrawRectangle(120,175,230,230, BLACK);					
										LCD_ShowString(25, 130, "��δ�����", BLACK, GREEN);
										LCD_ShowString(48, 150, "����", BLACK, GREEN);
										LCD_ShowString(30, 195, "���ֱ���", BLACK, GREEN);
										LCD_ShowString(142, 142, "�񶯱���", BLACK, GREEN);
										LCD_ShowString(142, 195, "��������", BLACK, GREEN);									
									//close mp3
									break;
								case 0x06:
									DisableWarning = true;
									break;
								case 0x07:
									DisableWarning = false;
									break;									
							 case 0x08:
									break;
								 default:
							}							
						}						
				}
			}	
#endif
			if(NodeAddress == true){
					NodeAddress = false;
					LCD_ShowString(50,245,"  Zigbee Connected  ", BLACK, WHITE);			
				//get node's address
					AskStat[5] = message[39];
					AskStat[6] = message[40];
			
					for(i = 1; i < 13; i++)
						sum ^= AskStat[i];
				
				  AskStat[13] = sum;
				  USART1_Send(AskStat, 14);	
					delay_ms(100);
			}
			else
				LCD_ShowString(50,245,"Zigbee Connecting...", BLACK, WHITE);
			
			if(NodeStat == true){
				NodeStat = false;			
				if(message[20] == 1){		//warning
					switch(message[18]){
						case 04:	rain_warning();
											break;
						case 05: vibration_warning();	
											break;
						case 06: fire_warning();	
											break;
						case 07: metal_warning();	
											break;
					}		
				}else{
					switch(message[18]){
						case 04:	rain_no_warning();
											break;
						case 05: vibration_no_warning();	
											break;
						case 06: fire_no_warning();	
											break;
						case 07: metal_no_warning();	
											break;	
					}						
				}
				
			}
	BMP180_Test();
	}
}
