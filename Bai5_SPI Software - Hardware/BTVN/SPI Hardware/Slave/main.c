#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI

#define SPI1_NSS_Pin GPIO_Pin_4
#define SPI1_SCK_Pin GPIO_Pin_5
#define SPI1_MISO_Pin GPIO_Pin_6
#define SPI1_MOSI_Pin GPIO_Pin_7
#define SPI1_GPIO GPIOA

// Cap xung clock
void RCC_Config(){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
}

// Cau hinh Timer
void TIM_Config(){
		TIM_TimeBaseInitTypeDef TIM_InitStruct;
    
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV2; //Chia thanh clock nho hon de cap cho timer default: 72MHz (1s tao duoc 72 trieu dao dong) , DIV1: khong chia
    TIM_InitStruct.TIM_Prescaler = 36000-1; //Sau bao nhieu dao dong thi se dem len 1 lan.  1s = 72M giao dong, gia tri < 65535, neu lon hon thi doi bo chia
    //VD muon 1ms dem len 1 lan thi (10^-3)/(1/36M) = 36000 dao dong
    TIM_InitStruct.TIM_Period  = 0xFFFF;//Dem bao nhieu lan thi reset
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; //Set mode dem len tu 0
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
    
    TIM_Cmd(TIM2, ENABLE); //Cho phep timer2 hoat dong
}

// Ham delay ms
void delay_ms(uint16_t timedelay)
{
		TIM_SetCounter(TIM2,0);
		while(TIM_GetCounter(TIM2) < timedelay*2){}
}

// Cau hinh GPIO
void GPIO_Config(){
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = SPI1_NSS_Pin | SPI1_SCK_Pin | SPI1_MISO_Pin | SPI1_MOSI_Pin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI1_GPIO, &GPIO_InitStruct);
}

// Cau hinh SPI cho Slave
void SPI_Config(){
		SPI_InitTypeDef SPI_InitStruct;
	
		SPI_InitStruct.SPI_Mode = SPI_Mode_Slave; // Che do
		SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // Kieu truyen
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // He so chia khi cap xung clock
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; // (Low) Trang thai ranh SCK = 0, (High) SCK = 1
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; // (1Edge) truyen tin hieu o canh dau tien
		SPI_InitStruct.SPI_DataSize =  SPI_DataSize_8b; // Kich thuc du lieu muon truyen di, truyen 8 bit
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB; // Bit truyen dau tien
		SPI_InitStruct.SPI_CRCPolynomial = 7; // So bit Checksum de kiem tra loi, thong thuong la 7, khong dung thanh vien nay cung dc
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; // Dieu khien chan SS bang phan mem (SS chan Master chon Slave de giao tiep, ghi muc 0)
		
		// Gan Struct	 luu gia tri vao thanh ghi
		SPI_Init(SPI1, &SPI_InitStruct);
		SPI_Cmd(SPI1, ENABLE);
}

// Ham nhan Slave
uint8_t SPI_Receive1Byte(void){
		// Kiem tra duong day co dang ban. khong
		// Neu no ranh, no nhan du lieu ReceiveData
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
    uint8_t temp = (uint8_t)SPI_I2S_ReceiveData(SPI1); // Ep kieu 8bit de tra ve
	
		// FLAG_RXNE: Kiem tra nhan thanh cong chua 
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); // Chua nhan xong la reset, thuc thi while
   
		return temp; // Nhan xong roi thi tra ve
}

uint8_t DataReceive;
int main(){
		RCC_Config();
		GPIO_Config();
		TIM_Config();
		SPI_Config();
		while(1){
				while(GPIO_ReadInputDataBit(GPIOA, SPI1_NSS_Pin) == 1){}
					if(GPIO_ReadInputDataBit(GPIOA, SPI1_NSS_Pin) == 0){
						DataReceive = SPI_Receive1Byte();
					
					}

		}
}		



