//E8d : Semáforo simples para carros com botão para pedestre:
// • O semáforo permanece em verde para o carros até que,
// • Seja pressionado o botão de pedestre,
// • aguarda 3 segundos;
// • Sinal passa para amarelo por 2 s;
// • Vermelho por 3 s;
// • Verde até que o botão seja pressionado novamente.

// PA0 -> LED Vermelho
// PA2 -> LED Amarelo
// PA4 -> LED Verde
// PA7 -> push-button para o terra

#include "stm32f1xx.h" // Biblioteca padrão STM

void delay_ms(uint32_t time_ms) {
    uint32_t i;
    for(i = 0; i < time_ms; i++) {
        // Aguarda aproximadamente 1ms
        // com base na frequência do processador
        for(volatile uint32_t j = 0; j < 7200; j++);
    }
}

int main(void) {
    // Habilita o clock para os periféricos GPIOA e GPIOC
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    // Configura PA0, PA2 e PA4 como saída
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE2 | GPIO_CRL_MODE4);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE4_1;
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF2 | GPIO_CRL_CNF4);

    // Configura PA7 como entrada com pull-up
    GPIOA->CRL &= ~GPIO_CRL_MODE7;
    GPIOA->CRL |= GPIO_CRL_CNF7_1;
    GPIOA->BSRR |= GPIO_BSRR_BS7;

    while(1) {
        // Verifica se o botão foi pressionado
        if(GPIOA->IDR & GPIO_IDR_IDR7) {
            // Acende o LED verde (PA4)
            GPIOA->BSRR |= GPIO_BSRR_BS4;

            // Espera por 3 segundos
            delay_ms(3000);

            // Apaga o LED verde (PA4)
            GPIOA->BSRR |= GPIO_BSRR_BR4;

            // Acende o LED amarelo (PA2)
            GPIOA->BSRR |= GPIO_BSRR_BS2;

            // Espera por 2 segundos
            delay_ms(2000);

            // Apaga o LED amarelo (PA2)
            GPIOA->BSRR |= GPIO_BSRR_BR2;

            // Acende o LED vermelho (PA0)
            GPIOA->BSRR |= GPIO_BSRR_BS0;

            // Espera por 3 segundos
            delay_ms(3000);

            // Apaga o LED vermelho (PA0)
            GPIOA->BSRR |= GPIO_BSRR_BR0;
        }
    }
}
