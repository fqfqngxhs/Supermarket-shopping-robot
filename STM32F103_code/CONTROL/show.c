#include "show.h"
#include "oled.h"
#include "control.h"

extern int Encoder_A,Encoder_B,Encoder_C,Encoder_D;   

void oled_show(void)
{
	
	if(Encoder_A<0)
	{
		OLED_ShowChar(5,0,'-',12);
		OLED_ShowNum(12,0,myabs(Encoder_A),16,12);
	}
	else
	{	
		OLED_ShowChar(5,0,'+',12);
		OLED_ShowNum(12,0,Encoder_A,16,12);
	}
	
	if(Encoder_B<0)
	{
		OLED_ShowChar(5,16,'-',12);
		OLED_ShowNum(12,16,myabs(Encoder_B),16,12);
	}
	else
	{	
		OLED_ShowChar(5,16,'+',12);
		OLED_ShowNum(12,16,Encoder_B,16,12);
	}
	
	if(Encoder_C<0)
	{
		OLED_ShowChar(5,32,'-',12);
		OLED_ShowNum(12,32,myabs(Encoder_C),16,12);
	}
	else
	{	
		OLED_ShowChar(5,32,'+',12);
		OLED_ShowNum(12,32,Encoder_C,16,12);
	}	
	
	if(Encoder_D<0)
	{
		OLED_ShowChar(5,48,'-',12);
		OLED_ShowNum(12,48,myabs(Encoder_D),16,12);
	}
	else
	{	
		OLED_ShowChar(5,48,'+',12);
		OLED_ShowNum(12,48,Encoder_D,16,12);
	}		
	
	
	OLED_Refresh();
}


