#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PWMD   TIM8->CCR2  
#define PWMC   TIM8->CCR1  
#define PWMB   TIM8->CCR4 
#define PWMA   TIM8->CCR3 
#define IND   PCout(12)  
#define INC   PDout(2)  
#define INB   PCout(5)  
#define INA   PCout(4)  
 
void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
#endif
