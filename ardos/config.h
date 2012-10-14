
#ifndef __ARDOS_CONFIG_H
#define __ARDOS_CONFIG_H


/* Endereço do topo da pilha para funções
 * do Kernel (que utilizam variáveis locais)
 */
#define ARDOS_CONFIG_KERNEL_STACK_TOP 0x8FF

/* Tamanho da pilha para funções do Kernel */
#define ARDOS_CONFIG_KERNEL_STACK_SIZE 0x80


/* Número máximo de processos que 
 * podem ser alocados no SO
 */
#define ARDOS_CONFIG_MAX_PROCESSES 4

/* Endereço do topo da pilha utilizada
 * pelos processos
 */
#define ARDOS_CONFIG_PROCESSES_STACK_TOP (ARDOS_CONFIG_KERNEL_STACK_TOP - ARDOS_CONFIG_KERNEL_STACK_SIZE)

/* Tamanho da pilha alocada para cada processo criado */
#define ARDOS_CONFIG_PROCESS_STACK_SIZE 0x100


/* Tamanho (em bytes) do contexto de hardware */
#define ARDOS_CONFIG_HWCONTEXT_SIZE 0x23

#endif /* __ARDOS_CONFIG_H */
