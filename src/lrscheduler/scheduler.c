#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

#define MAX_PROCESS 8

Queue high_priority_queue;
Queue low_priority_queue;

int current_time = 0;
int q;

// void clear(Process** p, int len) {
// 	for (int i = 0; i < len; ++i) {
// 		free(p[i]);
// 	}
// 	free(p);
// }

// funcion para declarar las colas de alta y baja prioridad
// con sus respectivos quantums
void declaring_queues(int quantum, int num_processes) {
    q = quantum;
    high_priority_queue = create_Q(2 * q, num_processes);
    low_priority_queue = create_Q(q, num_processes);
}

// // Si un proceso esta en estado waiting, se actualiza su tiempo de espera
// // Aquellos que terminan su tiempo de espera, pasan a estado READY
// 1. actualizar los procesos que hayan termanado su tiempo de espera de I/O de WAITING a READY
void update_process_status(Process** processes, int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i]->estado == WAITING) {
            processes[i]->available_waiting_time--;
            if (processes[i]->available_waiting_time < 1) {
                processes[i]->estado = READY;
            }
        }
    }
}


// esta variable la definimos para ir guardando
// el proceso que se encuentra en ejecucion
Process* running_process = NULL;

// // 2. Si hay un proceso en estado RUNNING, se actualiza su estado
// // Disminuye el burst time cuando tienen su turno
// // Disminuye el quantum disponible cuando tienen su turno
// // Aquellos que terminan su rafaga de cpu, pasan a estado WAITING
void update_running_process() {
    if (running_process != NULL) {
        running_process->burst_time--;
        running_process->available_quantum--;

        if (running_process->burst_time <= 0) {
            running_process->burst_number--;
            if (running_process->burst_number <= 0) {
                running_process->estado = FINISHED;
            } else {
                running_process->estado = WAITING;
            }
            running_process = NULL;
        } else if (running_process->available_quantum <= 0) {
            running_process->estado = READY;
            if (running_process->tlcpu == high_priority_queue.quantum) {
                push(&low_priority_queue, running_process);
            } else {
                push(&high_priority_queue, running_process);
            }
            running_process->interruptions++;
            running_process = NULL;
        }
    }
}

// Todo proceso que ingresa al scheduler por primera vez, lo hace en
// la cola High y estado Ready
// Agrega los procesos a las colas segun corresponda
// Si cumple la condicion, el proceso se agrega la cola HIGH
// Es decir, ingresa al scheduler
void add_process(Process** processes, int num_processes, int current_tick) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i]->t_start == current_tick) {
            processes[i]->estado = READY;
            push(&high_priority_queue, processes[i]);
        }
    }
}

// Si un proceso cumple la condicion para subir de cola, se cambia de cola
void low_to_high_priority(Queue* low, Queue* high, int current_tick) {
    for (int i = 0; i < low->len; i++) {
        Process* p = low->arr[i];
        if (2 * p->deadline_time < current_tick - p->tlcpu) {
            pop(low, current_tick);
            push(high, p);
            i--;  // ajustamos el indice para cuando recorra nuevamente el for
        }
    }
}

// // Selecciona el siguiente proceso a ejecutar
// // Actualiza el estado del proceso a RUNNING y los tiempos de respuesta
void next_process(Queue* high, Queue* low, int current_tick) {
// // Si no hay un proceso en estado RUNNING, se selecciona el proceso de mayor prioridad
    if (running_process == NULL) {
        if (high->len > 0) {
            running_process = peek(high);
            pop(high, current_tick);
        }}
    else if (running_process != NULL) {
        running_process->estado = RUNNING;
        // si el proceso todavia esta en la cpu, se actualiza su tlcpu
        running_process->tlcpu = current_tick;
        running_process->available_quantum--;
        running_process->response_time = current_time - running_process->t_start; 
    }
}

// Funcion que ejecuta el scheduler a partir de un loop while
void run_scheduler(Process** processes, int num_processes, int current_tick) {
    bool all_finished = false;

    while (!all_finished) {
        update_process_status(processes, num_processes);
        update_running_process();
        add_process(processes, num_processes, current_tick);
        low_to_high_priority(&low_priority_queue, &high_priority_queue, current_tick);
        next_process(&high_priority_queue, &low_priority_queue, current_tick);

        all_finished = true;
        for (int i = 0; i < num_processes; i++) {
            if (processes[i]->estado != FINISHED) {
                all_finished = false;
                break;
            }
        }

        current_tick++;
    }
}