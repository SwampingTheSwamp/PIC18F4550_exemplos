#include "isr.h"

// --------------------------------------------------------------------
// Interrupção de alta prioridade
// --------------------------------------------------------------------
void interrupt ISR_Alta_Prioridade(void) {
    PIR1bits.CCP1IF = 0;	
    LATCbits.LATC2 = !LATCbits.LATC2;
}

// --------------------------------------------------------------------
// Interrupção de baixa prioridade 
// --------------------------------------------------------------------
void interrupt low_priority ISR_Baixa_Prioridade(void) {

}
