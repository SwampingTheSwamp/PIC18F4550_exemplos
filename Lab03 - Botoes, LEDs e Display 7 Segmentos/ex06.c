//Exemplo 6: Contador de 4 bits com Display de 7 segmentos em XC8
// com intervalo de 1 segundo (PICSimLab)
#include<xc.h>

// Configuracoes
#pragma config	FOSC = HS				// Fosc = 20MHz, Tcy = 200ns (PLL não habilitado)
#pragma config	CPUDIV = OSC1_PLL2		// sinal de clk do sistema é o oscliador primário
#pragma config	PBADEN = OFF			// Pinos do PORTB começam como digitais
#pragma config	WDT = OFF				// Watchdog desativado
#pragma config	LVP = OFF				// Desabilita gravação em baixa tensão

#define _XTAL_FREQ 20000000             // define o clock para 20 MHz (fosc)

// Definicoes de hardware
#define BT1	PORTBbits.RB0		// Botao BT1

void main(void)
{
   unsigned char cont = 0, atraso = 0;
   // Vetor com os valores em binario p/ os segmentos do display
	unsigned char const disp7seg[] = {
					  0b00111111,    //0
                      0b00000110,    //1
                      0b01011011,    //2
                      0b01001111,    //3
                      0b01100110,    //4
                      0b01101101,    //5
                      0b01111101,    //6
                      0b00000111,    //7
                      0b01111111,    //8
                      0b01100111,    //9
					  0b01110111,    //10
					  0b01111100,    //11
					  0b00111001,    //12
					  0b01011110,    //13
					  0b01111001,    //14
					  0b01110001};   //15

   TRISB=0b00000001;				// PORTB,0 como entrada
   TRISD=0;                         // Toda a PORTD como saida
   TRISA=0;                         // Toda a PORTA como saida
   ADCON1 = 0x0F;                   // Pinos analógicos desligados
   INTCON2bits.nRBPU = 0;           // Habilita Pull-ups do PORTB
   LATD = 0x00;		
   while(1){
		if(BT1 == 0){
			LATAbits.LATA5 = 1;	// Aciona display
			for (cont = 0;cont<16;cont++){
				LATD = disp7seg[cont];
                while (atraso < 100){
                    __delay_ms(10);
                    atraso += 1;
				}
                atraso = 0;    
			}
   		}
		else
			LATAbits.LATA5 = 0;	// Desliga display			
   }
}
