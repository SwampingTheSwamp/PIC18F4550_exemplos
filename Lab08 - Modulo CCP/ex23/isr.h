#include <xc.h>

// Rotina de Serviço de Interrupção (ISR) de alta prioridade
void interrupt ISR_Alta_Prioridade(void);

// Rotina de Serviço de Interrupção (ISR) de baixa prioridade
void interrupt low_priority ISR_Baixa_Prioridade(void);
