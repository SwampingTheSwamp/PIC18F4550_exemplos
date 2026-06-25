// Exemplo de geração de sinal com CCP modo compara

#include <xc.h>
#include "nxlcd.h"

#pragma config	CPUDIV = OSC1_PLL2		
#pragma config	FOSC = HS	
#pragma config	WDT = OFF			
#pragma config	PBADEN = OFF		
#pragma config	LVP = OFF	

void main (void){
	TRISE = 0x00;
	TRISD = 0x00;
	TRISCbits.TRISC2 = 0;
	ADCON1 = 0x0F; 

	// Configuração do timer 1 com prescaler 8
    T1CON = 0b10110101;
    TMR1H = 0;
	TMR1L = 0;
	// Seleção de Timer1
	T3CONbits.T3CCP1 = 0;
	T3CONbits.T3CCP2 = 0;

	// Configuração do CCP -> modo comparação
	CCP1CON = 0b00001011;	 // Special event trigger
	PIR1bits.CCP1IF = 0;	 // zera flag da interrupção CCP1
	PIE1bits.CCP1IE = 1;	 // habilita interrupção do CCP1
	IPR1bits.CCP1IP = 1;
	//CCPR1 = 3125 = 0x0C35
	CCPR1H = 0x0C;
	CCPR1L = 0x35;

	RCONbits.IPEN = 0;		// desabilita prioridade
	INTCONbits.PEIE = 1;	// habilita interrupções de periféricos	
	INTCONbits.GIE = 1;		// habilita interrupções global

    OpenXLCD( FOUR_BIT & LINES_5X7 ); 
  	WriteCmdXLCD(0x01);
  	__delay_ms(16);	

	WriteCmdXLCD(0x80);
	putrsXLCD("Gerador de onda");
	WriteCmdXLCD(0xC0);
	putrsXLCD("quadrada 100 Hz");
	WriteCmdXLCD(0x0C);
	for(;;){

	}
}
