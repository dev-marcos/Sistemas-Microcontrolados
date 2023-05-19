// exemplo 5: controla servo com PWM no PA0 (Canal 1 do Timer 2)
#include "stm32f1xx.h" // Biblioteca STM
#include <math.h>
int angulo;

void TIM2_IRQHandler(void){ /* Trata da IRQ do TIM2 */
    TIM2->SR &= ~TIM_SR_UIF;    // Apaga flag sinalizadora da IRQ
    GPIOC->ODR ^= (1<<13);      // LED toggle (troca estado do LED onboard) 
    if((angulo += 1) > 180){
        angulo = -180;
    }
    TIM2->CCR1 = ((angulo*100/180) + 150)*(TIM2->ARR)/2000; // Valor da largura do pulso      
}

int main(void){ 
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN; // Habilita clock do barramento APB2

    GPIOC->CRH |= GPIO_CRH_MODE13_1;    // Configura pino PC13 como saida open-drain 2 MHz
    GPIOA->CRL = (GPIOA->CRL & 0xFFFFFFF0) | 0x0000000B; // 0b1011 PA0 como saída push-pull de função alternativa (para TIM2) e alta velocidade// 
    
    /* Config. TIM2 com PWM no canal 1 (PA0) */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Habilita clock do TIM2 do bus APB1
    TIM2->ARR = 999;    // Registrador de auto-carregamento
    TIM2->PSC = 159;    // Prescaler dividindo por 159+1 (20ms => 50 Hz)
    TIM2->CNT = 0;      // Valor inicial da contagem
    angulo = -90;       // ângulo inicial
    TIM2->CCR1 = (angulo*10/180 + 15)*(TIM2->ARR)/200; // Valor da largura do pulso 20 ms
    TIM2->CCMR1 |= (0b110 << TIM_CCMR1_OC1M_Pos);   // canal 1 em modo PWM
    TIM2->CCER |= TIM_CCER_CC1E;    // Hab. pino do canal 1 (PA0) como saída de comparação
    //TIM2->CCER |= TIM_CCER_CC1P;    // Inverte polaridade do sinal de saída
    TIM2->CR1 |= TIM_CR1_CEN;       // Hab. contagem
    TIM2->DIER |= TIM_DIER_UIE;     // Hab. interrupção por estouro (update)
    //NVIC->IP[TIM2_IRQn] = 35;       // Config. prioridade do TIM2
    NVIC->ISER[0] = (uint32_t)(1 << TIM2_IRQn); // Hab. IRQ do TIM2 na NVIC

    //__enable_irq(); // Hab. globalmente as IRQs (PRIMASK), no reset já vem habilitado
    //__disable_irq(); 
    for(;;); /* Loop infinito */
}