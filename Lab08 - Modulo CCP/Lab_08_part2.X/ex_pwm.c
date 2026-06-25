// Exemplo: Acionamento PWM CCP2
// Saída PWM no RC1
// Controle de duty cycle pela INT0 (RB0)

#include <xc.h>
#include "nxlcd.h"
#include <stdio.h>


#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config CCP2MX = ON


void ajusta_dc(unsigned int valor);


unsigned int valor = 0;



void main()
{

    char dc_str[3];
    char dc;


    // TRISB = 0x01;        // RB0 entrada INT0
    TRISB = 0x06;
    TRISD = 0x00;
    TRISE = 0x00;


    // CCP2 no RC1
    TRISCbits.TRISC1 = 0;
    LATCbits.LATC1 = 0;

    INTCON2bits.nRBPU = 0;



    OpenXLCD(FOUR_BIT & LINES_5X7);

    WriteCmdXLCD(0x01);
    __delay_ms(16);



    /*
     * Configuração Timer2
     *
     * Fpwm = Fosc / ((PR2+1)*4*TMR2prescaler)
     *
     * Fosc = 20MHz
     *
     * Prescaler = 16
     *
     * PR2 = 249
     *
     * Fpwm =
     * 20000000/(250*4*16)
     *
     * Fpwm = 1250 Hz
     */


   
    T2CON = 0b00000101;  // Timer2 ON, prescaler 1:4

    PR2 = 199;


    // CCP2 modo PWM
    // CCP2M3:CCP2M0 = 1100

    CCP2CON = 0b00001100;

    CCPR2L = 128;
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;

    // Interrupção INT0
    /*RCONbits.IPEN = 1;

    INTCONbits.INT0IE = 1;

    INTCONbits.INT0IF = 0;

    INTCONbits.GIE = 1;*/
    
    // INT1 RB1
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;


    // INT2 RB2
    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT2IF = 0;
    
    INTCON2bits.INTEDG1 = 1;  
    INTCON2bits.INTEDG2 = 1;


    // habilita interrupções
    RCONbits.IPEN = 1;

    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;



    WriteCmdXLCD(0x0C);

    

    while(1)
    {


        ajusta_dc(valor);


        dc = valor / 10;


        sprintf(dc_str,"%d",dc);


        WriteCmdXLCD(0x80);

        putrsXLCD("DC: ");

        putsXLCD(dc_str);

        putrsXLCD("%  ");



        __delay_ms(35);


    }


}



// Ajuste do Duty Cycle CCP2

void ajusta_dc(unsigned int valor_dc)
{


    // 10 bits do PWM:
    //
    // CCPR2L recebe os 8 bits superiores
    //
    // CCP2CONbits.DC2B recebe os 2 bits inferiores


    CCPR2L = (char)(valor_dc >> 2);



    CCP2CONbits.DC2B0 = valor_dc % 2;


    valor_dc = valor_dc >> 1;


    CCP2CONbits.DC2B1 = valor_dc % 2;


}