// Exemplo 9c: Exibe tecla pressionada no terminal serial
// PA4 a PA7 -> pinos de 5 a 8 de varredura do teclado
// PA9 Tx da USART1

#include "stm32f1xx.h" // Biblioteca STM

#define CTE_AGUARDA 5 
signed char aguarda = 0;   // Armazena a temporização em (aguarda x 100ms), 0 -> não aguarda

void EnviaDadoUSART(char tx_dado)  {
    while (!(USART1->SR & USART_SR_TXE)); // Aguarda buffer de Tx estar vazio
    USART1->DR = tx_dado;
}

int coluna = 0;
char tecla_press = 0;
const char matriz_teclas[4][4] = {
    {   '1',  '2',  '3',  'A'},  // valor em ASCII
    {   '4',  '5',  '6',  'B'},
    {   '7',  '8',  '9',  'C'},
    {   '*',  '0',  '#',  'D'}
    // {   1,  2,  3,  0xA},  // valor puro
    // {   4,  5,  6,  0xB},
    // {   7,  8,  9,  0xC},
    // {  0xE, 0, 0xF, 0xD}
};

void EXTI0_IRQHandler (void) {  /* Linha 0 (PA0) */
    EXTI->PR = EXTI_PR_PIF1;    // Apaga flag sinalizadora de interrupção
    if(GPIOA->IDR & (1<<0)){ // PA0 = 1?
        tecla_press = matriz_teclas[0][coluna];
        EXTI->IMR &= ~EXTI_IMR_IM0; // Desab. a int. do EXTI0
        aguarda = CTE_AGUARDA; // Variável para aguardar 1s para reab. int.
    }
}
void EXTI1_IRQHandler (void) {  /* Linha 1 (PA1) */
    EXTI->PR = EXTI_PR_PIF2; 
    if(GPIOA->IDR & (1<<1)){
        tecla_press = matriz_teclas[1][coluna];
        EXTI->IMR &= ~EXTI_IMR_IM1;
        aguarda = CTE_AGUARDA;
    }
}
void EXTI2_IRQHandler (void) {  /* Linha 2 (PA2) */
    EXTI->PR = 0b0100;                                
    if(GPIOA->IDR & (1<<2)){
        tecla_press = matriz_teclas[2][coluna];
        EXTI->IMR &= ~EXTI_IMR_IM2; 
        aguarda = CTE_AGUARDA;
    }      
}
void EXTI3_IRQHandler (void) {  /* Linha 3 (PA3) */
    EXTI->PR = 0b1000;                  
    if(GPIOA->IDR & (1<<3)){                     
        tecla_press = matriz_teclas[3][coluna];
        EXTI->IMR &= ~EXTI_IMR_IM3;
        aguarda = CTE_AGUARDA;
    }     
}

void SysTick_Handler (void) {   /* Trata IRQ do SysTick (a cada 10ms) */
    if (++coluna > 3)
        coluna = 0;
    GPIOA->ODR = (GPIOA->ODR & ~(0x000000F0)) | (1 << (coluna+4));

    if(aguarda == CTE_AGUARDA){
        GPIOC->ODR ^= (1<<13);  // Toggle LED onboard
        EnviaDadoUSART(tecla_press);
        EnviaDadoUSART('\t');
    }

    if(aguarda >= 0){ // Anti bounce
        aguarda--;
        if (aguarda == 0)
            // Hab. a int. do EXTI0 a EXTI3
            EXTI->IMR |= EXTI_IMR_IM0 | EXTI_IMR_IM1 | EXTI_IMR_IM2 | EXTI_IMR_IM3;  
    }   
}

int main(void){ /* Funcao principal */
    // Habilita clock APB2 para os periféricos
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
     
    // PC13 como saida open-drain de 2 MHz
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk)) | (0b0110 << GPIO_CRH_MODE13_Pos); 
    // PA0 a PA3 como entrada com pull-down; PA5 a PA8 como saídas push-pull 2MHz  (teclado 4x4)
    GPIOA->CRL = 0x22228888;    // 0b0010 = 2; 0b1000 = 8

    /* Config. SysTick com interrupção a cada 1O ms ->  100 Hz */     
    SysTick->LOAD = 800000;      // (default BluePill: HSI com 8MHz)
	SysTick->VAL = 0;		    // Limpa o valor da contagem
    SysTick->CTRL = 0b111; 	    // Clock do processador sem dividir, Hab. IRQ e SysTick */

    /* Config. USART1 somente para Tx */
    // PA9 como saída push-pull em função alt. (Tx da USART1)
    GPIOA->CRH = (GPIOA->CRH & 0xFFFFFF0F) | 0x000000B0; // 0b1011=0xB 
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Hab. clock para USART1
    USART1->BRR  = 8000000/9600;    // baudrate = 9600 baud/s    
    USART1->CR1 |=  USART_CR1_TE;   // Hab. TX
    USART1->CR1 |= USART_CR1_UE;    // Hab USART1 
    EnviaDadoUSART('u');
    EnviaDadoUSART('0');
    EnviaDadoUSART('9');
    EnviaDadoUSART('c');

    /* Config PAO interrupção no EXTI0 */
    // Importante! "EXTICR1 = EXTICR[0]""
    // Seleciona PA0 à PA3 para EXTI0 á EXTI03
    AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA | AFIO_EXTICR1_EXTI1_PA | AFIO_EXTICR1_EXTI2_PA | AFIO_EXTICR1_EXTI3_PA;        
    EXTI->RTSR = EXTI_RTSR_RT0 | EXTI_RTSR_RT1 | EXTI_RTSR_RT2 | EXTI_RTSR_RT3; // Sensível na rampa de subida
    EXTI->IMR = EXTI_IMR_IM0 | EXTI_IMR_IM1 | EXTI_IMR_IM2 | EXTI_IMR_IM3;      // Hab. máscara de interrup. do EXTI0 à EXTI3  
    //NVIC->IP[EXTI0_IRQn] = 14;                    // Config. prioridade do EXTI0
    NVIC->ISER[0] = (1 << EXTI0_IRQn) | (1 << EXTI1_IRQn) | (1 << EXTI2_IRQn) | (1 << EXTI3_IRQn);    // Hab. IRQ do EXTI0
    
    for(;;); /* Loop infinito */
}