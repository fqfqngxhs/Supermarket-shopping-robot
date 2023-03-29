#include "timer.h"
#include "encoder.h"
 
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器6
void TIM6_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //使能TIM6时钟
	TIM_TimeBaseStructure.TIM_Period = arr;              //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = psc;           //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;         //采样分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);     //使能TIM6中断
	
	NVIC_InitStructure.NVIC_IRQChannel =TIM6_IRQn;//TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//占先式优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //副优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断使能
	NVIC_Init(&NVIC_InitStructure);                //中断初始化
	TIM_Cmd(TIM6, ENABLE);                        //使能定时器6

}

extern int Encoder_A,Encoder_B,Encoder_C,Encoder_D;         
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//清除更新中断标志位
			//Encoder_A+=100;
			Encoder_A=Read_Encoder(2);                                          //===读取编码器的值
			Encoder_B=-Read_Encoder(3);                                          //===读取编码器的值
			Encoder_C=Read_Encoder(4);                                         //===读取编码器的值
			Encoder_D=-Read_Encoder(5);                                         //===读取编码器的值
		
	}
//	printf("A:%d\r\n",Encoder_A);
//	printf("B:%d\r\n",Encoder_B);
//	printf("C:%d\r\n",Encoder_C);
//	printf("D:%d\r\n",Encoder_D);
//	printf("\r\n\r\n");
}









