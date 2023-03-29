#ifndef _key_H
#define _key_H


#include "sys.h"
 
#define KEY_LEFT_Pin    GPIO_Pin_5    //定义K_LEFT管脚
#define KEY_DOWN_Pin    GPIO_Pin_3    //定义K_DOWN管脚
#define KEY_RIGHT_Pin   GPIO_Pin_4   //定义K_RIGHT管脚
#define KEY_UP_Pin      GPIO_Pin_0  //定义KEY_UP管脚

#define KEY_Port (GPIOE) //定义端口
#define KEY_UP_Port (GPIOA) //定义端口


//使用位操作定义
#define K_UP PAin(0) //无用
#define K_DOWN PEin(3)
#define K_LEFT PEin(5)
#define K_RIGHT PEin(4)




//定义各个按键值  
#define KEY_UP 1  //无用
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4  



void KEY_Init(void);
int KEY_Scan(int mode);
#endif
