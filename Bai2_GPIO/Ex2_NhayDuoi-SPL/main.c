#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC                    
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void delay(__IO uint32_t timedelay){
	for (int i = 0; i < timedelay; i++){}
}

// Ham nhay duoi 4 LED tren Port A
void chaseLed(uint8_t loop){
	uint16_t Ledval;
	for(int j = 0; j < loop; j++)
	{
		Ledval = 0x0010; //0b0 0001 0000
		for(int i = 0; i < 4; i++)
		{
			Ledval = Ledval << 1;
			GPIO_Write(GPIOA, Ledval);
			delay(10000000);
		}
	}
}

// Cap xung clock cho GPIOA
void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
}

// Cau hinh ngoai vi GPIOA
void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct; 
	
	// Dam bao 4 chan GPIO: 5,6,7,8 cung 2 tham so Speed va Mode
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(){
	RCC_Config();
	GPIO_Config();
	while(1){
		chaseLed(3);
		break;
	}
}
