#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

u32 myabs(long int a);
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d);
void Xianfu_Pwm(int amplitude);

int Incremental_PI_A (int Encoder,int Target);
int Incremental_PI_B (int Encoder,int Target);
int Incremental_PI_C (int Encoder,int Target);
int Incremental_PI_D (int Encoder,int Target);


int PIDcontrolA(int Encoder,int Target,int pwm);
int PIDcontrolB(int Encoder,int Target,int pwm);
int PIDcontrolC(int Encoder,int Target,int pwm);
int PIDcontrolD(int Encoder,int Target,int pwm);


int Position_PID_A (int Encoder,int Target);
int Position_PID_B (int Encoder,int Target);
int Position_PID_C (int Encoder,int Target);
int Position_PID_D (int Encoder,int Target);
#endif
