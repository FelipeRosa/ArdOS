
#include "../config.h"
#include "../hardware/context_switch.h"
#include "../hardware/critical.h"

#include "process_management.h"
#include "scheduling.h"

#include <avr/io.h>
#include <avr/interrupt.h>


/* Macros do TIMER1 */
#define START_TIMER1()                   \
    TCCR1A = 0;                          \
    TCCR1B = (1 << WGM12) | (1 << CS10); \
    OCR1A = 15000;                       \
    TIMSK1 = 1 << OCIE1A
    
#define STOP_TIMER1() \
    TCCR1B = 0
    

/* Filas de escalonamento (buffer circular)
 * (fila de pronto e fila de espera) 
 */
static struct scheduling_queue_t
{
    uint8_t head;                               /* Início da pilha */
    uint8_t tail;                               /* Final da pilha */
    uint8_t size;                               /* Tamanho da pilha, i.e número de elementos */
    pid_t entries[ARDOS_CONFIG_MAX_PROCESSES];  /* Entradas da fila */
} ready_queue, wait_queue;

static void sched_queue_init(struct scheduling_queue_t *queue)
{
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
}

static void sched_queue_enqueue(struct scheduling_queue_t *queue, pid_t pid)
{
    if (queue->size < ARDOS_CONFIG_MAX_PROCESSES)
    {
        queue->entries[queue->tail++] = pid;
        
        if (queue->tail >= ARDOS_CONFIG_MAX_PROCESSES)
        {
            queue->tail = 0;
        }
        
        queue->size++;
    }
}

static pid_t sched_queue_dequeue(struct scheduling_queue_t *queue)
{
    if (queue->size > 0)
    {
        pid_t pid = queue->entries[queue->head++];
        
        if (queue->head >= ARDOS_CONFIG_MAX_PROCESSES)
        {
            queue->head = 0;
        }
        
        queue->size--;
        return pid;
    }
    
    return -1;
}


/* Implementação da interface */
static pid_t sched_epid = -1;    /* PID do processo em execução */


void ardos_kernel_scheduling_init()
{
    sched_queue_init(&ready_queue);
    sched_queue_init(&wait_queue);

    /* Inicializa o TIMER1 */
    START_TIMER1();
}

void ardos_kernel_schedule(pid_t pid)
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Coloca o processo na fila de pronto */
    ardos_kernel_set_process_state(pid, ARDOS_PROCESS_STATE_READY);
    sched_queue_enqueue(&ready_queue, pid);
}

void ardos_kernel_reschedule(pid_t pid)
{
    ardos_kernel_schedule(pid);
    ardos_kernel_update_waitqueue();
}

bool_t ardos_kernel_put_onwait(pid_t pid, const struct wait_event_t *we)
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Só é possível colocar o processo em
     * espera, se ele estiver em estado de PRONTO */
    if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_READY)
    {
        /* Configura o evento de espera */
        ardos_kernel_set_process_waitevent(pid, we);
        /* Coloca o processo em estado de espera e o
         * adiciona à fila de espera */
        ardos_kernel_set_process_state(pid, ARDOS_PROCESS_STATE_WAIT);
        sched_queue_enqueue(&wait_queue, pid);
        /* Atualiza a fila de pronto (retira o processo de lá) */
        ardos_kernel_update_readyqueue();
        
        return TRUE;
    }

    return FALSE;
}

void ardos_kernel_update_readyqueue()
{
    uint8_t i;
    uint8_t s = ready_queue.size;
    
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    for (i = 0; i < s; i++)
    {
        pid_t pid = sched_queue_dequeue(&ready_queue);
        
        if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_READY)
        {
            sched_queue_enqueue(&ready_queue, pid);
        }
    }
}

void ardos_kernel_update_waitqueue()
{
    uint8_t i;
    uint8_t s = wait_queue.size;
 
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    for (i = 0; i < s; i++)
    {
        pid_t pid = sched_queue_dequeue(&wait_queue);
        
        if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_WAIT)
        {
            sched_queue_enqueue(&wait_queue, pid);
        }
    }
}

void ardos_kernel_wakeup_joined(pid_t pid)
{
    uint8_t i;

    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Procura e acorda qualquer processo que
     * esteja esperando pelo processo "pid" */
    for (i = wait_queue.size; i > 0; i--)
    {
        pid_t wakeup_pid = sched_queue_dequeue(&wait_queue);
        struct wait_event_t *we = ardos_kernel_get_process_waitevent(wakeup_pid);
        
        if (we->code == ARDOS_JOIN_WAIT_EVENT)
        {
            if (we->e_info.join.joined_pid == pid)
            {
                ardos_kernel_schedule(wakeup_pid);
                continue;
            }
        }
        
        sched_queue_enqueue(&wait_queue, wakeup_pid);
    }
}

void ardos_kernel_process_yield()
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Pára o TIMER e o seta com um valor
     * para forçar uma interrupção */
    STOP_TIMER1();
    TCNT1 = 14999;
    START_TIMER1();
    sei(); 
}


/* Interrupção */
static void run_scheduling()
{
    uint8_t i;
    time_t tick = ardos_kernel_millis();

    volatile time_t t;

    /* Já estaremos em uma seção crítica,
     * então apenas pára o TIMER1 */
    STOP_TIMER1();
    
    /* Retorna o processo que estava sendo
     * executado (se havia algum) para a fila
     * de pronto */
    if (sched_epid != -1)
    {
        if (ardos_kernel_get_process_state(sched_epid) == ARDOS_PROCESS_STATE_READY)
        {
            ardos_kernel_schedule(sched_epid);
        }
    }
    
    /* Verifica se algum processo precisa ser acordado por
     * estar esperando por um evento de TEMPO */
    for (i = wait_queue.size; i > 0; i--)
    {
        pid_t pid = sched_queue_dequeue(&wait_queue);
        struct wait_event_t *we = ardos_kernel_get_process_waitevent(pid);
        
        if (we->code == ARDOS_TIME_WAIT_EVENT)
        {
            if (tick >= we->e_info.time.wakeup_tick)
            {
                ardos_kernel_schedule(pid);
                continue;
            }
        }
        
        sched_queue_enqueue(&wait_queue, pid);
    }
    
    /* Obtém o PID do próximo processo a ser
     * executado */
    if (ready_queue.size > 0)
    {   
        sched_epid = sched_queue_dequeue(&ready_queue);
    }
    else
    { 
        /* Coloca o processo IDLE */
        sched_epid = ARDOS_CONFIG_MAX_PROCESSES;
    }
       
    /* Reinicia o TIMER1 */
    START_TIMER1();
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
    /* Armazena o SP temporariamente */
    static volatile uint16_t sp_tmp;
 
    /* Salva o contexto de hardware do processo */
    ARDOS_SAVE_HWCONTEXT();
    /* Salva o SP para podermos usar a pilha do Kernel
     * para chamar funções */
    sp_tmp = SP;
    /* Muda para a pilha do Kernel */
    SP = ARDOS_CONFIG_KERNEL_STACK_TOP;
    /* Salva o ponteiro da pilha do processo */
    ardos_kernel_set_process_stack(sched_epid, (void *)sp_tmp);
    /* Executa o escalonament */
    run_scheduling();
    /* Restaura o contexto de hardware do processo
     * a ser executado a seguir */
    SP = (uint16_t)ardos_kernel_get_process_stack(sched_epid);
    ARDOS_RESTORE_HWCONTEXT();
    reti();
}
