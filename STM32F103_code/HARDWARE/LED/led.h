#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
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



u8 digtal(u8 channel);  	//��ȡXͨ������ֵ��0��1�� 1~8	
u8 digtal2(u8 channel);  	//��ȡXͨ������ֵ��0��1�� 1~8	

void chuan_Init(void);
void chuan_Init2(void);

		 				    
#endif
