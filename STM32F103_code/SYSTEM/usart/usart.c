#include "sys.h"
#include "usart.h"	  
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

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
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
extern int flagpai;  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}




extern u16 len;//���ش���1���ݳ���
extern u16 t;  
int j=0,r=0,xun=0;
extern u32 time_s[200];//�洢����1��������
extern u8 uart_content[12];			//������ʾ��������
extern u8 i;
extern u32 goods[100][4];
int rx=0,ry=0;//����xy�����жϼ���
extern u8 action [6];
extern u8 target[10];//Ŀ����

//goods��1����x���������������͸���
//goods��2����x�����������x����
//goods��3����x�����������y����
//"Red_Block", "Rubik_Cube", "Bule_Block", "Snowflake_Beer", "Red_Bull", "Tennis", "AD_Calcium_Milk", "Deluxe_Milk"
//1����ɫ���飻2��ħ����3����ɫ���飻4��ѩ��ơ�ƣ�5����ţ��6������7��AD�ƣ�8��������

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}  
				

				
		 if(USART_RX_STA&0x8000)
		  {		

						
				len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
					for(t=0;t<len;t++)
					{
						time_s[t]=USART_RX_BUF[t];
					}
				  
				  	j++;
					printf("%d",j);
				 for(i=0;i<len;i++)
					{
	          sprintf((char*)uart_content,"%c ",time_s[i]); 

									
					}
			
				 
				xun++;//��������
				rx=0;
			  for(i=0;i<len;i++)
					{
						flagpai=0;	
						if(time_s[i]=='E'&&time_s[i+1]=='N'&&time_s[i+2]=='D')
						{
								flagpai=1;
								delay_ms(100);
							break;
						}
						else if(time_s[i]=='S'&&time_s[i+1]=='T')
						{
                    goods[xun][0]=time_s[i+2]-48;
										if(time_s[i+3]=='x')
										{
											 for(rx=i+3;time_s[rx+1]!='y';rx++)
												{
												
												goods[xun][1]=goods[xun][1]*10+(time_s[1+rx]-48);	
											
												}
										}
											
										for(r=i;time_s[r+1]!='n';r++)
										{
											if(time_s[r]=='y')
											{
												 for(ry=r;time_s[ry+1]!='n';ry++)
												 {
													 
													goods[xun][2]= goods[xun][2]*10+(time_s[ry+1]-48);
													
												 }
											}
											
										}
                 if(goods[xun][2]>180)
								 {
									 if(goods[xun][1]<107)
									 {
										 goods[xun][3]=4;//���������·�
									 }
									 else if(goods[xun][1]>213)
									 {
										 goods[xun][3]=6;//���������·�
									 }
									 else
									 {
									 goods[xun][3]=5;//����������
									 }

								 }
                 else
								 {
									 if(goods[xun][1]<107)
									 {
										 goods[xun][3]=1;//���������Ϸ�
									 }
									 else if(goods[xun][1]>213)
									 {
										 goods[xun][3]=3;//���������Ϸ�
									 }
									 else
									 {
										goods[xun][3]=2;//����������
									 }

								 }											 

						}
							else{ }

						}
				USART_RX_STA=0;	
			}			
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
	
#endif	

//����2
 #if EN_USART2_RX   //���ʹ���˽���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART2_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  
  
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART2_RX	  GPIOA.��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2

}


u8 USART_RX_STA2[3] = { 0 };       //����״̬���	  
u8 Num2 = 0;              //�������ݵĵ�ǰλ��

/*********************************************************************
 *  �������ƣ�Read_Data(u8 *Data)
 *  �������ܣ���ȡ�������ݲ��������ݻ�����
 *  ��    �Σ�*Data��ָ��
 *  ��    ������
 *  ��    ע��
 *			   	   
 ********************************************************************/    
void Read_Data1(u16 *Data)	       
{	
	u8 y=0;
	u16 Receive_data = 0;       //���ݻ�����
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, 0x57);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	
/////////////////////////��������ֵ///////////////////////////////	
	
//	while(Num != 1)
//	{
//		
//		Delay_ms(1);
//		if(y>5)
//		{
//			y=0;
//			break;
//		}
//		y++;
//	}
//	*Data = USART_RX_STA3[0];
	
/////////////////////////��������ֵ///////////////////////////////	

///////////////////////////ƫ������ֵ///////////////////////////////	
	
	while(Num2 != 3)
	{
		
		delay_ms(1);
		if(y>10)
		{
			y=0;
			break;
		}
		y++;
	}
	Receive_data = USART_RX_STA2[1];
	Receive_data <<= 8;
	Receive_data |= USART_RX_STA2[2];

	*Data = USART_RX_STA2[0];
	*(Data+1) = Receive_data;
	
///////////////////////////ƫ������ֵ///////////////////////////////	

	Num2 = 0; 
}



/*********************************************************************
 *  �������ƣ�void USART3_IRQHandler
 *  �������ܣ�����3�жϷ�����
 *  ��    �Σ���
 *  ��    ������
 *  ��    ע����
 ********************************************************************/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_RX_STA2[Num2++] =USART_ReceiveData(USART2);	//��ȡ���յ�������
	} 	
	
	
}




   #endif

//����3

u8 USART_RX_BUF3[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART_RX_STA3 = 0;       //����״̬���	  

int group_done=0;

void uart3_init(u32 bound){
  //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //ʹ��UART3����GPIOB��ʱ��  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
  
	 //����ʹ�õ�GPIO������  
 // Configure USART3 Rx (PB.11) as input floating    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
   
  //USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart1 NVIC ����
	 NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //�����ȼ�3
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
     NVIC_Init(&NVIC_InitStructure);  
  
   //USART ��ʼ������

	 USART_InitStructure.USART_BaudRate = bound;  
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;  
     USART_InitStructure.USART_Parity = USART_Parity_No;  
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  

	 USART_Init(USART3, &USART_InitStructure);//���ô���3
	 USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
	 USART_Cmd(USART3, ENABLE);//ʹ�ܴ���3  

}


void USART3_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
/////////////////	
		if((USART_RX_STA3&0x8000)==0)//����δ���
			{
			if(USART_RX_STA3&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA3=0;//���մ���,���¿�ʼ
				else USART_RX_STA3|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA3|=0x4000;
				else
					{
					USART_RX_BUF3[USART_RX_STA3&0X3FFF]=Res ;
					USART_RX_STA3++;
					if(USART_RX_STA3>(USART_REC_LEN-1))USART_RX_STA3=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}
/////////////////
			if(Res == '@')
			{
				group_done = 1;
			}
			
			
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}


//����4
u8 USART_RX_BUF4[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_RX_STA4[3] = { 0 };       //����״̬���

void Usart4_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);//for UART4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//USART3_TX		PC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//USART3_RX		PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//�Դ���3ͨ��Э����г�ʼ������
	USART_InitStructure.USART_BaudRate = 9600;						//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//����żЧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//˫��ͨ��
	USART_Init(UART4, &USART_InitStructure);
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);					//���������ж�	
	USART_Cmd(UART4, ENABLE);

	//�Դ���3�շ��ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					//�ж���ѡ�ڶ���
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;  				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  		//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);
}

	  
u8 Num4 = 0;              //�������ݵĵ�ǰλ��

/*********************************************************************
 *  �������ƣ�Read_Data(u8 *Data)
 *  �������ܣ���ȡ�������ݲ��������ݻ�����
 *  ��    �Σ�*Data��ָ��
 *  ��    ������
 *  ��    ע��
 *			   	   
 ********************************************************************/    
void Read_Data(u16 *Data)	       
{	
	u8 y=0;
	u16 Receive_data = 0;       //���ݻ�����
	
	while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
	USART_SendData(UART4, 0x57);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
	
/////////////////////////��������ֵ///////////////////////////////	
	
//	while(Num4 != 1)
//	{
//		
//		Delay_ms(1);
//		if(y>5)
//		{
//			y=0;
//			break;
//		}
//		y++;
//	}
//	*Data = USART_RX_STA4[0];
	
/////////////////////////��������ֵ///////////////////////////////	

///////////////////////////ƫ������ֵ///////////////////////////////	
	
	while(Num4 != 3)
	{
		
		delay_ms(1);
		if(y>10)
		{
			y=0;
			break;
		}
		y++;
	}
	Receive_data = USART_RX_STA4[1];
	Receive_data <<= 8;
	Receive_data |= USART_RX_STA4[2];

	*Data = USART_RX_STA4[0];
	*(Data+1) = Receive_data;
	
///////////////////////////ƫ������ֵ///////////////////////////////	

	Num4 = 0; 
}


/*********************************************************************
 *  �������ƣ�void USART3_IRQHandler
 *  �������ܣ�����3�жϷ�����
 *  ��    �Σ���
 *  ��    ������
 *  ��    ע����
 ********************************************************************/
void UART4_IRQHandler(void)                    //UART4 Receive Interrupt 
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_RX_STA4[Num4++] =USART_ReceiveData(UART4);	//��ȡ���յ�������
	} 	
	
	
}




 