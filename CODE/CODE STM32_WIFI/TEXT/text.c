#include "sys.h" 
#include "fontupd.h"
#include "flash.h"
#include "lcd.h"
#include "text.h"													   
						            
 
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//str  :�ַ���
//size :�����С
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ    		   
void Show_Str(u8 x,u8 y,u8*str,u8 size,u8 mode)
{												  	  
    u8 bHz=0;     //�ַ���������  	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(LCD_W-size/2))//����
				{				   
					y+=size;
					x=0;	   
				}							    
		        if(y>(LCD_H-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(LCD_W-size))//����
			{	    
				y+=size;
				x=0;		  
			}
	        if(y>(LCD_H-size))break;//Խ�緵��  						     
	        Show_Font(x,y,str,size,mode); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  			 		  
//�õ��ַ����ĳ��� 
u16 my_strlen(u8*str)
{
	u16 len=0;
	while(*str!='\0')
	{
		str++;
		len++;
	}
	return len;
}
//��str1�������str2
//����ԭ��@HYW
//CHECK OK 091118 
void my_stradd(u8*str1,u8*str2)
{				  
	while(*str1!='\0')str1++;
	while(*str2!='\0')
	{
		*str1=*str2;
		str2++;
		str1++;
	}
	*str1='\0';//���������  		 
}  
//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��
//���12*12����!!!
void Show_Str_Mid(u8 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
   	strlenth=my_strlen(str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,str,size,1);
	}
}   

























		  






