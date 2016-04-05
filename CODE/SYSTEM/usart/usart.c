#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif 
 //qinyx write
 uint8_t BufferA[20];
 uint8_t BufferB[20];
 uint8_t g_flag1=0,g_flag2=0;
 uint8_t A_count=0,B_count=0;
 uint8_t DBufferFlag1=0,DBufferFlag2=0;
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init1(u32 bound)
{
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
		NVIC_EnableIRQ(USART1_IRQn);
		USART_ClearFlag(USART1, USART_FLAG_TC);

}
void uart_init2(u32 bound)
{
//**************************************************
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//////////////////////////////////////////////////
	   //Usart2 NVIC ����

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
/*	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3/*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��/
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���/  

	/* ��4��������USART����
	    - BaudRate = 115200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - No parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 9600;//115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	/* ��5����ʹ�� USART�� ������� */
	USART_Cmd(USART2, ENABLE);

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
	///////////////////////////////////////////////////////////
	  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
		NVIC_EnableIRQ(USART2_IRQn);	

}
void USART1_Send(uint8_t *str,uint8_t num)
{
	while(num!=0)
	{
    //USART1->DR = (*str++ & (uint16_t)0x01FF);
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		//num--;
		USART_SendData(USART1,*str++ & (uint16_t)0x01FF);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		num--;
	}
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	/*g_flag1=1;
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	BufferA[A_count]=USART1->DR & (uint16_t)0x01FF;
	if(A_count<20)
	{
		A_count=A_count++;
		DBufferFlag1=0;
}
else if((A_count>20)||(A_count==20))
	{
		A_count=0;
		DBufferFlag1=1;
}*/
	
	uint8_t tmp1;
	uint8_t i,j;
	uint16_t tim_dly;
	//USART_SendData(USART1,'G');
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	tmp1=USART1->DR & (uint16_t)0x01FF;
	if(tmp1==0x02)//�ȴ�֡ͷ
	{
				BufferA[0]=tmp1;
				tim_dly=0xffff;
				while(!USART_GetITStatus(USART1,USART_IT_RXNE))
					{
						if(tim_dly==0)
						{
							g_flag1=0;
							return ;
						}
						else tim_dly--;		
					}//�ȴ���һ���ַ��ĵ���
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);
				tmp1=USART1->DR & (uint16_t)0x01FF;
				BufferA[1]=tmp1;
		    j=2;
				for(i=(3+tmp1);i>0;i--)
		    {  
					tim_dly=0xffff;
					while(!USART_GetITStatus(USART1,USART_IT_RXNE))
					{
						if(tim_dly==0)
						{
							g_flag1=0;
							return ;
						}
						else tim_dly--;			
					}//�ȴ���һ���ַ��ĵ���
					USART_ClearITPendingBit(USART1,USART_IT_RXNE);
				  BufferA[j++]=USART1->DR & (uint16_t)0x01FF;
				}
				g_flag1=1;  //����һ������֡����
	}
	

} 
void USART2_Send(uint8_t *str,uint8_t num)
{
	while(num!=0)
	{
    //USART1->DR = (*str++ & (uint16_t)0x01FF);
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		//num--;
		USART_SendData(USART2,*str++ & (uint16_t)0x01FF);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		num--;
	}
}


void USART2_IRQHandler(void)                	//����1�жϷ������
{
	g_flag2=1;
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	BufferB[B_count]=USART2->DR & (uint16_t)0x01FF;
	/*if(B_count<20)
	{
		B_count=B_count++;
		DBufferFlag2=0;
}
else if((B_count>20)||(B_count==20))
	{
		B_count=0;
		DBufferFlag2=1;
}*/
	USART_SendData(USART1,BufferB[B_count]);
} 
#endif	

