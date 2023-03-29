#include "led.h"
 #include "sys.h"



void chuan_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}


unsigned char digtal(unsigned char channel)//1-8	  获取X通道数字值
{
	u8 value = 0;
	switch(channel) 
	{
		case 1:  
			if(PBin(8) == 1) value = 1;
			else value = 0;  
			break;  
		case 2: 
			if(PBin(9) == 1) value = 1;
			else value = 0;  
			break;  
	}
	return value; 
}

void chuan_Init2(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
}


unsigned char digtal2(unsigned char channel)//1-8	  获取X通道数字值
{
	u8 value = 0;
	switch(channel) 
	{
		case 1:  
			if(PGin(0) == 1) value = 1;
			else value = 0;  
			break;  
		case 2: 
			if(PGin(1) == 1) value = 1;
			else value = 0;  
			break;  
		case 3:  
			if(PGin(2) == 1) value = 1;
			else value = 0;  
			break;  
		case 4: 
			if(PGin(3) == 1) value = 1;
			else value = 0;  
			break;  
		case 5:  
			if(PGin(4) == 1) value = 1;
			else value = 0;  
			break;  
		case 6: 
			if(PGin(5) == 1) value = 1;
			else value = 0;  
			break;  
		case 7: 
			if(PGin(6) == 1) value = 1;
			else value = 0;  
			break;  
		case 8: 
			if(PGin(7) == 1) value = 1;
			else value = 0;  
			break;  
	}
	return value; 
}

