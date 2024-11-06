#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI

#define SPI_SCK_Pin GPIO_Pin_0
#define SPI_MISO_Pin GPIO_Pin_1
#define SPI_MOSI_Pin GPIO_Pin_2
#define SPI_CS_Pin GPIO_Pin_3
#define SPI_GPIO GPIOA
#define SPI_RCC RCC_APB2Periph_GPIOA

// Ham delay ms
void delay_ms(uint16_t timedelay)
{
		TIM_SetCounter(TIM2,0);
		while(TIM_GetCounter(TIM2) < timedelay * 10){}
}

// Cap xung clock
void RCC_Config(){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
}

// Cau hinh Timer
void TIM_Config(){
		TIM_TimeBaseInitTypeDef TIM_InitStruct;
    
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV2; //Chia thanh clock nho hon de cap cho timer default: 72MHz (1s tao duoc 72 trieu dao dong) , DIV1: khong chia
    TIM_InitStruct.TIM_Prescaler = 36000; //Sau bao nhieu dao dong thi se dem len 1 lan.  1s = 72M giao dong, gia tri < 65535, neu lon hon thi doi bo chia
    //VD muon 1ms dem len 1 lan thi (10^-3)/(1/36M) = 36000 dao dong
    TIM_InitStruct.TIM_Period  = 0xFFFF;//Dem bao nhieu lan thi reset
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; //Set mode dem len tu 0
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
    
    TIM_Cmd(TIM2, ENABLE); //Cho phep timer2 hoat dong
}

// Cau hinh GPIO cho Slave
void GPIO_Config(){
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
		GPIO_InitStruct.GPIO_Pin = SPI_MISO_Pin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
}

// Ham dat lai trang thai ban dau, khong truyen dl
void SPI_Init_Config(){
	  // Khai bao 4 chan cung duoc, hoac khai bao 1 slvae - 3 master
		GPIO_WriteBit(SPI_GPIO, SPI_MISO_Pin,Bit_RESET);
}

// Ham nhan(Slave), khai bao ham tra ve kieu uint8_t
// Ham tra ve uint8_8, thi can bien tra ve dataReceive
uint8_t SPI_Slave_Receive(void){
	uint8_t dataReceive = 0x00; // 0b0000 0000
	uint8_t temp = 0x00; 				// 0b0000 000x ; Bien chua cac bit nhan vao
	
	// Kiem tra CS = 0, thoat while
	while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin)); 
	
	// Kiem tra SCK = 1, thoat while
	while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	// Doc lai chan SCK
	for(int i = 0; i< 8; i++){
		if(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){ // Kiem tra, Neu SCK = 1
			
			// Kiem tra lien tuc SCK = 1
			while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
				temp = GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI_Pin); // Doc lien tuc MOSI(nhan) tu MISO(truyen)
			}
			// Dich bit
			dataReceive <<= 1; // Dich trai
			dataReceive |=temp;
		}
		// Kiem tra CS =1
		while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	}
	return dataReceive;
}

uint8_t DataReceive;	
	int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	SPI_Init_Config();
	
	while(1){
		DataReceive = SPI_Slave_Receive();
	}
}