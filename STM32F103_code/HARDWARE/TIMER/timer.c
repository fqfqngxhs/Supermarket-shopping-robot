#include "timer.h"
#include "encoder.h"
 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��6
void TIM6_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʹ��TIM6ʱ��
	TIM_TimeBaseStructure.TIM_Period = arr;              //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = psc;           //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;         //������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);     //ʹ��TIM6�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel =TIM6_IRQn;//TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//ռ��ʽ���ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//�ж�ʹ��
	NVIC_Init(&NVIC_InitStructure);                //�жϳ�ʼ��
	TIM_Cmd(TIM6, ENABLE);                        //ʹ�ܶ�ʱ��6

}

extern int Encoder_A,Encoder_B,Encoder_C,Encoder_D;         
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//��������жϱ�־λ
			//Encoder_A+=100;
			Encoder_A=Read_Encoder(2);                                          //===��ȡ��������ֵ
			Encoder_B=-Read_Encoder(3);                                          //===��ȡ��������ֵ
			Encoder_C=Read_Encoder(4);                                         //===��ȡ��������ֵ
			Encoder_D=-Read_Encoder(5);                                         //===��ȡ��������ֵ
		
	}
//	printf("A:%d\r\n",Encoder_A);
//	printf("B:%d\r\n",Encoder_B);
//	printf("C:%d\r\n",Encoder_C);
//	printf("D:%d\r\n",Encoder_D);
//	printf("\r\n\r\n");
}









