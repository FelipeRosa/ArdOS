
#ifndef __ARDOS_KERNEL_PROCESS_MANAGEMENT_H
#define __ARDOS_KERNEL_PROCESS_MANAGEMENT_H

#include "types.h"
#include "timer.h"
#include "wait_events.h"


typedef uint8_t process_state_t;

#define ARDOS_PROCESS_STATE_UNKNOWN 0
#define ARDOS_PROCESS_STATE_READY   1
#define ARDOS_PROCESS_STATE_DEAD    2
#define ARDOS_PROCESS_STATE_WAIT    3

/* Inicializa o módulo gerenciamento de processos */
extern void ardos_kernel_process_management_init();

/* Obtém o número de processos existentes */
extern uint8_t ardos_kernel_process_count();

/* Cria um novo processo que executa a
 * thread passada como parâmetro.
 *
 * Retorna o PID do processo criado ou -1
 * caso não exista entrada disponível na
 * tabela de processos.
 */
extern pid_t ardos_kernel_create_process(void (*thread)());

/* Destrói um processo.
 * Isto consiste em desalocar todos os
 * recursos alocados pelo processo e
 * retirá-lo da tabela de processos
 */
extern void ardos_kernel_destroy_process(pid_t pid);

/* Obtém o número de milisegundos passados
 * desde a criação do processo */
extern time_t ardos_kernel_process_millis(pid_t pid);

/* Configura o estado de um processo */
extern void ardos_kernel_set_process_state(pid_t pid, process_state_t state);

/* Obtém o estado de um processo */
extern process_state_t ardos_kernel_get_process_state(pid_t pid);

/* Configura o ponteiro da pilha de um processo */
extern void ardos_kernel_set_process_stack(pid_t pid, void *stack);

/* Obtém o ponteiro da pilha de um processo */
extern void *ardos_kernel_get_process_stack(pid_t pid);


/* Configura as informações do evento de espera de um processo */
extern void ardos_kernel_set_process_waitevent(pid_t pid, const struct wait_event_t *we);

/* Obtém as informações do evento de espera de um processo */
extern struct wait_event_t *ardos_kernel_get_process_waitevent(pid_t pid);

#endif /* __ARDOS_KERNEL_PROCESS_MANAGEMENT_H */
