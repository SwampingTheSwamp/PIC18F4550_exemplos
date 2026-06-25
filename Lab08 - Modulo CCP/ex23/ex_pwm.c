//Exemplo: Acionamento PWM 

#include <xc.h>
#include "nxlcd.h"
#include<stdlib.h>

#pragma config	FOSC = HS			
#pragma config	CPUDIV = OSC1_PLL2  
#pragma config	PBADEN = OFF 		
#pragma config	WDT = OFF 			
#pragma config	LVP = OFF 			

// Protótipos das funções
void ajusta_dc(unsigned int valor);

unsigned int valor = 0;

void main() {
	char dc_str[3];
	char dc;
	TRISB = 0x01;  			  // RB0 como entrada
   	TRISD = 0x00;			
   	TRISE = 0x00;  
 	TRISCbits.TRISC2 = 0;	  // pino do PWM como saída
	INTCON2bits.nRBPU = 0; 
 	
 	OpenXLCD( FOUR_BIT & LINES_5X7 ); 
  	WriteCmdXLCD(0x01);
  	__delay_ms(16);
  	
	// Config TIMER2 e CCP  
	T2CON   = 0b00000111;	// Timer2 on, prescaler = 16, postscaler não importa
	PR2     = 249;			// Contagem máxima (carga inicial zero), 
							// T=200n*250*16 = 800 us => f = 1250 Hz
  	CCP1CON = 0b00001100;	// Modo PWM 
							// CCP1M3:0 = 1100 => active high
							// CCP1M3:0 = 1111 => active low

	// Configuração da interrupção INT0
	RCONbits.IPEN = 0;		// desabilita prioridade
	INTCONbits.INT0IE = 1;	// habilita INT0
	INTCONbits.INT0IF = 0;	// zera flag da INT0
							// INT0 é sempre de alta prioridade
	INTCONbits.GIE = 1;		// habilita interrupções 
	
	WriteCmdXLCD(0x0C);		// Desabilita cursor do display
	for (;;) {
    	ajusta_dc(valor);		// Ajuste do DC
   		dc = valor / 10; 		// Calcula o duty-cycle em %
        ultoa(dc_str,dc,10);    // Conversão para ASCII
		WriteCmdXLCD(0x80);		// Coloca na primeira linha
		putrsXLCD("DC: ");		// Apresentação no LCD
   		putsXLCD(dc_str);
		putrsXLCD("%  ");
		__delay_ms(35);	
   }
}


// Função de ajuste do duty cycle
void ajusta_dc(unsigned int valor_dc){
	CCPR1L = (char)(valor_dc >> 2);			// Retira os dois bits menos significativos
	CCP1CONbits.DC1B0 = valor_dc%2;			// Utiliza o LSB
	valor_dc = valor_dc >> 1;				// Retira o LSB
	CCP1CONbits.DC1B1 = valor_dc%2;			// Utiliza o próximo bit
}
