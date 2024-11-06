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
	while(TIM_GetCounter(TIM2) < timedelay){}
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

// Cau hinh GPIO cho Master
void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin; // SCK, MISO, CS (ouput)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SPI_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Chan MISO(Input) -> Cau hinh chi co chuc nang doc, floating tranh sai xot
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
}

// Cap Clock , SCK tao tin hieu dong bo de dieu khien 
void Clock(){
	// Ham viet tin hieu dien ap ra(WriteBit)
	GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_SET);
	delay_ms(4);
	GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
	delay_ms(4);
}

// Ham dat lai trang thai ban dau, khong truyen dl
void SPI_Init_Config(){
	GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
	GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
	GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
}

// Ham truyen(Master)
void SPI_Master_Transmit(uint8_t u8Data){ //0b10010000
																					//0b10000000
	uint8_t u8Mask = 0x80; // Tao 1 bitmask de truyen, lay MSP(trong so ben trai-Lon nhat), LSP + all bit con lai = 0
 	uint8_t tempData; // Bit chua gia tri cac bit truyen di
    GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_RESET);
	delay_ms(1); // Delay dam bao co thoi gian de CS keo xuong muc 0
	
	for(int i =0; i< 8; i++){
		// (Data) AND (bitmask)
		tempData = u8Mask & u8Data;
			
			// Neu ca 2 gia tri la 1, thi MISO se ghi gia tri, SET 1
			// Neu ca 2 gia tri 1 0 (KTM toan tu AND), MISO van giu nguyen RESET_0
			if(tempData){
				GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_SET);
				delay_ms(1);
			}
			else{
                GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
				delay_ms(1);
			}
			
			u8Data <<= 1;  // Dich qua ben Trai 1 bit
			Clock(); // 1 bit truyen + Cap 1 clock
	}
	// Het 8 Bit du lieu, keo CS len 1
	GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
	delay_ms(1);
}

uint8_t array[] = {7, 8, 4, 2};//Data
int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	SPI_Init_Config();
	
	while(1){	
		int i;
		for(i = 0; i < 4; i++){
			SPI_Master_Transmit(array[i]);
			delay_ms(1000);
		}
	}
}