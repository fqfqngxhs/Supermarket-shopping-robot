#include "control.h"
#include "motor.h"
#include "led.h"
extern int target1;
u32 myabs(long int a)
{ 		   
	u32 temp;
	if(a<0)  temp=-a;  
	else temp=a;
	return temp;
}

void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
{
	int siqu=0;
   	if(motor_a>0)
	{
		PWMA=motor_a+siqu;
		INA=0;
	}				
	else  
	{
		PWMA=7200+motor_a-siqu;
		INA=1;
	}   			
	
	
	if(motor_b>0)
	{
		PWMB=motor_b+siqu;
		INB=0;
	}			
	else 
	{
		PWMB=7200+motor_b-siqu;
		INB=1;
		
	}		

	if(motor_c>0)
	{
		PWMC=motor_c+siqu;
		INC=0;
	}		
	else 	
	{
	    PWMC=7200+motor_c-siqu;
		INC=1;
	}		

	if(motor_d>0)		
	{
		PWMD=motor_d+siqu;
		IND=0;
	}		
	else 
	{
	    PWMD=7200+motor_d-siqu;
		IND=1;
	}		
}

extern int Motor_A,Motor_B,Motor_C,Motor_D;
void Xianfu_Pwm(int amplitude)
{	
    if(Motor_A<-amplitude) Motor_A=-amplitude;	
	if(Motor_A>amplitude)  Motor_A=amplitude;	
	if(Motor_B<-amplitude) Motor_B=-amplitude;	
	if(Motor_B>amplitude)  Motor_B=amplitude;		
	if(Motor_C<-amplitude) Motor_C=-amplitude;	
	if(Motor_C>amplitude)  Motor_C=amplitude;		
	if(Motor_D<-amplitude) Motor_D=-amplitude;	
	if(Motor_D>amplitude)  Motor_D=amplitude;		
}



int PIDcontrolA(int Encoder,int Target,int pwm)
{
		
		int bias;
		static int lastbias=0;
		//int target=1000; 
		float kp=1.5;
		float ki=0;
	
		bias=Encoder-Target;
		pwm+=-kp*bias-(bias-lastbias)*ki;
		lastbias=bias;	
		return pwm;
	
}	
int PIDcontrolB(int Encoder,int Target,int pwm)
{
		int bias;
		static int lastbias=0;
		//int target=1000; 
		float kp=1.5;
		float ki=0;
	
		bias=Encoder-Target;
		pwm+=-kp*bias-(bias-lastbias)*ki;
		lastbias=bias;	
		return pwm;
}	
int PIDcontrolC(int Encoder,int Target,int pwm)
{
		int bias;
		static int lastbias=0;
		//int target=1000; 
		float kp=1.5;
		float ki=0;
	
		bias=Encoder-Target;
		pwm+=-kp*bias-(bias-lastbias)*ki;
		lastbias=bias;	
		return pwm;
}	
int PIDcontrolD(int Encoder,int Target,int pwm)
{
		int bias;
		static int lastbias=0;
		//int target=1000; 
		float kp=1.5;
		float ki=0;
	
		bias=Encoder-Target;
		pwm+=-kp*bias-(bias-lastbias)*ki;
		lastbias=bias;	
		return pwm;
}	




void backwardd(void)
{
	
	int Rtarget1,Ltarget1;
	if((D4 == 0)&&(D5 == 0))    
	{
		Rtarget1=target1;
		Ltarget1=target1;
	}
	else if((D4 == 0)&&(D5 != 0))   
	{
		//motor(38,40);
		Rtarget1=target1+200;
		Ltarget1=target1-200;		
	}
	else if((D4 != 0)&&(D5 == 0))   
	{
		//motor(40,38);
		Rtarget1=target1-200;
		Ltarget1=target1+200;		
	}
	else if((D3 != 0)&&(D4 == 0))   
	{
		//motor(32,40);
		Rtarget1=target1+300;
		Ltarget1=target1-300;		
	}
	else if((D5 == 0)&&(D6 == 0))   
	{
		//motor(40,32);
		Rtarget1=target1-300;
		Ltarget1=target1+300;
	}
	else if((D3 == 0)&&(D4 != 0))    
	{
		//motor(25,40);
		Rtarget1=target1+400;
		Ltarget1=target1-400;		
	}
	else if((D5 != 0)&&(D6 == 0))   
	{
		//motor(40,25);
		Rtarget1=target1-400;
		Ltarget1=target1+400;		
	}
	else if((D2 == 0)&&(D3 == 0))   
	{
		//motor(20,40);
		Rtarget1=target1+500;
		Ltarget1=target1-500;		
	}
	else if((D6 == 0)&&(D7 == 0))   
	{
		//motor(40,20);
		Rtarget1=target1-500;
		Ltarget1=target1+500;		
	}
	else if((D2 == 0)&&(D3 != 0))    
	{
		//motor(15,40);
		Rtarget1=target1+600;
		Ltarget1=target1-600;		
	}
	else if((D6 != 0)&&(D7 == 0))  
	{
		//motor(40,15);
		Rtarget1=target1-600;
		Ltarget1=target1+600;		
	}
	else if((D1 == 0)&&(D2 == 0))   
	{
		//motor(10,40);
		Rtarget1=target1+800;
		Ltarget1=target1-800;		
	}
	else if((D7 == 0)&&(D8 == 0))   
	{
		//motor(40,10);
		Rtarget1=target1-800;
		Ltarget1=target1+800;		
	}
	else if((D1 == 0)&&(D2 != 0))    
	{
		//motor(0,40);
		Rtarget1=target1+1000;
		Ltarget1=target1-1000;		
	}	
	else if((D7 !=0)&&(D8 == 0))   
	{
		//motor(40,0);
		Rtarget1=target1-300;
		Ltarget1=target1+300;		
	}
	else   
	{
		//motor(20,20);
		Rtarget1=target1;
		Ltarget1=target1;		
	}
//		
//		pwm1=PIDcontrolA(myabs(Encoder_A),Ltarget,pwm1);	
//		pwm2=PIDcontrolB(myabs(Encoder_B),Rtarget,pwm2);
//		pwm3=PIDcontrolC(myabs(Encoder_C),Ltarget,pwm3);
//		pwm4=PIDcontrolD(myabs(Encoder_D),Rtarget,pwm4);
		Set_Pwm(-Rtarget1*2,-Ltarget1*2,-Rtarget1*2,-Ltarget1*2);	
	
	
			
}




























/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
extern float Velocity_KP,Velocity_KI;
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_C (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_D (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
extern float Position_KP,Position_KI,Position_KD;
int Position_PID_A (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Integral_bias+=Bias;	                                 //求出偏差的积分
	 if(Integral_bias>100000)Integral_bias=10000;
	 if(Integral_bias<-100000)Integral_bias=-10000;
	 Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
	 Last_Bias=Bias;                                       //保存上一次偏差 
	 return Pwm;                                           //增量输出
}
int Position_PID_B (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Integral_bias+=Bias;	                                 //求出偏差的积分
	 if(Integral_bias>100000)Integral_bias=10000;
	 if(Integral_bias<-100000)Integral_bias=-10000;
	 Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
	 Last_Bias=Bias;                                       //保存上一次偏差 
	 return Pwm;                                           //增量输出
}
int Position_PID_C (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Integral_bias+=Bias;	                                 //求出偏差的积分
	 if(Integral_bias>100000)Integral_bias=10000;
	 if(Integral_bias<-100000)Integral_bias=-10000;
	 Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
	 Last_Bias=Bias;                                       //保存上一次偏差 
	 return Pwm;                                           //增量输出
}
int Position_PID_D (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Integral_bias+=Bias;	                                 //求出偏差的积分
	 if(Integral_bias>100000)Integral_bias=10000;
	 if(Integral_bias<-100000)Integral_bias=-10000;
	 Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
	 Last_Bias=Bias;                                       //保存上一次偏差 
	 return Pwm;                                           //增量输出
}



