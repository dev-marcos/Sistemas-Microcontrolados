.text				// Informa ao assembler para montar este texto
.syntax unified		// Usa sintaxe nova

// Declara funções globais (para outro arquivo acessar)
.global MainAsm
.global ConfigLed
.global LigaLed
.global DesligaLed
.global Delay

// Definições
.equ	GPIOA_base,	0x40010800
.equ	GPIOA_CRL,	GPIOA_base + 0x00
.equ	GPIOA_CRH,	GPIOA_base + 0x04
.equ	GPIOA_IDR,	GPIOA_base + 0x08
.equ	GPIOA_ODR,	GPIOA_base + 0x0C
.equ	GPIOA_BSRR, GPIOA_base + 0x10
.equ	GPIOA_BRR,	GPIOA_base + 0x14
.equ	GPIOA_LCKR, GPIOA_base + 0x18
.equ	GPIOC_base,	0x40011000
.equ	GPIOC_CRL,	GPIOC_base + 0x00
.equ	GPIOC_CRH,	GPIOC_base + 0x04
.equ	GPIOC_IDR,	GPIOC_base + 0x08
.equ	GPIOC_ODR,	GPIOC_base + 0x0C
.equ	GPIOC_BSRR, GPIOC_base + 0x10
.equ	GPIOC_BRR,	GPIOC_base + 0x14
.equ	GPIOC_LCKR, GPIOC_base + 0x18
.equ	RCC_APB2ENR,	0x40021018
.equ	LED_DELAY,	0x000FFFFF // Constante 

MainAsm:
	BL	ConfigLed	// Chama subrotina
LoopPrincipal:
	BL	LigaLed
	BL	Delay
	BL	DesligaLed
	BL	Delay
	B	LoopPrincipal

ConfigLed:
	// Habilita clock do APB2 no GPIOC
    LDR r6, =RCC_APB2ENR
    MOV r0, #0x10  //  0b00010000
    STR r0, [r6]
	// PC13 como saída open-drain, com velocidade de 2 MHz (bits 19-16 para'0110')
    LDR r6, =GPIOC_CRH
    LDR r0, =0x44644444		// 0b0110 -> 0x6
	// MOVW r0, =0x4444		// estas duas linhas são equivalentes à linha anterior
	// MOVT r0, =0x4464
	STR r0, [r6]

	// PCx entrada

    // Carrega R2 e R3 com constantes de liga e desliga LED
	MOV r2, #0x0000         // Valor para ligar o LED
    MOV r3, #0x2000         // Valor para desligar o LED
    LDR r6, =GPIOC_ODR    	// Aponta para GPIOC_ODR
    STR r3, [r6]	// Testa desliga
    STR r2, [r6]	// Testa liga
    STR r3, [r6]	// Testa desliga
    BX 	lr

LigaLed:
	LDR	r0, =GPIOC_BRR		// PC13 = 0 com GPIOC_BRR
	LDR	r1, =0x00002000  // 0b0010 000000000000
	STR	r1, [r0]
	BX  lr

DesligaLed:
	LDR	r0, =GPIOC_BSRR		// PC13 = 1 com GPIOC_BSRR
	LDR	r1, =0x00002000
	STR	r1, [r0]
	BX	lr

Delay:  // Rotina que gera um atraso de aprox. 0,5 s
	LDR	r0,	=LED_DELAY
LoopDelay:
	SUBS	r0,#0x01
	BNE	LoopDelay
	BX	lr

