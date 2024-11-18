#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

// Cap xung
void RCC_Config(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

/*Cau hinh Timer*/

/** @ClockDivision: Chia bot clock cap vao Timer2 dem cham hon, de binh thuong la 72MHz
	* DIV1: Chia 1, DIV2: Chia 2, DIV4.
	* DIV1 = 72MHz; 1s = 72 000 000 dao dong        -> Sau 1s dem len 1 lan 
	*               1dao dong = 1/72 000 000s 
	*
	***************************************************************************************
	*	@Prescaler: Cho phep sau bao nhieu dao dong dem len 1 lan.
	* Kieu uint16_t max = 65535  
	*
	* VD1: 
	*	DIV1 = 72MHz
	* - Neu 1s dem len 1 lan: Prescaler = 72000000(NOT_OK)
	* - Neu 1ms(1/1000s) dem len 1 lan  -> 1 dao dong = 1/72 0000s: Prescaler = 72000 > 65535 (NOT_OK)
	* - Neu 0.1ms(1/10 000s) dem len 1 lan: Prescaler = 7200(OK) 
	* - Neu 0.001ms = 1us(1/1000 000s) dem len 1 lan: Prescaler = 72 (OK)  
	*
	* DIV2 = 36MHz
	* - Neu 1s dem len 1 lan: Prescaler = 36000000(NOT_OK)
	* - Neu 1ms(1/1000s) dem len 1 lan: Prescaler = 36000(OK)
	* - Neu 0.001ms = 1us(1/1000 000s) dem len 1 lan: Prescaler = 36(OK)
	* 
	* => Gia tri Prescaler phai -1. Vi Timer no dem len tu 0
	*
	****************************************************************************************
	* @Period: Quy dinh sau bao nhieu lan dem Timer se dem lai tu dau  
	* - Timer dem tu 0 dem gia tri minh cai dat -> RESET
	* - Phai -1
	*
	* VD: Tao ngat Timer dinh ki sau moi 1ms
	* - 0.1ms dem len 1 lan -> Cho Period = 10-1
	*
	****************************************************************************************
	* @CounterMode: Chon che do dem 
	*
	****************************************************************************************/
	
void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct; 

	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_InitStruct.TIM_Prescaler = 7200-1;
	TIM_InitStruct.TIM_Period = 65536-1; 
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Dem tu 0 len 
	
	TIM_TimeBaseInit(TIM2, &TIM_InitStruct); // Cai dat cau hinh cho 4 tham so tren
	TIM_Cmd(TIM2, ENABLE); // Goi cmd de ngoai vi lien quan cmd hoat dong
}

/** Ham delay ms
	*
	* @SetCounter: Thay doi gia tri trong thanh ghi dem, ghi gia tri vao thanh ghi dem -> De dem len-xuong  
	*
	* @GetCouner: Theo doi gia tri dem 
	* - Khi vao ham delay TIM2 dem lai tu dau -> de kiem soat
	* - while lien tuc kiem tra gia tri TIM2
	*	- timedelay*10: Vi 0.1ms dem len 1 lan, can 10 lan moi dc 1ms 
	*		
	*/
void delay_ms(uint16_t timedelay)
{
	TIM_SetCounter(TIM2,0); 
	while(TIM_GetCounter(TIM2) < timedelay * 10){} 
}

int i=0;
int main(){
	RCC_Config();
	TIM_Config();
	while(1){
		i++;
		delay_ms(1000); // 1000 ms = 1 s
	}
}
