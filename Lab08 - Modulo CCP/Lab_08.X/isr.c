#include "isr.h"

// --------------------------------------------------------------------
// Interrupção de alta prioridade
// --------------------------------------------------------------------
void __interrupt(high_priority) ISR_Alta_Prioridade(void)
{
    /*
    // A
    if(PIR1bits.CCP1IF)
    {
        PIR1bits.CCP1IF = 0;

        LATCbits.LATC2 = !LATCbits.LATC2;
    }
    // A
    */
    static unsigned char estado = 0;

    // B
    // CCP1 gera onda 
    if(PIR1bits.CCP1IF)
    {
        PIR1bits.CCP1IF = 0;

        LATCbits.LATC2 = !LATCbits.LATC2;
    }



    // Timer0 troca frequência
    if(INTCONbits.TMR0IF)
    {

        INTCONbits.TMR0IF = 0;


        // recarrega 500ms
        TMR0H = 0xD9;
        TMR0L = 0xDB;



        if(estado == 0)
        {

            // 880Hz
            CCPR1H = 0x01;
            CCPR1L = 0x63;

            estado = 1;

        }
        else
        {

            // 440Hz
            CCPR1H = 0x02;
            CCPR1L = 0xC6;

            estado = 0;

        }

    }
    // B
    

}


// --------------------------------------------------------------------
// Interrupção de baixa prioridade
// --------------------------------------------------------------------
void __interrupt(low_priority) ISR_Baixa_Prioridade(void)
{

}