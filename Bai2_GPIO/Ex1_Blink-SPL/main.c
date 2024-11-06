#include "stm32f10x.h"                  // Device header
						
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC					
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void delay(__IO uint32_t timedelay){
	for (int i = 0; i< timedelay;i++){}
}


// Cap xung clock
void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
}

// Cau hinh ngoai vi
void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct; // Kieu dl - ten 
	
	// Truy cap tung thanh vien trong struct
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	// Luu cai dat vao thanh ghi, 2 tham so: ngoai vi, dia chi Struct
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(){
	RCC_Config();
	GPIO_Config();
	while(1){
		GPIO_SetBits(GPIOA, GPIO_Pin_0); // Ghi 1 ra PA0
		delay(10000000);
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);// Ghi 0 ra PA0
		delay(10000000);
	}
}

	