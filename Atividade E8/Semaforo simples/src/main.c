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

volatile uint32_t delay_counter = 0; // Armazena a temporização em (aguarda x 1ms)

void SysTick_Handler(void) {  /* Trata IRQ da SysTick (a cada 1OO ms) */     
    if(delay_counter>0)
        delay_counter--;
    else{
        EXTI->PR = 1; // Apaga flag sinalizadora da IRQ (necessário para o chip pirata)
        EXTI->IMR |= EXTI_IMR_IM0; // Hab. a int. do EXTI0 
    }
}

void delay_ms(uint32_t time_ms) {
    delay_counter = time_ms;
    while (delay_counter);
}

void EXTI0_IRQHandler(void) {  /* Tratamento da IRQ do EXTI0 */
    EXTI->IMR &= ~EXTI_IMR_IM0; // Desab. a int. do EXTI0
    EXTI->PR = 1;               // Apaga flag sinalizadora da IRQ
    delay_ms(500);                // Aguarda 0,5s p/ reab. IRQ       
    GPIOC->ODR ^= (1<<13);      // LED toggle
}

void GPIO_Configuration(void) {
    // Habilita o clock para os periféricos GPIOA e GPIOC
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    // Configura PA0, PA2 e PA4 como saída em modo Push-Pull
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE2 | GPIO_CRL_MODE4);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE4_1;
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF2 | GPIO_CRL_CNF4);
    GPIOA->BSRR |= GPIO_BSRR_BS0 | GPIO_BSRR_BS2 | GPIO_BSRR_BS4;


    // Configura PA7 como entrada com pull-up interno

    //GPIOA->CRL &= (~(GPIO_CRL_MODE7_Msk | GPIO_CRL_CNF7_Msk)) | (0b1000 << GPIO_CRL_MODE0_Pos);
    //GPIOA->ODR |= GPIO_ODR_ODR7; 
    // Configura PA7 como entrada com pull-up
    GPIOA->CRL &= ~GPIO_CRL_MODE7;
    GPIOA->CRL |= GPIO_CRL_CNF7_1;
    GPIOA->BSRR |= GPIO_BSRR_BS7;

    // PC13 como saida open-drain de 2 MHz
    GPIOC->CRH &= ( ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk)) | (0b0110 << GPIO_CRH_MODE13_Pos); 
    

}
int main(void){ /* Funcao principal */
    // Habilita clock do barramento APB2 para periféricos de porta
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN; 
    // PC13 como saida open-drain de 2 MHz
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk)) | (0b0110 << GPIO_CRH_MODE13_Pos); 
    GPIOA->CRL = (GPIOA->CRL & ~(GPIO_CRL_MODE7_Msk | GPIO_CRL_CNF7_Msk)) | (0b1000 << GPIO_CRL_MODE7_Pos);
    GPIOA->ODR |= GPIO_ODR_ODR7;    // PA0 como entrada e com resistor de pull-up interno

    /* Config. SysTick com interrupção a cada 1OO ms */     
    SysTick->LOAD = 800000;     // (default BluePill: HSI com 8MHz)
	SysTick->VAL = 0;		    // Limpa o valor da contagem
    SysTick->CTRL = 0b111; 	    // Clock sem dividir, Hab. IRQ e SysTick

    /* Config PAO com interrupção no EXTI0 */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;        // Seleciona PA0 para EXTI0
    EXTI->FTSR = EXTI_FTSR_FT0;                     // Sensível na rampa de descida
    EXTI->IMR = EXTI_IMR_IM0;                       // Hab. máscara de interrup. do EXTI0
    //NVIC->IP[EXTI0_IRQn] = 14;                    // Config. prioridade do EXTI0
    NVIC->ISER[0] = (uint32_t)(1 << EXTI0_IRQn);    // Hab. IRQ do EXTI0 na NVIC
    
    __enable_irq(); // Hab. globalmente as IRQs (PRIMASK), no reset já vem habilitado
    //__disable_irq(); 
    for(;;); /* Loop infinito */
}




/*
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
    }*/
