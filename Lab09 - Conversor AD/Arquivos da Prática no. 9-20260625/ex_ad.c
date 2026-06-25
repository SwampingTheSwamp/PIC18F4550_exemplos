// Ex. módulo A/D - Voltímetro 0 a 5V

#include <xc.h>
#include <stdlib.h>
#include "nxlcd.h"

#pragma config	FOSC = HS			// Fosc = 20MHz, Tcy = 200ns 
#pragma config	CPUDIV = OSC1_PLL2  // Fosc sem divisão extra (OSC1)
#pragma config	PBADEN = OFF 		// Pinos do PORTB começam como digitais
#pragma config	WDT = OFF 			// Watchdog desativado
#pragma config	LVP = OFF 			// Desabilita gravação em baixa tensão

#define _XTAL_FREQ 20000000

void main() {
	unsigned int q;
  	long v;
    char ADC[5];
 	char tensao[5];

  	// Configurações ADC
  	ADCON0=0b00000001; 	//seleciona CH0 e habilita AD
  	ADCON1=0b00001110; 	// Vref em VSS e VDD, seleciona AN0
  	ADCON2=0b10111101; 	// 10 bits, 20TAD, FOSC/16
  	TRISD = 0x00;
	TRISE = 0x00;         
	TRISA = 0b00000001;
 	OpenXLCD( FOUR_BIT & LINES_5X7 );
  	WriteCmdXLCD(0x01);				
  	__delay_ms(16);
	while (1) {
		ADCON0bits.GO_DONE=1;      		// Realiza a aquisição
       	while(ADCON0bits.GO_DONE); 		// Espera a conversão
       	q =(ADRESH*256)+ADRESL; 		// Lê o canal selecionado
		v = (5000*(long)q)/1023;
	   	ltoa(tensao,v,10);

	   	// Imprime valor lido do ADC (0 a 1023)
	   	WriteCmdXLCD(0x80);
	   	itoa(ADC,q,10);
	   	putsXLCD(ADC);
	   	putrsXLCD(" ADC");

       	// Imprime valor de Tensão (0 a 5)	   
	   	WriteCmdXLCD(0xC0);
       	putcXLCD(tensao[0]);
		putcXLCD(',');
       	putcXLCD(tensao[1]);
       	putcXLCD(tensao[2]);
       	putcXLCD(tensao[3]);
		putrsXLCD(" V");
       	WriteCmdXLCD(0x0C);
       	__delay_ms(35);
	}
}

