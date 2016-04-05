

//#include "stm32f10x.h"
#include "tm1638.h"
#include "delay.h"
//#include "gpio.h"
unsigned char const keytab[]={0x01,0x02,0x10,0x0f,0x11,0x12,0x00,0x00,0x03,0x04,0x0e,0x0d,0x13,0x14,0x00,0x00,0x05,0x06,0x0c,0x0b,0x15,0x16,0x00,0x00,0x07,0x08,0x0a,0x09,0x17,0x18,00};	
unsigned char const tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};//�����0��F����ʾ

//TM1638 STB��P9  CLK��PB4    DIO��PB8
////TM1638 STB��PA4  CLK��PA5    DIO��PA6
//#define STB PCout(13)// PC13
/*
 _________________________________________________
| 			|				|				|				|					|				|
|2BYTE	|4BYTE	| 2BYTE | 1BYTE |	32BYTE	| 1BYTE	|
|_______|_______|_______|_______|_________|_______|
| 			|				|				|				|					|				|
|  ֡ͷ	|8LED		| LED		| ����λ|�������� | ����λ|
|_______|_______|_______|_______|_________|_______|
*/
u8  rec_frame[42];

void delay_tim(unsigned char tim)
{
	unsigned char i;
	for(;tim>0;tim--)
	 for(i=0;i<250;i++)
	{;}
}
//��������ʱ����
static Delay_1us(int counter)
{
	int i,j;
	for(i=0;i<counter;i++)
	  for(j=0;j<33;j++);
}
//old
//PA6-->DIO
//PA5-->CLK
//PA4-->STB
//new
//PA8-->DIO
//PB1-->CLK
//PB2-->STB


void DIO_SETOUT() //����TM1638DIO����Ϊ�������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIO_RCC, ENABLE);	 //ʹ��PF�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = DIO_GPIO_PIN;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(DIO_GPIO, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
	DIO_OUT(1);
}
void  DIO_SETIN()  //����TM1638DIO����Ϊ��������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIO_RCC, ENABLE);	 //ʹ��PF�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = DIO_GPIO_PIN;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(DIO_GPIO, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
	
//	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//��ʼ������Ϊ0
	
}
void TM1638_Write(unsigned char	DATA)			//д���ݺ��� Q
{
   unsigned char i;
	
   DIO_SETOUT(); 		
   for(i=0;i<8;i++)
   {
   	//CLK=0
		 CLK_OUT(0);
	  if(DATA&0X01)
		{
	    //DIO=1;
			DIO_OUT(1);
		}
	  else 
		{
	    //DIO=0;
			DIO_OUT(0);
		}
	  DATA>>=1;
	  //CLK=1;
		CLK_OUT(1);
	  delay_tim(10);
   }
	 
}
unsigned char TM1638_Read(void)					//�����ݺ��� Q
{
	unsigned char i;
	unsigned char temp=0;
	DIO_SETIN();
	delay_tim(1);
	//DIO=1;
	for(i=0;i<8;i++)
	{
	  temp>>=1;
	 // CLK=0
		CLK_OUT(0);
		delay_tim(10);
		if(DIO_IN() == Bit_SET)
	    temp|=0x80;
	  //CLK=1
		CLK_OUT(1);
		
		delay_tim(1);
	}
	return temp;
}

void Write_COM(unsigned char cmd)		//����������Q
{ 
	//STB=0;
  STB_OUT(0);
	TM1638_Write(cmd);
	//STB=1;
	STB_OUT(1);
}

unsigned char c[4];
unsigned char Read_key(void)
{
	unsigned char i,key_value=0;
	unsigned char tmp_num[4]={0,0,0,0};
//	int* key;
  // STB=0;
	STB_OUT(0);
	TM1638_Write(0x42);
	delay_tim(200);
	
	for(i=0;i<4;i++)
	{
		c[i]=TM1638_Read();
	}
	//STB=1;				//4���ֽ����ݺϳ�һ���ֽ�
	STB_OUT(1);
	
		for(i=0;i<4;i++)
		{
			if(c[i]!=0)
				switch(c[i])
			{
				case 0x01:tmp_num[i]=i*8+1; break;
				case 0x10:tmp_num[i]=i*8+2; break;
				case 0x02:tmp_num[i]=i*8+3; break;
				case 0x20:tmp_num[i]=i*8+4; break;
				case 0x04:tmp_num[i]=i*8+5; break;
				case 0x40:tmp_num[i]=i*8+6; break;
				case 0x08:tmp_num[i]=i*8+7; break;
				case 0x80:tmp_num[i]=i*8+8; break;
				default : tmp_num[i]=0;break;
			}
		}
		key_value=tmp_num[0]+tmp_num[1]+tmp_num[2]+tmp_num[3];
    key_value=keytab[(key_value-1)];
  return (key_value);
}
void Write_DATA(unsigned char add,unsigned char DATA) //Q
{
    Write_COM(0x44);
   // STB=0
	STB_OUT(0);
	TM1638_Write(0xc0|add);
	TM1638_Write(DATA);
	//STB=1
	STB_OUT(1);
}
void Write_oneLED(unsigned char num,unsigned char flag)	//��������һ��LED������numΪ��Ҫ���Ƶ�led��ţ�flagΪ0ʱϨ�𣬲�Ϊ0ʱ����
{
	Write_DATA(num,flag);
}
void init_TM1638(void) //Q
{	
	unsigned char i;
	
	TM1638_GPIO_Configuration();   //IO�ڳ�ʼ��

	Write_COM(0x8a);//������Ϊȫ���ȵ�4/16
	Write_COM(0x40);
	//STB=0;
	STB_OUT(0);
	TM1638_Write(0xc0);
	for(i=0;i<16;i++)
	TM1638_Write(0x00);
	//STB=1;
	STB_OUT(1);

}

void TM1638_GPIO_Configuration(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIO_RCC, ENABLE);	 //ʹ��DIO�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = DIO_GPIO_PIN;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(DIO_GPIO, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
	DIO_OUT(1);
	
//	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//��ʼ������Ϊ0
	RCC_APB2PeriphClockCmd(CLK_RCC, ENABLE);	 //ʹ��PF�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = CLK_GPIO_PIN;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(CLK_GPIO, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
	CLK_OUT(1);
	
	RCC_APB2PeriphClockCmd(STB_RCC, ENABLE);	 //ʹ��PF�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = STB_GPIO_PIN;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(STB_GPIO, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
	STB_OUT(1);
}
void Tm1638_Test(void)
{
	unsigned char i,j;

	u8 tmp_fcs=0,tmp_len,tmp;
	
	for(i=0;i<16;i++)
	{
	  for(j=0;j<4;j++)
	  Write_DATA(j<<1,tab[i]); 
	  delay_ms(200);
	}

	for(i=0;i<16;i++)
	{ 
		i++;
		Write_oneLED(i,1);//i�ǵڼ���LED��j��LED��״̬
	  delay_ms(200);
	}
	
		/*	while(1)
	{
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
					}						
				}
				t_kdat=keyDat;
  }*/
}

void LED8_Display(unsigned char add,unsigned char DATA)
	{
		Write_DATA(add<<1,tab[DATA]);
	}
		