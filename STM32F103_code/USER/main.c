#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "motor.h"
#include "oled.h"
#include "show.h"
#include "encoder.h"
#include "control.h"
#include "timer.h"
#include "key.h"
#include "led.h"

/*############################################
��е�ۺ���action���ô��������ض���printf.
#############################################*/
//����1�õ�����
u8 i;
u16 t;  
u16 len;//���ش���1����
u32 time_s[200];//�洢����1��������
u8 uart_content[12];			//������ʾ��������
void take_phone(int flag);
u32 goods[100][4];//���ڴ洢ʱʱ������������
int flagpai=0;//���ս�����־λ		
///////////����Ϊͼ��ʶ�𲿷�

//goods��1����x���������������͸���
//goods��2����x�����������x����
//goods��3����x�����������y����
//"Red_Block", "Rubik_Cube", "Bule_Block", "Snowflake_Beer", "Red_Bull", "Tennis", "AD_Calcium_Milk", "Deluxe_Milk"
//1����ɫ���飻2��ħ����3����ɫ���飻4��ѩ��ơ�ƣ�5����ţ��6������7��AD�ƣ�8��������
int targetnumber=0;//��¼ʶ��������������
int targetthing[10];//��¼ʶ�����������

extern int xun;

extern int group_done;

long int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //���������������
long int Motor_A,Motor_B,Motor_C,Motor_D;        //���PWM����
long int Target_A,Target_B,Target_C,Target_D;     //���Ŀ��ֵ
float Position_KP=6,Position_KI=0,Position_KD=3;  //λ�ÿ���PID����
float Velocity_KP=10,Velocity_KI=10;	          //�ٶȿ���PID����
int flagzhuan=0;
int flagg=0;
void track_PID(void);
void backwardd(void);
void cemianxunjihuizhuan(void);//����8·ѭ���ж�
void cemianxunji(void);//����8·ѭ���ж�
void zhuanwanchoose(void);//ת��ѡ��
int number=0;//��¼�ڼ���ʮ��·��
int flag=0;//��·��־
void zhuanxiang(int i);//����ת�䶯��
int Rtarget,Ltarget;
int target=500;
int target1=1000;

void back(int i);
void go(int i);
void zhuan(int i);
void gofirst(void);
void gogo(int i);
int pwm1=1000;
int pwm2=1000;
int pwm3=1000;
int pwm4=1000;
int loading=0;
//int remenant=0;//�ϴ�ʣ�����Ʒ
//int flagremenant=0;
//int g1=1;
//int flagremenant2=0;
int flagt=100;//ת��ʱ����2��ѭ���������Ķ���ʱ

///////////������ѭ������


void jixiebi_init()
{
	printf("#001PSCK+085!#002PSCK-280!#003PSCK-050!#004PSCK-300!");
	delay_ms(50000);
	printf("$DGT:1-1,1!");
	group_done = 0;
	delay_ms(1000);
}

void my_init()
{
	JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
	delay_init();	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // �����ж����ȼ�����2
	uart_init(115200);
	uart2_init(9600);
	uart3_init(9600);
	Usart4_Init();
	KEY_Init();
	OLED_Init();
	OLED_ColorTurn(0);                          //0������ʾ��1 ��ɫ��ʾ
	OLED_DisplayTurn(0);                        //0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Clear();
	Encoder_Init_TIM2();
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
	Encoder_Init_TIM5();
	MiniBalance_PWM_Init(7199,0);
	TIM6_Int_Init(100,7199);                //10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
	chuan_Init();
	chuan_Init2();
	uart3_init(115200);
	jixiebi_init();
}

void oledchoose()
{
		u8 key;
	int momentobj=0;//��¼��ǰѡ�����Ʒ�����������λ��
	int flag=0;
	int i,j;
		OLED_Clear();
		OLED_ShowString(0,0,"Choose Things",16);
		OLED_ShowString(0,16,"1 2 3 4 5 6 7 8",16);	
		OLED_ShowString(16*momentobj,32,"o",16);	
		OLED_Refresh();
		
	while(1)
	{
		
		key=KEY_Scan(0);   //ɨ�谴��
		switch(key)
		{
			
			case KEY_DOWN: 
					flag=3;
					break;    //����K_DOWN����  Ϩ��D2ָʾ��
			case KEY_LEFT: 					
				momentobj++;	   //��ǰ��Ʒ������
				if(momentobj==8)momentobj=0;
				flag=1;		
				break;    //����K_DOWN����  Ϩ��D2ָʾ��
			case KEY_RIGHT:
				flag=2;		
				break;    //����K_DOWN����  Ϩ��D2ָʾ��
		}

		delay_ms(10);	
		
		
		if(flag==1)
		{
			OLED_Clear();
			OLED_ShowString(0,0,"Choose Things",16);
			OLED_ShowString(0,16,"1 2 3 4 5 6 7 8",16);	
			OLED_ShowString(16*momentobj,32,"o",16);	
			
			for(j=1;j<=targetnumber;j++) OLED_ShowNum(16*(j-1),48,targetthing[j],1,16);
					
			OLED_Refresh();
			flag=0;
		}	
		
		if(flag==2)
		{
			flag=0;
			targetnumber++;
			targetthing[targetnumber]=momentobj+1;
			momentobj=0;
			
			OLED_Clear();
			OLED_ShowString(0,0,"Choose Things",16);
			OLED_ShowString(0,16,"1 2 3 4 5 6 7 8",16);	
			OLED_ShowString(16*momentobj,32,"o",16);	
			
				
			for(j=1;j<=targetnumber;j++) OLED_ShowNum(16*(j-1),48,targetthing[j],1,16);
					
			OLED_Refresh();		
	
		}
		
		if(flag==3) //DOWN ������
		{
			break;
		}		
		
	}	
}




int luxian1[20]={3,0,3,0,3,0,3,0,3,0,3,0,3,0,1,3,0,3,0,0};	
int luxian[20]={3,0,3,0,3,0,3,0,3,0,3,0,3,0,1,3,0,3,0,0};	
int luflag[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int dangqianweizhi=0;
void xunjigo(void);
void takegoods(void);
void putdown(void);
int tttt,tttt2,mubiaoweizhi[20];//tttt����Ŀ����������

int wutinumber[30];//ÿ��·�ڵ�Ŀ���������
int wutinumber1[30];//���ڱ���
int wutiweizhi[30][30];//ÿ��·�ڵ�Ŀ������λ��
int wutizhonglei[30][30];//ÿ��·�ڵ���������
int wutisum[30];	
	
int mubiao[20]={2,3,4,5,6,1};
int lukou=0;
int xuhao=0;
int a=0;
int main(void)
{	
	int i,j;
	int finish=0;
	int weizhi=1;
	
	int everynumber=0;
	int oldxun=0;
	
	my_init();
	
	
	oledchoose();  //����ѡ����Ʒ
	gofirst();
	delay_ms(1000);
	
while(1)//ɨ��ʶ��ѭ��1
{		
		lukou++;
		wutinumber[lukou]=0;//·����������
		xuhao=0;
		
		oldxun=xun;
		USART_SendData(USART1,1);//�򴮿�1��������
		while(1)
		{
			if(flagpai==1)
			{						
				break;
			}
		}
		flagpai=0;

			
			//���������ĵ���Ŀ��
			for(i=oldxun+1;i<xun;i++)
			{
				for(j=0;j<=targetnumber;j++)
				{
							if(targetthing[j]==goods[i][0])
							{
									wutinumber[lukou]++;  //��ǰ·�ڵ�������
									wutinumber1[lukou]++;
									xuhao++;  //��ǰ����ʶ�𵽵��������
									wutiweizhi[lukou][xuhao]=goods[i][3]; //��Ӧλ��
							}
				}
			}	
			
		if(lukou==6) break; //�ߵ����
		
		go(1);

}
	
//һ�ż�����///////////////////////////////////////////
	zhuan(1);//ԭ�ش�ת��������
	delay_ms(500);
	zhuan(1);

	lukou=6;
	loading=0;
	xuhao=1;
	weizhi=6;


while(1) //�˶�ѭ��1
{
	 
	while(1)  //�˶�ѭ��2 - ����
	{
		xuhao=1;	

		if(lukou==0)
		{
			finish=1;
			break;
		}		
		
		gogo(weizhi-lukou);
		weizhi=lukou;
		

		
		if(wutinumber[lukou]>0)//��·������Ŀ�����塢��ȥץȡ
		{
				zhuan(1);
				go(2);     //ƫ��켣�����ֻ��һ��
				break;
		}
		else  //��Ŀ�����壬����ǰ��
		{
				lukou--;		 	
		}	
		if(lukou==0)
		{
			finish=1;
			break;
		}
	}
	
	if(finish==1)
	{
		loading=2;
		putdown();
		loading=0;
		finish=0;
		break;
	}
	


//step2ץ����
if(wutinumber[lukou]>=2 )
{
	if(loading == 1)
	{
		tttt=1;
		takegoods();
		loading+=1;
		wutinumber[lukou]-=1;
	}
	else if(loading == 0)
	{
		tttt=2;
		takegoods();
		loading+=2;
		wutinumber[lukou]-=2;
	}
}	
else if(wutinumber[lukou]==1)
{
		tttt=1;
		takegoods();
		loading+=1;
		wutinumber[lukou]-=1;
}
else
{}

//step3����///////////////////////////////////////////////////////
		back(2);
		zhuan(2);
//����//////////////////////////////////////	
		if(loading==2)
		{
				//go(lukou-1);//��Ϊ������Ҫ�ص���һ��·��
				gogo(lukou-1);
				putdown();
				delay_ms(500);
				loading=0;	
				weizhi=1;
				if(wutinumber[lukou]==0)//��ǰ�������
				{
						lukou--;			
				}				
		}
		else
		{
						lukou--;	
		}
		



	}	
	
	
	//�ڶ��Ź���
	lukou=0;
	back(7);
	zhuan(2);
	back(2);
//��һ�Ź���
	
while(1)//ɨ��
{		
		lukou++;
		wutinumber[lukou]=0;//·����������
		xuhao=0;
		oldxun=xun;
		USART_SendData(USART1,1);//�򴮿�1��������
		while(1)
		{
			if(flagpai==1)
			{						
				break;
			}
		}
		flagpai=0;
			//���������ĵ���Ŀ��
			for(i=oldxun+1;i<xun;i++)
			{
				for(j=0;j<=targetnumber;j++)
				{
							if(targetthing[j]==goods[i][0])
							{
									wutinumber[lukou]++;  //��ǰ·�ڵ�������
									wutinumber1[lukou]++;
									xuhao++;  //��ǰ����ʶ�𵽵��������
									wutiweizhi[lukou][xuhao]=goods[i][3]; //��Ӧλ��
							}
				}
			}	
			
		if(lukou==6) break; //�ߵ����
		
		go(1);

}
	
	zhuan(1);//ԭ�ش�ת��������
	delay_ms(500);
	zhuan(1);

	lukou=6;
	loading=0;
	xuhao=1;
	weizhi=6;


while(1)
{
	
	while(1)
	{
		xuhao=1;	
		if(lukou==0)
		{
			finish=0;
		}
			gogo(weizhi-lukou);
			weizhi=lukou;
		
		OLED_Clear();
		OLED_ShowNum(0,0,wutinumber[lukou],2,16);	
		OLED_Refresh();
		
		
		if(wutinumber[lukou]>0)//��·������Ŀ�����塢��ȥץȡ
		{
				zhuan(1);
				go(2);
				break;
		}
		else
		{
				lukou--;			
		}	
		
		if(lukou==0)
		{
			finish=0;
		}
	}
	
	if(finish==1)
	{
		back(2);
		zhuan(2);
		go(7);
		loading=2;
		putdown();
		loading=0;
		finish=0;
		break;
	}


//step2ץ����
if(wutinumber[lukou]>=2 )
{
	if(loading == 1)
	{
		tttt=1;
		takegoods();
		loading+=1;
		wutinumber[lukou]-=1;
	}
	else if(loading == 0)
	{
		tttt=2;
		takegoods();
		loading+=2;
		wutinumber[lukou]-=2;
	}
}	
else if(wutinumber[lukou]==1)
{
		tttt=1;
		takegoods();
		loading+=1;
		wutinumber[lukou]-=1;
}
else
{}

//step3����
		back(2);
		zhuan(2);
		//zhuan(2);
		if(loading==2)//���ػص����
		{
			gogo(weizhi-3);
			zhuan(2);
			go(7);
			putdown();
			delay_ms(500);
			loading=0;				
	
			if(wutinumber[lukou]==0)//��ǰ�������
			{
					lukou--;
					back(7);
					zhuan(1);
					weizhi=3;
			}		
			else
			{
					back(7);
					zhuan(1);
					weizhi=3;				
			}

		}
		else//��ǰװ��1�����壬Ҫȥ��һ��·��
		{
			lukou--;
		}
	}	
	

	
	
	
	
	
		
}
void jixiebi(u8 n)       //��е��ִ�еڼ��Ŷ�����
{
	group_done = 0;
	switch(n)
	{
		case 1:   printf("$DGT:1-6,1!");  delay_ms(1000); break;//���ϼ�ȡ
		case 2:   printf("$DGT:7-12,1!");  delay_ms(1000); break;//���ϼ�ȡ
		case 3:   printf("$DGT:13-18,1!");  delay_ms(1000); break;//���ϼ�ȡ
		case 4:   printf("$DGT:19-24,1!");  delay_ms(1000); break;//����
		case 5:   printf("$DGT:25-30,1!");  delay_ms(1000); break;//����
		case 6:   printf("$DGT:31-36,1!");  delay_ms(1000); break;//����
		case 7:   printf("$DGT:37-41,1!");  delay_ms(1000); break;//�������
		case 8:   printf("$DGT:42-47,1!");  delay_ms(1000); break;//�����м���
		case 9:   printf("$DGT:48-51,1!");  delay_ms(1000); break;//�Ż�
		default:    ;
	}
	while(1) //����
	{
		if(	group_done == 1) break;
	}
	
}



void takegoods()
{
	int c;
	int d;
	
	c=wutiweizhi[lukou][xuhao];
	d=wutiweizhi[lukou][xuhao+1];
	OLED_Clear();
	OLED_ShowString(16,0,"place1",16);OLED_ShowNum(32,0,c,2,16); 
	OLED_ShowString(16,16,"place2",16);OLED_ShowNum(32,16,d,2,16); 
	OLED_ShowString(16,32,"t",16);OLED_ShowNum(32,32,tttt,2,16); 
	OLED_ShowString(16,48,"load",16);OLED_ShowNum(32,48,loading,2,16); 
	OLED_Refresh();
	
	if(tttt == 1 )
	{
		if(loading==1)
		{
			jixiebi(c);
		}
		else
		{
			jixiebi(c);
			jixiebi(7);//��һ�μ��𣬼�һ�����¶���  		
		}
	}
	else if(tttt == 2)
	{
		jixiebi(c);
		jixiebi(7);//��һ�μ��𣬼�һ�����¶���  
		jixiebi(d);
	}
	
	if(xuhao+2 <= wutinumber1[lukou])
	{
		xuhao+=2;
	}
	else
	{
		wutinumber1[lukou]=0;
		xuhao=1;	
	}

}

void putdown(void)
{
	if(loading == 1)
	{
		jixiebi(9);
	}
	else if(loading == 2)
	{
		jixiebi(9);
		jixiebi(8);
		jixiebi(9);
	}
}

void xunjigo()
{
	
	flag=luxian[number];
	while(1)
	{
		if(flagt==0)
		{
			cemianxunji();		
		}
		else
		{
			flagt--;
		}	
		if(flag==0)//ֹͣ
		{
			Set_Pwm(0,0,0,0);
			break;
		}	
		if(flag==3)//ֱ��
		{
			track_PID();			 //ֱ�ߺ���
		}	
		if(flag==1)//��ת
		{	
			zhuanxiang(flag);	 //ת����
		}
		if(flag==2)//��ת
		{	
			zhuanxiang(flag);	 //ת����
		}
		if(flag==4)//����
		{
			backwardd();	
		}
	}
}



void zhuanxiang(int i)
{
		//700
		if(i==1)//��ת
		{			
				Set_Pwm(pwm1,pwm2,pwm3,pwm4);
				pwm1=PIDcontrolA(Encoder_A,-500,pwm1);	
				pwm2=PIDcontrolB(Encoder_B,500,pwm2);
				pwm3=PIDcontrolC(Encoder_C,-500,pwm3);
				pwm4=PIDcontrolD(Encoder_D,500,pwm4);	
				oled_show();
		}
		if(i==2)//��ת
		{			
				Set_Pwm(pwm1,pwm2,pwm3,pwm4);
				pwm1=PIDcontrolA(Encoder_A,500,pwm1);	
				pwm2=PIDcontrolB(Encoder_B,-500,pwm2);
				pwm3=PIDcontrolC(Encoder_C,500,pwm3);
				pwm4=PIDcontrolD(Encoder_D,-500,pwm4);	
				oled_show();
		}		
		

}



void track_PID()
{
	static float Integral_error,Last_error;
	u16 temp_data[2] = { 0 };       //���ݻ�����
	int error = 0;         //ƫ��ֵ
	//int L_Pwm,R_Pwm;			 //�������ٶ�
	float P=25;
	float I = 0,D = 0;		 //����ϵ����΢��ϵ��
	Read_Data(temp_data);
	if(temp_data[0]==0)
	{
		error = temp_data[1];
		Rtarget = (target+(error*P+Integral_error*I+(error-Last_error)*D));	
		Ltarget = (target-(error*P+Integral_error*I+(error-Last_error)*D));			
		if(target-Ltarget>100 )Ltarget=target-100;
		if(Rtarget-target>100 )Rtarget=target+100;		
		pwm1=PIDcontrolA(Encoder_A,Ltarget,pwm1);	
		pwm2=PIDcontrolB(Encoder_B,Rtarget,pwm2);
		pwm3=PIDcontrolC(Encoder_C,Ltarget,pwm3);
		pwm4=PIDcontrolD(Encoder_D,Rtarget,pwm4);
		if(pwm1>3600 && pwm2>3600 && pwm3>3600 && pwm4>3600 )
		{
		pwm1=3600;	
		pwm2=3600;
		pwm3=3600;
		pwm4=3600;
		}
		Set_Pwm(pwm1,pwm2,pwm3,pwm4);	
	}
	
	else
	{
		error = temp_data[1];
		
		Rtarget = (target+(error*P+Integral_error*I+(error-Last_error)*D));	
		
		Ltarget = (target-(error*P+Integral_error*I+(error-Last_error)*D));			

		if(target-Ltarget>100 )Ltarget=target-100;
		
		if(Rtarget-target>100 )Rtarget=target+100;		
		pwm1=PIDcontrolA(Encoder_A,Rtarget,pwm1);	
		pwm2=PIDcontrolB(Encoder_B,Ltarget,pwm2);
		pwm3=PIDcontrolC(Encoder_C,Rtarget,pwm3);
		pwm4=PIDcontrolD(Encoder_D,Ltarget,pwm4);
		
		if(pwm1>3600 && pwm2>3600 && pwm3>3600 && pwm4>3600 )
		{
		pwm1=3600;	
		pwm2=3600;
		pwm3=3600;
		pwm4=3600;
		}	
		Set_Pwm(pwm1,pwm2,pwm3,pwm4);		
		
	}

		Last_error = error;

}






void cemianxunji()//����8·ѭ���ж�
{
	u16 Temp[2] = { 0 };       //���ݻ����� 
	 //int flagzhuan=0;
		
	
		flagg=0;//����ѭ�����صı�־λ
		Read_Data1(Temp);
		
		switch(Temp[0])
		{                                         //8765 4321
		//case 0xFE:		zhuanwanchoose();	break;      //1111 1110
		//case 0xFC:		zhuanwanchoose();	break;      //1111 1100
		//case 0xFD:		zhuanwanchoose();	break;      //1111 1101
		//case 0xF9:		zhuanwanchoose();	break;      //1111 1001
		//case 0xFB:		zhuanwanchoose();	break;      //1111 1011
		//case 0xF3:		zhuanwanchoose();	break;      //1111 0011
		//case 0xF7:		zhuanwanchoose();	break;      //1111 0111
		//case 0xE7:		zhuanwanchoose();	break;      //1110 0111		//���м�λ��
		//case 0xEF:		zhuanwanchoose();	break;      //1110 1111			
		//case 0xCF:		zhuanwanchoose();	break;      //1100 1111
		//case 0xDF:		zhuanwanchoose();	break;      //1101 1111
		case 0x9F:		flagg=1;	break;      //1001 1111
		case 0xBF:		flagg=1;	break;      //1011 1111
		case 0x3F:		flagg=1;	break;      //0011 1111
		case 0x7F:		flagg=1;	break;      //0111 1111		
		default :	flagg=0;		;	break;
		}
		
	if(flagzhuan==1) 
	{
		if((digtal(1)==0 || digtal(2)==0 )&& (D4==1 || D5==1 || D6==1 || D3==1))
		{
				flagzhuan=0;
				flag=0;			
				number++;
				flagt=15;
		}
	}	
	else
	{
		if((luxian[number]==1 || luxian[number]==2 )&& flagg==1)
		{
		flagzhuan=1;
		flag=luxian[number];
		flagt=15;
			
		}
		else if(luxian[number]==3 && flagg==1)
		{
		number++;
		flagt=250;
		flag=3;
		}
		else if(luxian[number]==0 && flagg==1)
		{
			flagt=0;
			flag=0;
			//number++;
		}
		else if(luxian[number]==4 && flagg==1)
		{
		number++;
		flagt=50;
		flag=4;		
		}
		else 
		{
		
		}
		
	}
}


void zhuan(int i)
{
		flagzhuan=1;
		luxian[0]=i;
		luxian[1]=0;
		luxian[2]=0;
		//luxian[3]=0;
		number=0;
		flagt=15;
		xunjigo();
	
}


void gofirst(void)
{
	int flaggg=0;
	int time;
	int j;
u16 Temp[2] = { 0 };       //���ݻ����� 
	 //int flagzhuan=0;		
		flaggg=0;//����ѭ�����صı�־λ
		while(1)
		{	
				track_PID();
	
				
					Read_Data1(Temp);
					switch(Temp[0])
					{                                         //8765 4321

					case 0x9F:		flaggg=1;	break;      //1001 1111
					case 0xBF:		flaggg=1;	break;      //1011 1111
					case 0x3F:		flaggg=1;	break;      //0011 1111
					case 0x7F:		flaggg=1;	break;      //0111 1111		
					default :	flaggg=0;		;	break;
					}			

					if(flaggg==1)
					{
						break;
					}	
			}		
				
			Set_Pwm(0,0,0,0);
			
		

}

void go(int i)
{
	int flaggg=0;
	int time;
	int j;
u16 Temp[2] = { 0 };       //���ݻ����� 
	 //int flagzhuan=0;		
		flaggg=0;//����ѭ�����صı�־λ
	

	if(i>5)
	{
	while(i--)
	{	
			if(i>=1)
			{
				target=1000;
			}
			else
			{
				target=500;
			}


		time=50;
		while(time--)
		{	
					track_PID();
					delay_ms(1);	
		}	
		while(1)
		{	
				track_PID();
	
				
					Read_Data1(Temp);
					switch(Temp[0])
					{                                         //8765 4321

					case 0x9F:		flaggg=1;	break;      //1001 1111
					case 0xBF:		flaggg=1;	break;      //1011 1111
					case 0x3F:		flaggg=1;	break;      //0011 1111
					case 0x7F:		flaggg=1;	break;      //0111 1111		
					default :	flaggg=0;		;	break;
					}			

					if(flaggg==1)
					{
						break;
					}	
			}		
		}		
	
	}
	else
	{
	while(i--)
	{	
target=500;
		time=50;
		while(time--)
		{	
					track_PID();
					delay_ms(1);	
		}	
		while(1)
		{	
				track_PID();
	
				
					Read_Data1(Temp);
					switch(Temp[0])
					{                                         //8765 4321

					case 0x9F:		flaggg=1;	break;      //1001 1111
					case 0xBF:		flaggg=1;	break;      //1011 1111
					case 0x3F:		flaggg=1;	break;      //0011 1111
					case 0x7F:		flaggg=1;	break;      //0111 1111		
					default :	flaggg=0;		;	break;
					}			

					if(flaggg==1)
					{
						break;
					}	
			}		
		}			
	}
	
			Set_Pwm(0,0,0,0);
			
	
}

void back(int i)
{
	int flaggg=0;
	int time=400;
	int j;
u16 Temp[2] = { 0 };       //���ݻ����� 
	 //int flagzhuan=0;		
		flaggg=0;//����ѭ�����صı�־λ
if(i>4)
{
	while(i--)
	{
			if(i>=1)
			{
				target1=2000;
			}
			else
			{
				target1=1000;
			}		
			
		time=400;
		while(time--)
		{	
					backwardd();
					delay_ms(1);	
		}	
		while(1)
		{	
				backwardd();
	
				
					Read_Data1(Temp);
					switch(Temp[0])
					{                                         //8765 4321

					case 0x9F:		flaggg=1;	break;      //1001 1111
					case 0xBF:		flaggg=1;	break;      //1011 1111
					case 0x3F:		flaggg=1;	break;      //0011 1111
					case 0x7F:		flaggg=1;	break;      //0111 1111		
					default :	flaggg=0;		;	break;
					}			

					if(flaggg==1)
					{
						break;
					}	
			}		
		}		
			Set_Pwm(0,0,0,0);
			
}
else
{
	while(i--)
	{		
		target1=1000;
		time=400;
		while(time--)
		{	
					backwardd();
					delay_ms(1);	
		}	
		while(1)
		{	
				backwardd();
	
				
					Read_Data1(Temp);
					switch(Temp[0])
					{                                         //8765 4321

					case 0x9F:		flaggg=1;	break;      //1001 1111
					case 0xBF:		flaggg=1;	break;      //1011 1111
					case 0x3F:		flaggg=1;	break;      //0011 1111
					case 0x7F:		flaggg=1;	break;      //0111 1111		
					default :	flaggg=0;		;	break;
					}			

					if(flaggg==1)
					{
						break;
					}	
			}		
		}		
			Set_Pwm(0,0,0,0);

}
	
}

void gogo(int i)
{
	if(i>0)
	{
		go(i);
	}
	else if(i==0)
	{
		
	}
	else
	{
		back(-i);
	}
}
