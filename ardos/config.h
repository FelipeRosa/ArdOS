
#ifndef __ARDOS_CONFIG_H
#define __ARDOS_CONFIG_H


/* Kernel's stack top address */
#define ARDOS_CONFIG_KERNEL_STACK_TOP 0x8FF

/* Kernel's stack size */
#define ARDOS_CONFIG_KERNEL_STACK_SIZE 0x80


/* Number of processes that can
 * be allocated by the OS */
#define ARDOS_CONFIG_MAX_PROCESSES 4

/* Processes' stack top.
 * The system starts allocating
 * the stack for processes at this address. */
#define ARDOS_CONFIG_PROCESSES_STACK_TOP (ARDOS_CONFIG_KERNEL_STACK_TOP - ARDOS_CONFIG_KERNEL_STACK_SIZE)

/* Size of the stack allocated for each
 * of the processes */
#define ARDOS_CONFIG_PROCESS_STACK_SIZE 0x100


/* Size (in bytes) of the processes' hardware context */
#define ARDOS_CONFIG_HWCONTEXT_SIZE 0x23

#endif /* __ARDOS_CONFIG_H */
