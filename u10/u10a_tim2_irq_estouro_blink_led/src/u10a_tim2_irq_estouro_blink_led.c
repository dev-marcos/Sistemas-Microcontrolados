// Blink na bluepill usando interrupção do Timer 2
#include "stm32f1xx.h" // Biblioteca STM

void TIM2_IRQHandler(void){  /* Tratamento da interrupção do TIM2 */
    TIM2->SR &= ~TIM_SR_UIF;    // Apaga flag sinalizadora da IRQ        
    GPIOC->ODR ^= (1<<13);      // LED toggle (troca estado do LED onboard) 
}

int main(void){ 
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Habilita clock do barramento APB2 para GPIOC
    GPIOC->CRH |= GPIO_CRH_MODE13_1;    // Configura pino PC13 como saida open-drain 2 MHz

    /* Config. TIM2 com interrupção em 1 seg = 1 Hz */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Habilita clock do TIM2 do bus APB1
    TIM2->ARR = 9999;   // Registrador de auto-carregamento
    TIM2->PSC = 799;    // Prescaler dividindo por 799+1
    TIM2->CNT = 0;      // Valor inicial da contagem
    TIM2->DIER |= TIM_DIER_UIE;         // Hab. interrupção por estouro (update)
    TIM2->CR1 |= TIM_CR1_CEN;           // Hab. contagem
    // NVIC->IP[TIM2_IRQn] = 35;        // Config. prioridade do TIM2
    NVIC->ISER[0] = (uint32_t)(1 << TIM2_IRQn); // Hab. IRQ do TIM2 na NVIC

    //__enable_irq(); // Hab. globalmente as IRQs (PRIMASK), no reset já vem habilitado
    //__disable_irq(); 
    for(;;); /* Loop infinito */
}