#ifndef _key_H
#define _key_H


#include "sys.h"
 
#define KEY_LEFT_Pin    GPIO_Pin_5    //����K_LEFT�ܽ�
#define KEY_DOWN_Pin    GPIO_Pin_3    //����K_DOWN�ܽ�
#define KEY_RIGHT_Pin   GPIO_Pin_4   //����K_RIGHT�ܽ�
#define KEY_UP_Pin      GPIO_Pin_0  //����KEY_UP�ܽ�

#define KEY_Port (GPIOE) //����˿�
#define KEY_UP_Port (GPIOA) //����˿�


//ʹ��λ��������
#define K_UP PAin(0) //����
#define K_DOWN PEin(3)
#define K_LEFT PEin(5)
#define K_RIGHT PEin(4)




//�����������ֵ  
#define KEY_UP 1  //����
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4  



void KEY_Init(void);
int KEY_Scan(int mode);
#endif
