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
机械臂函数action调用串口三，重定向printf.
#############################################*/
//串口1用到变量
u8 i;
u16 t;  
u16 len;//传回串口1数据
u32 time_s[200];//存储串口1传回数据
u8 uart_content[12];			//用于显示传回数据
void take_phone(int flag);
u32 goods[100][4];//用于存储时时传回来的数据
int flagpai=0;//拍照结束标志位		
///////////以上为图像识别部分

//goods【1】【x】代表物体的种类和个数
//goods【2】【x】代表物体的x坐标
//goods【3】【x】代表物体的y坐标
//"Red_Block", "Rubik_Cube", "Bule_Block", "Snowflake_Beer", "Red_Bull", "Tennis", "AD_Calcium_Milk", "Deluxe_Milk"
//1：红色方块；2：魔方；3：蓝色方块；4：雪花啤酒；5：红牛；6：网球；7：AD钙；8：特仑苏
int targetnumber=0;//记录识别物体的种类个数
int targetthing[10];//记录识别物体的种类

extern int xun;

extern int group_done;

long int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //编码器的脉冲计数
long int Motor_A,Motor_B,Motor_C,Motor_D;        //电机PWM变量
long int Target_A,Target_B,Target_C,Target_D;     //电机目标值
float Position_KP=6,Position_KI=0,Position_KD=3;  //位置控制PID参数
float Velocity_KP=10,Velocity_KI=10;	          //速度控制PID参数
int flagzhuan=0;
int flagg=0;
void track_PID(void);
void backwardd(void);
void cemianxunjihuizhuan(void);//侧面8路循迹判断
void cemianxunji(void);//侧面8路循迹判断
void zhuanwanchoose(void);//转弯选择
int number=0;//记录第几个十字路口
int flag=0;//线路标志
void zhuanxiang(int i);//正在转弯动作
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
//int remenant=0;//上次剩余的物品
//int flagremenant=0;
//int g1=1;
//int flagremenant2=0;
int flagt=100;//转弯时屏蔽2号循迹传感器的短延时

///////////以上是循迹部分


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
	JTAG_Set(JTAG_SWD_DISABLE);     //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //=====打开SWD接口 可以利用主板的SWD接口调试
	delay_init();	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 设置中断优先级分组2
	uart_init(115200);
	uart2_init(9600);
	uart3_init(9600);
	Usart4_Init();
	KEY_Init();
	OLED_Init();
	OLED_ColorTurn(0);                          //0正常显示，1 反色显示
	OLED_DisplayTurn(0);                        //0正常显示 1 屏幕翻转显示
	OLED_Clear();
	Encoder_Init_TIM2();
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
	Encoder_Init_TIM5();
	MiniBalance_PWM_Init(7199,0);
	TIM6_Int_Init(100,7199);                //10Khz的计数频率，计数到5000为500ms  
	chuan_Init();
	chuan_Init2();
	uart3_init(115200);
	jixiebi_init();
}

void oledchoose()
{
		u8 key;
	int momentobj=0;//记录当前选择的物品序号与光标所在位置
	int flag=0;
	int i,j;
		OLED_Clear();
		OLED_ShowString(0,0,"Choose Things",16);
		OLED_ShowString(0,16,"1 2 3 4 5 6 7 8",16);	
		OLED_ShowString(16*momentobj,32,"o",16);	
		OLED_Refresh();
		
	while(1)
	{
		
		key=KEY_Scan(0);   //扫描按键
		switch(key)
		{
			
			case KEY_DOWN: 
					flag=3;
					break;    //按下K_DOWN按键  熄灭D2指示灯
			case KEY_LEFT: 					
				momentobj++;	   //当前物品的种类
				if(momentobj==8)momentobj=0;
				flag=1;		
				break;    //按下K_DOWN按键  熄灭D2指示灯
			case KEY_RIGHT:
				flag=2;		
				break;    //按下K_DOWN按键  熄灭D2指示灯
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
		
		if(flag==3) //DOWN 第三颗
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
int tttt,tttt2,mubiaoweizhi[20];//tttt代表目标物体数量

int wutinumber[30];//每个路口的目标物体个数
int wutinumber1[30];//用于保存
int wutiweizhi[30][30];//每个路口的目标物体位置
int wutizhonglei[30][30];//每个路口的物体种类
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
	
	
	oledchoose();  //输入选择物品
	gofirst();
	delay_ms(1000);
	
while(1)//扫描识别循环1
{		
		lukou++;
		wutinumber[lukou]=0;//路口物体总数
		xuhao=0;
		
		oldxun=xun;
		USART_SendData(USART1,1);//向串口1发送数据
		while(1)
		{
			if(flagpai==1)
			{						
				break;
			}
		}
		flagpai=0;

			
			//记下所有拍到的目标
			for(i=oldxun+1;i<xun;i++)
			{
				for(j=0;j<=targetnumber;j++)
				{
							if(targetthing[j]==goods[i][0])
							{
									wutinumber[lukou]++;  //当前路口的物体数
									wutinumber1[lukou]++;
									xuhao++;  //当前框内识别到的物体序号
									wutiweizhi[lukou][xuhao]=goods[i][3]; //对应位置
							}
				}
			}	
			
		if(lukou==6) break; //走到最后
		
		go(1);

}
	
//一排检测完成///////////////////////////////////////////
	zhuan(1);//原地打转调整方向
	delay_ms(500);
	zhuan(1);

	lukou=6;
	loading=0;
	xuhao=1;
	weizhi=6;


while(1) //运动循环1
{
	 
	while(1)  //运动循环2 - 返程
	{
		xuhao=1;	

		if(lukou==0)
		{
			finish=1;
			break;
		}		
		
		gogo(weizhi-lukou);
		weizhi=lukou;
		

		
		if(wutinumber[lukou]>0)//若路口上有目标物体、则去抓取
		{
				zhuan(1);
				go(2);     //偏离轨迹会出错：只走一格
				break;
		}
		else  //无目标物体，就往前走
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
	


//step2抓物体
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

//step3返程///////////////////////////////////////////////////////
		back(2);
		zhuan(2);
//满载//////////////////////////////////////	
		if(loading==2)
		{
				//go(lukou-1);//因为满载需要回到第一个路口
				gogo(lukou-1);
				putdown();
				delay_ms(500);
				loading=0;	
				weizhi=1;
				if(wutinumber[lukou]==0)//当前货架完毕
				{
						lukou--;			
				}				
		}
		else
		{
						lukou--;	
		}
		



	}	
	
	
	//第二排柜子
	lukou=0;
	back(7);
	zhuan(2);
	back(2);
//第一排柜子
	
while(1)//扫描
{		
		lukou++;
		wutinumber[lukou]=0;//路口物体总数
		xuhao=0;
		oldxun=xun;
		USART_SendData(USART1,1);//向串口1发送数据
		while(1)
		{
			if(flagpai==1)
			{						
				break;
			}
		}
		flagpai=0;
			//记下所有拍到的目标
			for(i=oldxun+1;i<xun;i++)
			{
				for(j=0;j<=targetnumber;j++)
				{
							if(targetthing[j]==goods[i][0])
							{
									wutinumber[lukou]++;  //当前路口的物体数
									wutinumber1[lukou]++;
									xuhao++;  //当前框内识别到的物体序号
									wutiweizhi[lukou][xuhao]=goods[i][3]; //对应位置
							}
				}
			}	
			
		if(lukou==6) break; //走到最后
		
		go(1);

}
	
	zhuan(1);//原地打转调整方向
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
		
		
		if(wutinumber[lukou]>0)//若路口上有目标物体、则去抓取
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


//step2抓物体
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

//step3返程
		back(2);
		zhuan(2);
		//zhuan(2);
		if(loading==2)//满载回到起点
		{
			gogo(weizhi-3);
			zhuan(2);
			go(7);
			putdown();
			delay_ms(500);
			loading=0;				
	
			if(wutinumber[lukou]==0)//当前货架完毕
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
		else//当前装载1个物体，要去下一个路口
		{
			lukou--;
		}
	}	
	

	
	
	
	
	
		
}
void jixiebi(u8 n)       //机械臂执行第几号动作组
{
	group_done = 0;
	switch(n)
	{
		case 1:   printf("$DGT:1-6,1!");  delay_ms(1000); break;//左上夹取
		case 2:   printf("$DGT:7-12,1!");  delay_ms(1000); break;//中上夹取
		case 3:   printf("$DGT:13-18,1!");  delay_ms(1000); break;//右上夹取
		case 4:   printf("$DGT:19-24,1!");  delay_ms(1000); break;//左下
		case 5:   printf("$DGT:25-30,1!");  delay_ms(1000); break;//中下
		case 6:   printf("$DGT:31-36,1!");  delay_ms(1000); break;//右下
		case 7:   printf("$DGT:37-41,1!");  delay_ms(1000); break;//放入盒子
		case 8:   printf("$DGT:42-47,1!");  delay_ms(1000); break;//盒子中夹起
		case 9:   printf("$DGT:48-51,1!");  delay_ms(1000); break;//放回
		default:    ;
	}
	while(1) //反馈
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
			jixiebi(7);//第一次夹起，加一个放下动作  		
		}
	}
	else if(tttt == 2)
	{
		jixiebi(c);
		jixiebi(7);//第一次夹起，加一个放下动作  
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
		if(flag==0)//停止
		{
			Set_Pwm(0,0,0,0);
			break;
		}	
		if(flag==3)//直走
		{
			track_PID();			 //直走函数
		}	
		if(flag==1)//左转
		{	
			zhuanxiang(flag);	 //转向函数
		}
		if(flag==2)//右转
		{	
			zhuanxiang(flag);	 //转向函数
		}
		if(flag==4)//后退
		{
			backwardd();	
		}
	}
}



void zhuanxiang(int i)
{
		//700
		if(i==1)//左转
		{			
				Set_Pwm(pwm1,pwm2,pwm3,pwm4);
				pwm1=PIDcontrolA(Encoder_A,-500,pwm1);	
				pwm2=PIDcontrolB(Encoder_B,500,pwm2);
				pwm3=PIDcontrolC(Encoder_C,-500,pwm3);
				pwm4=PIDcontrolD(Encoder_D,500,pwm4);	
				oled_show();
		}
		if(i==2)//右转
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
	u16 temp_data[2] = { 0 };       //数据缓存区
	int error = 0;         //偏差值
	//int L_Pwm,R_Pwm;			 //左右轮速度
	float P=25;
	float I = 0,D = 0;		 //积分系数，微分系数
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






void cemianxunji()//侧面8路循迹判断
{
	u16 Temp[2] = { 0 };       //数据缓存区 
	 //int flagzhuan=0;
		
	
		flagg=0;//侧面循迹返回的标志位
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
		//case 0xE7:		zhuanwanchoose();	break;      //1110 0111		//正中间位置
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
u16 Temp[2] = { 0 };       //数据缓存区 
	 //int flagzhuan=0;		
		flaggg=0;//侧面循迹返回的标志位
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
u16 Temp[2] = { 0 };       //数据缓存区 
	 //int flagzhuan=0;		
		flaggg=0;//侧面循迹返回的标志位
	

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
u16 Temp[2] = { 0 };       //数据缓存区 
	 //int flagzhuan=0;		
		flaggg=0;//侧面循迹返回的标志位
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
