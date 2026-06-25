// Exemplo de geração de sinal com CCP modo compara
// Geração de onda quadrada 440Hz no RC2/CCP1

#include <xc.h>
#include "nxlcd.h"

#define _XTAL_FREQ 20000000

#pragma config CPUDIV = OSC1_PLL2
#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF


void main(void){

    TRISE = 0x00;
    TRISD = 0x00;

    // RC2 como saída CCP1
    TRISCbits.TRISC2 = 0;

    ADCON1 = 0x0F;


    // Timer1 prescaler 1:8
    T1CON = 0b10110101;

    TMR1H = 0;
    TMR1L = 0;


    // CCP usando Timer1
    T3CONbits.T3CCP1 = 0;
    T3CONbits.T3CCP2 = 0;


    // CCP1 modo comparação
    CCP1CON = 0b00001011;

    
    // Frequência inicial 440Hz
    CCPR1H = 0x02;
    CCPR1L = 0xC6;
    
    
    // Interrupção CCP1
    PIE1bits.CCP1IE = 1;   // habilita CCP1
    IPR1bits.CCP1IP = 1;   // CCP1 alta prioridade

    RCONbits.IPEN = 1;     // habilita prioridades
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;


   

    // RCONbits.IPEN = 0; // A -
    
    // Timer0 ligado B -
    // 16 bits
    // Prescaler 1:256
    T0CON = 0b10000111;
    TMR0H = 0xD9;
    TMR0L = 0xDB;


    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;

    // B -

    RCONbits.IPEN = 1;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;


    OpenXLCD(FOUR_BIT & LINES_5X7);

    WriteCmdXLCD(0x01);
    __delay_ms(16);

    WriteCmdXLCD(0x80);
    putrsXLCD("Gerador CCP1");

    WriteCmdXLCD(0xC0);
    putrsXLCD("Onda 440 Hz");


    while(1){

    }

}