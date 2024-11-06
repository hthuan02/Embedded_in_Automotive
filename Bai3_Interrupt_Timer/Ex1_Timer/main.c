#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

// Cap xung
void RCC_Config(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

// Cau hinh timer
void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct; 

	// Chia bot Clock, cap vao Timer2 dem cham hon
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHz/1 = 72MHz; 1s tao duoc 72000000 dao dong; 1dao dong 1/72000000s
	TIM_InitStruct.TIM_Prescaler = 7200-1; // 1s dem len 1 lan; (-1) vi Timer dem tu 0->7199
	TIM_InitStruct.TIM_Period = 65536-1; 
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Dem tu 0 len 
	
	TIM_TimeBaseInit(TIM2, &TIM_InitStruct); // Cai dat cau hinh cho 4 tham so tren
	TIM_Cmd(TIM2, ENABLE); // Goi cmd de ngoai vi lien quan cmd hoat dong
}

// Ham delay ms
void delay_ms(uint8_t timedelay)
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
