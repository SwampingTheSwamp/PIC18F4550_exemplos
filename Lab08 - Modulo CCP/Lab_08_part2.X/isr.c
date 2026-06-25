#include "isr.h"


// void interrupt ISR_Alta_Prioridade(void)
void __interrupt(high_priority) ISR_Alta_Prioridade(void)
{


   extern unsigned int valor;



    // INT1 aumenta 5%

    if(INTCON3bits.INT1IF)
    {

        valor += 51;


        if(valor > 1023)
            valor = 1023;


        INTCON3bits.INT1IF = 0;

    }



    // INT2 diminui 5%

    if(INTCON3bits.INT2IF)
    {

        if(valor >= 51)
            valor -= 51;

        else
            valor = 0;


        INTCON3bits.INT2IF = 0;

    }


}

// void interrupt low_priority ISR_Baixa_Prioridade(void)
void __interrupt(low_priority) ISR_Baixa_Prioridade(void)

{

}