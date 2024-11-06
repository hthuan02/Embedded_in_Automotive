#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC                    
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void delay(__IO uint32_t timedelay){
	for (int i = 0; i < timedelay; i++){}
}

// Cap xung clock cho GPIOA
void RCC_Config(){
	// GPIO A-C: cung duong bus APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE); 
}

// Cau hinh ngoai vi GPIOA
void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct; // Kieu du lieu struct - ten struct 
	
	// Button PA0
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	
	// Led PC13
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int main(){
	RCC_Config();
	GPIO_Config();
	while(1){
	
	/* 1. Ham doc muc dien ap cua Pin, cau hinh Input
	 * 2. while, khi nhan nut = 0: k lam gi het, tha ra = 1: LED sang
   * 3. Doc trang thai Led PC 13
	 * 4. Dao trang thai reset-set	
   */		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){ 
		
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);
			if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)){
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			} 
			else {
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
			}
		}

	}
}
