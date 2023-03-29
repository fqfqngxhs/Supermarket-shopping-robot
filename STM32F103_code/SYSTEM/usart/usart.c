#include "sys.h"
#include "usart.h"	  
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
extern int flagpai;  
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}




extern u16 len;//传回串口1数据长度
extern u16 t;  
int j=0,r=0,xun=0;
extern u32 time_s[200];//存储串口1传回数据
extern u8 uart_content[12];			//用于显示传回数据
extern u8 i;
extern u32 goods[100][4];
int rx=0,ry=0;//用于xy坐标判断计数
extern u8 action [6];
extern u8 target[10];//目标物

//goods【1】【x】代表物体的种类和个数
//goods【2】【x】代表物体的x坐标
//goods【3】【x】代表物体的y坐标
//"Red_Block", "Rubik_Cube", "Bule_Block", "Snowflake_Beer", "Red_Bull", "Tennis", "AD_Calcium_Milk", "Deluxe_Milk"
//1：红色方块；2：魔方；3：蓝色方块；4：雪花啤酒；5：红牛；6：网球；7：AD钙；8：特仑苏

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}  
				

				
		 if(USART_RX_STA&0x8000)
		  {		

						
				len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
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
			
				 
				xun++;//种类数量
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
										 goods[xun][3]=4;//物体在左下方
									 }
									 else if(goods[xun][1]>213)
									 {
										 goods[xun][3]=6;//物体在右下方
									 }
									 else
									 {
									 goods[xun][3]=5;//物体在中下
									 }

								 }
                 else
								 {
									 if(goods[xun][1]<107)
									 {
										 goods[xun][3]=1;//物体在左上方
									 }
									 else if(goods[xun][1]>213)
									 {
										 goods[xun][3]=3;//物体在右上方
									 }
									 else
									 {
										goods[xun][3]=2;//物体在中上
									 }

								 }											 

						}
							else{ }

						}
				USART_RX_STA=0;	
			}			
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
	
#endif	

//串口2
 #if EN_USART2_RX   //如果使能了接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART2_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记	  
  
void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART2_RX	  GPIOA.初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口2

}


u8 USART_RX_STA2[3] = { 0 };       //接收状态标记	  
u8 Num2 = 0;              //接收数据的当前位置

/*********************************************************************
 *  函数名称：Read_Data(u8 *Data)
 *  函数功能：读取串口数据并赋给数据缓存区
 *  形    参：*Data，指针
 *  输    出：无
 *  备    注：
 *			   	   
 ********************************************************************/    
void Read_Data1(u16 *Data)	       
{	
	u8 y=0;
	u16 Receive_data = 0;       //数据缓存区
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, 0x57);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	
/////////////////////////数字量数值///////////////////////////////	
	
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
	
/////////////////////////数字量数值///////////////////////////////	

///////////////////////////偏移量数值///////////////////////////////	
	
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
	
///////////////////////////偏移量数值///////////////////////////////	

	Num2 = 0; 
}



/*********************************************************************
 *  函数名称：void USART3_IRQHandler
 *  函数功能：串口3中断服务函数
 *  形    参：无
 *  输    出：无
 *  备    注：无
 ********************************************************************/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		USART_RX_STA2[Num2++] =USART_ReceiveData(USART2);	//读取接收到的数据
	} 	
	
	
}




   #endif

//串口3

u8 USART_RX_BUF3[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA3 = 0;       //接收状态标记	  

int group_done=0;

void uart3_init(u32 bound){
  //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //使能UART3所在GPIOB的时钟  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
  
	 //串口使用的GPIO口配置  
 // Configure USART3 Rx (PB.11) as input floating    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
   
  //USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart1 NVIC 配置
	 NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级3
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
     NVIC_Init(&NVIC_InitStructure);  
  
   //USART 初始化设置

	 USART_InitStructure.USART_BaudRate = bound;  
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;  
     USART_InitStructure.USART_Parity = USART_Parity_No;  
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  

	 USART_Init(USART3, &USART_InitStructure);//配置串口3
	 USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
	 USART_Cmd(USART3, ENABLE);//使能串口3  

}


void USART3_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
/////////////////	
		if((USART_RX_STA3&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA3&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA3=0;//接收错误,重新开始
				else USART_RX_STA3|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA3|=0x4000;
				else
					{
					USART_RX_BUF3[USART_RX_STA3&0X3FFF]=Res ;
					USART_RX_STA3++;
					if(USART_RX_STA3>(USART_REC_LEN-1))USART_RX_STA3=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}
/////////////////
			if(Res == '@')
			{
				group_done = 1;
			}
			
			
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}


//串口4
u8 USART_RX_BUF4[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_RX_STA4[3] = { 0 };       //接收状态标记

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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//USART3_RX		PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//对串口3通信协议进行初始化设置
	USART_InitStructure.USART_BaudRate = 9600;						//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶效验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//双向通信
	USART_Init(UART4, &USART_InitStructure);
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);					//开启接收中断	
	USART_Cmd(UART4, ENABLE);

	//对串口3收发中断设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					//中断组选第二组
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;  				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  		//先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  			//从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);
}

	  
u8 Num4 = 0;              //接收数据的当前位置

/*********************************************************************
 *  函数名称：Read_Data(u8 *Data)
 *  函数功能：读取串口数据并赋给数据缓存区
 *  形    参：*Data，指针
 *  输    出：无
 *  备    注：
 *			   	   
 ********************************************************************/    
void Read_Data(u16 *Data)	       
{	
	u8 y=0;
	u16 Receive_data = 0;       //数据缓存区
	
	while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
	USART_SendData(UART4, 0x57);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
	
/////////////////////////数字量数值///////////////////////////////	
	
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
	
/////////////////////////数字量数值///////////////////////////////	

///////////////////////////偏移量数值///////////////////////////////	
	
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
	
///////////////////////////偏移量数值///////////////////////////////	

	Num4 = 0; 
}


/*********************************************************************
 *  函数名称：void USART3_IRQHandler
 *  函数功能：串口3中断服务函数
 *  形    参：无
 *  输    出：无
 *  备    注：无
 ********************************************************************/
void UART4_IRQHandler(void)                    //UART4 Receive Interrupt 
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
	{
		USART_RX_STA4[Num4++] =USART_ReceiveData(UART4);	//读取接收到的数据
	} 	
	
	
}




 