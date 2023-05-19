// Ex. 10e: TIM2 CH1 conta os pulsos gerados por uma ventoinha 
// e as extrai rotações por minuto (RPM)
// PA0 <- Entrada dos pulsos (fio amarelo)

#include "stm32f1xx.h" // Biblioteca STM

unsigned char tx_str[100]; // String para transmissão na usart 

void EnviaDado_USART(char tx_dado)  {
    while (!(USART1->SR & USART_SR_TXE)); // Aguarda reg. de dado Tx estar vazio
    USART1->DR = tx_dado;
}

void EnviaStr_USART(char *string){
    while(*string){
        EnviaDado_USART(*string);
        string++;
    }
}

void int2str(unsigned int valor){ /* Converte um valor inteiro em string */
    tx_str[4] = 0;  // fim do vetor
    tx_str[3] = 48 + valor % 10;    // 48 é o valor do caracter 0 em ASCII
    valor /= 10;                    // unidades
    tx_str[2] = 48 + valor % 10;    // dezenas
    valor = valor / 10;          
    tx_str[1] = 48 + valor % 10;    // centenas
    valor = valor / 10;          
    tx_str[0] = 48 + valor % 10;    // milhares
}

/* Tratamento da interrupção do SysTick*/
void SysTick_Handler (void) { 
unsigned int pulsos, rpm;         
    GPIOC->ODR ^= (1<<13);  // LED toggle (troca estado do LED) ;
    pulsos= TIM2->CNT;      // Carrega o valor da contagem
    TIM2->CNT = 0;          // Reinicia valor da contagem
    rpm = pulsos*60/2;
    int2str(pulsos); 
    EnviaStr_USART("\nPulsos/segundo = ");
    EnviaStr_USART(&tx_str);
    int2str(rpm); 
    EnviaStr_USART("\t RPM = ");
    EnviaStr_USART(&tx_str);
}

void Configura_SysTick(void){
    /* Config. SysTick com interrupção em 1 seg = 1 Hz */
    //SystemCoreClockUpdate();  // a) Se você não sabe
    //SysTick->LOAD = SystemCoreClock;
    SysTick->LOAD = 8000000;    // b) Se você sabe a velocidade do clock (default BluePill: HSI com 8MHz)
	SysTick->VAL = 0;		    // Limpa o valor da contagem
    SysTick->CTRL = 0b111; 	    // Clock do processador sem dividir, Hab. IRQ e SysTick

}

void Configura_USART1(void){
    // PA9 como saída push-pull em função alt. (Tx da USART1)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    GPIOA->CRH = (GPIOA->CRH & 0xFFFFFF0F) | 0x000000B0; // 0b1011=0xB
    /* Config USART1 para Tx e Rx sem IRQ */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Hab. clock para USART1
    // Define baudrate = 9600 baud/s (APB2_clock = 8 MHz)
    USART1->BRR  = 8000000/9600;          
    USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Hab. RX e TX
    USART1->CR1 |= USART_CR1_UE;                  // Hab USART1
}

int main(void){ 
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Habilita clock do barramento APB2 para GPIOC
    GPIOC->CRH |= GPIO_CRH_MODE13_1;    // Configura pino PC13 como saida open-drain 2 MHz
    
    Configura_USART1();
    EnviaStr_USART("\n\nEx. 10e: TIM2 CH1 calcula a RPM de uma ventoinha");
    EnviaStr_USART("\n(PA0-> Entrada de pulsos - fio amarelo)\n");

    /* Config. TIM2 para incremento a cada 0,1 ms */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL = (GPIOA->CRL & 0xFFFFFFF0) | 0x00000008; // 0b1000=0x8, PA0 como entrada...
    GPIOA->ODR |= GPIO_ODR_ODR0;  // com pull up 
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Habilita clock do TIM2 do bus APB1
    TIM2->ARR = 0xFFFF; // Registrador de auto-carregamento (no valor máximo)
    TIM2->PSC = 0;      // Prescaler dividindo por 1
    TIM2->CNT = 0;      // Inicia valor da contagem
    TIM2->SMCR |= TIM_SMCR_ECE |                // Habilita entrada de clock externo 
                  (0b100<< TIM_SMCR_TS_Pos) |   // TI1_ED (PA0) como fonte de clock 
                  TIM_SMCR_SMS_Msk;             // Modo de clock externo TR
    TIM2->CR1 |= TIM_CR1_CEN;           // Hab. contagem

    Configura_SysTick();

    //__enable_irq(); // Hab. globalmente as IRQs (PRIMASK), no reset já vem habilitado
    //__disable_irq(); 
    for(;;); /* Loop infinito */
}