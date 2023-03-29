#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 



#define D1 digtal2(1)
#define D2 digtal2(2)
#define D3 digtal2(3)
#define D4 digtal2(4)
#define D5 digtal2(5)
#define D6 digtal2(6)
#define D7 digtal2(7)
#define D8 digtal2(8)



u8 digtal(u8 channel);  	//获取X通道数字值（0，1） 1~8	
u8 digtal2(u8 channel);  	//获取X通道数字值（0，1） 1~8	

void chuan_Init(void);
void chuan_Init2(void);

		 				    
#endif
