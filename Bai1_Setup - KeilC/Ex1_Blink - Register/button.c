#include "stm32f10x.h" // Header thi?t b?

#define HIGH 1
#define LOW 0

void delay(__IO uint32_t timedelay) {
    for (int i = 0; i < timedelay; i++) {}
}

void RCC_Config() {
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN); // Kich hoat xung clock cho GPIOA va GPIOC
}

void GPIO_Config() {
    // Cau hinh cho LED PC13
    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13); // Reset cac bit CNF13 va MODE13
    GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1; // Output mode, toc do toi da 50 MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13; // General purpose output push-pull

    // Cau hinh cho nut bam PA0
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); // Reset trang thai CNF & MODE
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // CNF0 = 10: Input with pull-up/pull-down
    GPIOA->CRL &= ~GPIO_CRL_CNF0_0; // CNF0[0] = 0
    GPIOA->ODR |= GPIO_ODR_ODR0; // Dat chan ODR0, PA0 la input pull-up
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
    while (1) {
        if ((GPIOA->IDR & (1 << 0)) == 0) { // IDR: Doc trang thai button
            WritePin(GPIOC, 13, HIGH);
        } else {
            WritePin(GPIOC, 13, LOW);
        }
        delay(100000); // Them do tre de tranh doc qua nhanh
    }
}
