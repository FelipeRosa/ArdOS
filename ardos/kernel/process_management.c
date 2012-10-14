
#include "../config.h"
#include "../hardware/critical.h"

#include "scheduling.h"
#include "process_management.h"

#include <string.h>


/* Tabela de processos do SO */
struct process_table_entry_t
{
    bool_t used;                        /* Indica se a entrada está sendo usada ou não */
    pid_t pid;                          /* PID do processo armazenado na entrada */
    process_state_t state;              /* Estado do processo armazenado na entrada */

    time_t creation_tick;               /* Tick de criação do processo (1 tick = 1 milisegundo) */
    
    struct wait_event_t wait_event;     /* Informação sobre evento de espera (caso o processo esteja no estado de espera) */
    
    void *stack;                        /* Ponteiro da pilha do processo, também utilizado
                                         * para armazenar o contexto de hardware do processo */
};

static struct process_table_t
{
    uint8_t process_count; /* Contador de processos existentes */
    struct process_table_entry_t entries[ARDOS_CONFIG_MAX_PROCESSES + 1]; /* Entradas da tabela de processos. 
                                                                           * Uma entrada extra é reservada para
                                                                           * o processo IDLE */
} process_table;


/* PROCESSO IDLE */
void ardos_idle_process_thread() __attribute__((naked));
void ardos_idle_process_thread()
{

    for (; ; )
        sei();
}

static void configure_idle_process()
{
    uint8_t i;
    struct process_table_entry_t *idle;
    uint8_t *sp;

    /* Configura o processo IDLE */
    idle = process_table.entries + ARDOS_CONFIG_MAX_PROCESSES;
    idle->used = TRUE;
    idle->pid = ARDOS_CONFIG_MAX_PROCESSES;
    idle->state = ARDOS_PROCESS_STATE_DEAD;
    idle->creation_tick = 0;
    
    /* Pilha do processo IDLE */
    sp = (uint8_t *)(ARDOS_CONFIG_KERNEL_STACK_TOP - ARDOS_CONFIG_KERNEL_STACK_SIZE + ARDOS_CONFIG_HWCONTEXT_SIZE);
    idle->stack = (void *)(sp - ARDOS_CONFIG_HWCONTEXT_SIZE);
    
    /* Empilha o endereço de retorno */   
    *sp-- = (uint8_t)(uint16_t)ardos_idle_process_thread;
    *sp-- = (uint8_t)(((uint16_t)ardos_idle_process_thread) >> 8);
    /* Empilha o R0 */
    *sp-- = 0;
    /* Empilha o SREG */
    *sp-- = 0x80;
    /* Empilha os registradores restantes */
    for (i = 0; i < 31; i++)
    {
        *sp-- = 0;
    }
}


static void init_process_table()
{
    uint8_t i;
    
    /* Inicializa */
    for (i = 0; i < ARDOS_CONFIG_MAX_PROCESSES; i++)
    {
        process_table.entries[i].used = FALSE;
        process_table.entries[i].state = ARDOS_PROCESS_STATE_UNKNOWN;
    }

    /* Configura o processo IDLE */
    configure_idle_process();    
}

static void configure_process_table_entry(pid_t pid, void (*thread)())
{
    uint16_t thread_addr = (uint16_t)thread;
    uint8_t *sp;
    uint8_t i;

    /* Marca a entrada como usada */
    process_table.entries[pid].used = TRUE;
    /* Garante o PID correto */
    process_table.entries[pid].pid = pid;
    /* Configura o tick de criação */
    process_table.entries[pid].creation_tick = ardos_kernel_millis();
    /* Coloca o processo em um estado indefinido */
    process_table.entries[pid].state = ARDOS_PROCESS_STATE_UNKNOWN;
    /* Configura a pilha do processo para a primeira execução */
    sp = (uint8_t *)(ARDOS_CONFIG_PROCESSES_STACK_TOP - (ARDOS_CONFIG_PROCESS_STACK_SIZE + ARDOS_CONFIG_HWCONTEXT_SIZE) * pid);
    process_table.entries[pid].stack = (void *)(sp - ARDOS_CONFIG_HWCONTEXT_SIZE);
 
    /* Empilha o endereço de retorno */   
    *sp-- = (uint8_t)thread_addr;
    *sp-- = (uint8_t)(thread_addr >> 8);
    /* Empilha o R0 */
    *sp-- = 0;
    /* Empilha o SREG */
    *sp-- = 0x80;
    /* Empilha os registradores restantes */
    for (i = 0; i < 31; i++)
    {
        *sp-- = 0;
    }
}


/* Implementação da interface */
void ardos_kernel_process_management_init()
{
    /* Inicializa a tabela de processos */
    process_table.process_count = 0;
    init_process_table();    
}

uint8_t ardos_kernel_process_count()
{
    return process_table.process_count;
}

pid_t ardos_kernel_create_process(void (*thread)())
{
    pid_t pid = -1;

    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    /* Só cria se houver entrada disponível na tabela */
    if (process_table.process_count < ARDOS_CONFIG_MAX_PROCESSES)
    {
        /* Encontra uma entrada vazia na tabela de processos */
        for (pid = 0; pid < ARDOS_CONFIG_MAX_PROCESSES; pid++)
        {
            if (process_table.entries[pid].used == FALSE)
            {
                break;
            }
        }
    
        /* Configura a entrada da tabela de processos 
         * para o novo processo sendo criado */
        configure_process_table_entry(pid, thread);
        /* Começa o escalonamento */
        ardos_kernel_schedule(pid);
    
        /* Incrementa o contador de processos */
        process_table.process_count++;
    }
    
    return pid;
}

void ardos_kernel_destroy_process(pid_t pid)
{
    struct process_table_entry_t *entry = process_table.entries + pid;
    process_state_t prev_state = entry->state;

    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    /* Marca a entrada como não usada */
    entry->used = FALSE;
    /* Coloca o processo no estado de MORTO */
    entry->state = ARDOS_PROCESS_STATE_DEAD;
    /* Decrementa o contador de processos */
    process_table.process_count--;
    /* Faz a atualização necessária do escalonador */
    if (prev_state == ARDOS_PROCESS_STATE_WAIT)
    {
        ardos_kernel_update_waitqueue();
    }
    else if (prev_state == ARDOS_PROCESS_STATE_READY)
    {
        ardos_kernel_update_readyqueue();
    }
    /* Verifica se existe algum processo esperando pelo
     * termino deste que acabou de ser destruído */
    ardos_kernel_wakeup_joined(pid);
}

time_t ardos_kernel_process_millis(pid_t pid)
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Calcula */
    return (ardos_kernel_millis() - process_table.entries[pid].creation_tick);
}

void ardos_kernel_set_process_state(pid_t pid, process_state_t state)
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Configura o estado do processo */
    process_table.entries[pid].state = state;
}

process_state_t ardos_kernel_get_process_state(pid_t pid)
{
    /* Não é necessário entrar em uma seção crítica,
     * pois sizeof(process_state_t) = 1
     */
    return process_table.entries[pid].state;
}

void ardos_kernel_set_process_stack(pid_t pid, void *stack)
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Configura a stack */
    if (pid != -1 && pid != ARDOS_CONFIG_MAX_PROCESSES)
    {
        process_table.entries[pid].stack = stack;
    }
}

void *ardos_kernel_get_process_stack(pid_t pid)
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Retorna o ponteiro */
    return process_table.entries[pid].stack;
}

void ardos_kernel_set_process_waitevent(pid_t pid, const struct wait_event_t *we)
{
    struct process_table_entry_t *entry = process_table.entries + pid;

    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Copia as informações */
    memcpy((void *)(&entry->wait_event), (const void *)we, sizeof(struct wait_event_t));
}

struct wait_event_t *ardos_kernel_get_process_waitevent(pid_t pid)
{
    /* Seção crítica (ponteiro = 2 bytes) */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Retorna */
    return &process_table.entries[pid].wait_event;   
}
