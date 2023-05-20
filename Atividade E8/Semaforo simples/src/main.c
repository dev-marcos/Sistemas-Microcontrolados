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


volatile uint32_t delay_counter = 0;

volatile uint32_t pedestre = 0;

void SysTick_Handler(void) {
    
    if (delay_counter > 0) {
        delay_counter--;   
    } 
}

void delay_ms(uint32_t time_ms) {
    delay_counter = time_ms;
    while (delay_counter);
}




void EXTI0_IRQHandler(void) {  /* Tratamento da IRQ do EXTI0 */
    EXTI->IMR &= ~EXTI_IMR_IM0; // Desab. a int. do EXTI0
    EXTI->PR = 1;               // Apaga flag sinalizadora da IRQ   
    GPIOC->ODR ^= (1<<13);      // LED toggle

    pedestre = 1;
    
    //delay_counter = 50;    
   
}

void GPIO_Configuration(void) {

    // Habilita clock do barramento APB2 para periféricos de porta
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN; 
    // PC13 como saida open-drain de 2 MHz
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk)) | (0b0110 << GPIO_CRH_MODE13_Pos); 
    
    
    //0b0010 = 2 ->
    //0b1000 = 8 -> 
    GPIOA->CRL = 0x44422248; // PA0 - IN   PA2~PA4 - LED (Vermelho, Amarelo, Verde)
    GPIOA->ODR |= GPIO_ODR_ODR0;    // PA0 como entrada e com resistor de pull-up interno

    // Configura o SysTick para interrupção a cada 1ms
    //SysTick_Config(SystemCoreClock / 1000);
    /* Config. SysTick com interrupção em 1 seg = 1 Hz */
    //SystemCoreClockUpdate(); // a) Se você não sabe
    //SysTick->LOAD = SystemCoreClock;
    SysTick->LOAD = 80000; // b) Se você sabe a velocidadedo clock (default BluePill: HSI com 8MHz)
    SysTick->VAL = 0; // Limpa o valor da contagem
    SysTick->CTRL = 0b111; // Clock do processador sem dividir, Hab. IRQ e SysTick

}



int main(void){ /* Funcao principal */

    GPIO_Configuration();

    // Apaga LED Amarelo e Vermelho
    GPIOA->ODR |= (1<<2);
    GPIOA->ODR |= (1<<3);



    /* Config PAO com interrupção no EXTI0*/
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;        // Seleciona PA0 para EXTI0
    EXTI->FTSR = EXTI_FTSR_FT0;                     // Sensível na rampa de descida
    EXTI->IMR = EXTI_IMR_IM0;                       // Hab. máscara de interrup. do EXTI0
    //NVIC->IP[EXTI0_IRQn] = 14;                    // Config. prioridade do EXTI0
    NVIC->ISER[0] = (uint32_t)(1 << EXTI0_IRQn);    // Hab. IRQ do EXTI0 na NVIC
     
    __enable_irq(); // Hab. globalmente as IRQs (PRIMASK), no reset já vem habilitado
    //__disable_irq(); 
     
    for(;;){ /* Loop infinito */

        if(pedestre == 1){
            
            // Espera por 3 segundos
            delay_ms(300);

            // Apaga o LED verde (PA4)
            GPIOA->ODR ^= (1<<4);

            // Acende o LED amarelo (PA3)
            GPIOA->ODR ^= (1<<3);

            // Espera por 2 segundos
            delay_ms(200);

            // Apaga o LED amarelo (PA3)
            GPIOA->ODR ^= (1<<3);

            // Acende o LED vermelho (PA2)
            GPIOA->ODR ^= (1<<2);

            // Espera por 3 segundos
            delay_ms(300);
            // Apaga o LED vermelho (PA2)
            GPIOA->ODR ^= (1<<2);

            // Acende o LED verde (PA4)
            GPIOA->ODR ^= (1<<4);

            pedestre = 0;

            // Hab. a int. do EXTI0 a EXTI3
            EXTI->IMR |= EXTI_IMR_IM0; 
        }
    }
}