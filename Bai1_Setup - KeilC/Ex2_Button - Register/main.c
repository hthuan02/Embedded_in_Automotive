/* Kiem tra trang thai Button 
* Button PA0 =1 -> Led = 0 (Tat)
* Button PA0 =0 -> Led = 1 (Sang)
* Do LED PC13 (active-low) -> Hoat dong nguoc lai voi cac GPIO khac
*/ 
#include "stm32f10x.h"                  

#define HIGH 1
#define LOW 0

void delay(__IO uint32_t timedelay) { 
    for (int i = 0; i < timedelay; i++) {}
}

void RCC_Config() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN; // Kich hoat xung clock cho GPIOA & GPIOC
}

void GPIO_Config() {
		
	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13); // Reset cac bit CNF13 va MODE13 trong thanh ghi GPIOC_CRH.
											
	GPIOC->CRH |= GPIO_CRH_MODE13_0; 	// MODE13[1:0] = 11: Output mode, max speed 50 MHz
	GPIOC->CRH |= GPIO_CRH_MODE13_1; 	
	GPIOC->CRH &= ~GPIO_CRH_CNF13_0;  // CNF13[1:0] = 00: General purpose output push-pull
	GPIOC->CRH &= ~GPIO_CRH_CNF13_1;

    // Cau hinh cho button PA1
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1); // Reset cac bit CNF1 va MODE1 cho PA1
    GPIOA->CRL |= GPIO_CRL_CNF1_1; // Input voi pull-up/pull-down (CNF1[1:0] = 10)
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0;
    GPIOA->ODR |= GPIO_ODR_ODR1; // Kich hoat pull-up tren PA1
}

// Ham ghi tin hieu dien ap
void WritePin(GPIO_TypeDef *GPIO_Port, uint8_t Pin, uint8_t state) {
    if (state == HIGH) {
        GPIO_Port->ODR |= (1 << Pin);
    } else {
        GPIO_Port->ODR &= ~(1 << Pin);
    }
}

int main() {
    RCC_Config();
    GPIO_Config();

    while(1){
        if ((GPIOA->IDR & (1 << 1)) == 0) {  // Kiem tra PA1 (nut nhan) xem co nhan khong
            WritePin(GPIOC, 13, LOW);         // Neu PA1 duoc nhan, tat LED PA0
        } else {
            WritePin(GPIOC, 13, HIGH);        // Neu khong nhan PA1, bat LED PA0
        }
    }
}