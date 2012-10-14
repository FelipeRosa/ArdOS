
#ifndef __ARDOS_KERNEL_SCHEDULING_H
#define __ARDOS_KERNEL_SCHEDULING_H

#include "types.h"
#include "wait_events.h"


/* Inicializa o módulo de escalonamento */
extern void ardos_kernel_scheduling_init();

/* Diz ao escalonador para
 * começar a escalonar o processo
 * indicado
 */
extern void ardos_kernel_schedule(pid_t pid);
extern void ardos_kernel_reschedule(pid_t pid);

/* Coloca um processo para esperar por um evento */
extern bool_t ardos_kernel_put_onwait(pid_t pid, const struct wait_event_t *we);

/* Atualiza a fila de pronto.
 * (Verifica se existe algum processo que 
 *  não está mais no estado de pronto e
 *  o retira)
 */
extern void ardos_kernel_update_readyqueue();

/* Atualiza a fila de espera
 * (Verifica se existe algum processo
 *  que não está mais no estado de espera)
 */
extern void ardos_kernel_update_waitqueue();

/* Acorda qualquer processo que esteja esperando
 * por algum outro terminar 
 */
extern void ardos_kernel_wakeup_joined(pid_t pid);

/* Passa a CPU para o próximo processo */
extern void ardos_kernel_process_yield();

#endif /* __ARDOS_KERNEL_SCHEDULING_H */

