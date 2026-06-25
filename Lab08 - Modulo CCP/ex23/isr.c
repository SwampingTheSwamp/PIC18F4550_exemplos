#include "isr.h"

// --------------------------------------------------------------------
// Interrupção de alta prioridade
// --------------------------------------------------------------------
void interrupt ISR_Alta_Prioridade(void) {
	extern unsigned int valor;
	valor = valor +100; 	// Aumenta 10% o duty cycle
    if (valor > 1000) 	
		valor = 0; 			// Condição para zerar o duty cycle
	INTCONbits.INT0IF = 0;
}

// --------------------------------------------------------------------
// Interrupção de baixa prioridade 
// --------------------------------------------------------------------
void interrupt low_priority ISR_Baixa_Prioridade(void) {

}
