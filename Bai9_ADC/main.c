#include "stm32f10x.h"                  // Header 
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC

// Timer, ham delay ms
void delay_ms(uint8_t timedelay)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < timedelay * 100) {}
}

// Cap xung clock
void RCC_Config() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

// Cau hinh timer
void TIM_Config() {
    TIM_TimeBaseInitTypeDef TIM_InitStruct; 

    // Chia bot Clock, cap vao Timer2 dem cham hon
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHz/1 = 72MHz; 1s tao duoc 72000000 dao dong; 1dao dong 1/72000000s
    TIM_InitStruct.TIM_Prescaler = 7200 - 1; // 1s dem len 1 lan; (-1) vi Timer dem tu 0->7199
    TIM_InitStruct.TIM_Period = 65536 - 1; 
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Dem tu 0 len 
    
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct); // Cai dat cau hinh cho 4 tham so tren
    TIM_Cmd(TIM2, ENABLE); // Goi cmd de ngoai vi lien quan cmd hoat dong

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; // AIN: Dau vao tuong tu
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void ADC_Config() {
    // Cau hinh ADC
    ADC_InitTypeDef ADC_InitStruct;

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;

    ADC_Init(ADC1, &ADC_InitStruct);
    
    // Chon kenh ADC1, chon tan so lay mau
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

static uint16_t val;  // Change made here: declare val as static
uint16_t sum = 0;
uint16_t final = 0;

int main() {
    RCC_Config();
    TIM_Config();
    ADC_Config();
    
    while(1) {
        for(int i = 0; i < 10; i++) {
            val = ADC_GetConversionValue(ADC1);
            // delay_us(100);
            // sum += val;
        }
        // sum = sum / 10;
        delay_ms(100);
        // sum = 0;
    }
}
