#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"

/////////////////////////////////////�û�������///////////////////////////////////	 
//����2���궨�壬������Ļ����ʾ��ʽ��IO�ٶ�
#define USE_HORIZONTAL  0	//�����Ƿ�ʹ�ú��� 		0,��ʹ��.1,ʹ��.
 
//TFTLCD������Ҫ���õĺ���		   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ
//����LCD�ĳߴ�
#if USE_HORIZONTAL==1	//ʹ�ú���
#define LCD_W 400
#define LCD_H 240
#else
#define LCD_W 240
#define LCD_H 400
#endif
////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 
#define	LCD_LED PCout(13) //LCD����    		 PC13  

//����Ƶ����Ϊ������ٶȣ�����ֱ�ӼĴ�������

#define	LCD_CS_SET  GPIOA->BSRR=1<<0    //Ƭѡ�˿�  	 Pa0
#define	LCD_RS_SET	GPIOA->BSRR=1<<1    //����/����    Pa1	   
#define	LCD_WR_SET	GPIOB->BSRR=1<<0    //д����			 Pb0
#define	LCD_RD_SET	GPIOC->BSRR=1<<13    //������			 Pa13
								    
#define	LCD_CS_CLR  GPIOA->BRR=1<<0     //Ƭѡ�˿�  	 Pa0
#define	LCD_RS_CLR	GPIOA->BRR=1<<1     //����/����    Pa1	   
#define	LCD_WR_CLR	GPIOB->BRR=1<<0     //д����			 Pb0
#define	LCD_RD_CLR	GPIOC->BRR=1<<13     //������			 Pa13
							    

//PB8~15,��Ϊ������
//�������Ϊ����Ƶ����Ϊ������ٶȣ�����ֱ���üĴ���������
#define DATAOUT(x) GPIOB->ODR=((GPIOB->ODR&0x00ff)|x); //�������
#define DATAIN     GPIOB->IDR   //��������


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
//GUI��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			     0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    		
					
extern u16 BACK_COLOR, POINT_COLOR ; 


void LCD_Init(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//����
u16  LCD_ReadPoint(u16 x,u16 y); //����
void Draw_Circle(u16 x0,u16 y0,u8 r);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  //��ʾһ������
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);//��ʾ2������
void LCD_ShowString(u16 x,u16 y,const u8 *p);		 //��ʾһ���ַ���,16����
									    
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
u16 LCD_ReadRAM(void);		   

 
#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
DATAOUT(data&0xff00);\
LCD_WR_CLR;\
LCD_WR_SET;\
DATAOUT((0x00ff&data)<<8);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

#define LCD_INIT_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
DATAOUT((0x00ff&data)<<8);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 
				  		 
#endif  
	 
