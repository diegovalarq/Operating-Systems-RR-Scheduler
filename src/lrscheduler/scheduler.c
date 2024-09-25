// #include <stdio.h>	// FILE, fopen, fclose, etc.
// #include <stdlib.h> // malloc, calloc, free, etc
// #include <stdbool.h>
// #include "../process/process.h"
// #include "../queue/queue.h"
// #include "../file_manager/manager.h"

// #define MAX_PROCESS 100

// Queue high_priority_queue;
// Queue low_priority_queue;
// Process* running_process = NULL;
// int current_time = 0;
// int q;

// // void clear(Process** p, int len) {
// // 	for (int i = 0; i < len; ++i) {
// // 		free(p[i]);
// // 	}
// // 	free(p);
// // }

// // iniciamos las colas de alta y baja prioridad
// // con sus respectivos quantums
// void initialize_scheduler(int quantum, int num_processes) {
//     q = quantum;
//     high_priority_queue = create_Q(2 * q, num_processes);
//     low_priority_queue = create_Q(q, num_processes);
// }

// // Si un proceso esta en estado waiting, se actualiza su tiempo de espera
// // Aquellos que terminan su tiempo de espera, pasan a estado READY
// void update_waiting_processes(Process** processes, int num_processes) {
//     for (int i = 0; i < num_processes; i++) {
//         if (processes[i]->estado == WAITING) {
//             processes[i]->waiting_IO_time--;
//             if (processes[i]->waiting_IO_time <= 0) {
//                 processes[i]->estado = READY;
//             }
//         }
//     }
// }

// // Si hay un proceso en estado RUNNING, se actualiza su estado
// // Disminuye el burst time cuando tienen su turno
// // Disminuye el quantum disponible cuando tienen su turno
// // Aquellos que terminan su rafaga de cpu, pasan a estado WAITING
// void update_running_process() {
//     if (running_process != NULL) {
//         running_process->burst_time--;
//         running_process->available_quantum--;

//         if (running_process->burst_time <= 0) {
//             running_process->burst_number--;
//             if (running_process->burst_number <= 0) {
//                 running_process->estado = FINISHED;
//             } else {
//                 running_process->estado = WAITING;
//                 running_process->waiting_IO_time = running_process->waiting_IO_time;
//             }
//             running_process = NULL;
//         } else if (running_process->available_quantum <= 0) {
//             running_process->estado = READY;
//             if (running_process->tlcpu == high_priority_queue.quantum) {
//                 push(&low_priority_queue, running_process);
//             } else {
//                 push(&high_priority_queue, running_process);
//             }
//             // running_process->interruptions++;
//             running_process = NULL;
//         }
//     }
// }

// // Agrega los procesos a las colas segun corresponda
// // Si cumple la condicion, el proceso se agrega la cola HIGH
// void add_new_processes(Process** processes, int num_processes) {
//     for (int i = 0; i < num_processes; i++) {
//         if (processes[i]->t_start == current_time) {
//             push(&high_priority_queue, processes[i]);
//         }
//     }
// }

// // Si un proceso cumple la condicion para subir de cola, se cambia de cola
// void check_low_to_high_priority() {
//     for (int i = 0; i < low_priority_queue.len; i++) {
//         Process* p = low_priority_queue.arr[i];
//         if (2 * p->deadline_time < current_time - p->tlcpu) {
//             pop(&low_priority_queue, current_time);
//             push(&high_priority_queue, p);
//             i--;  // Adjust index after removal
//         }
//     }
// }

// // Selecciona el siguiente proceso a ejecutar
// // Si no hay un proceso en estado RUNNING, se selecciona el proceso de mayor prioridad
// // Actualiza el estado del proceso a RUNNING y los tiempos de respuesta
// void select_next_process() {
//     if (running_process == NULL) {
//         if (high_priority_queue.len > 0) {
//             running_process = peek(&high_priority_queue);
//             pop(&high_priority_queue, current_time);
//         } else if (low_priority_queue.len > 0) {
//             running_process = peek(&low_priority_queue);
//             pop(&low_priority_queue, current_time);
//         }

//         if (running_process != NULL) {
//             running_process->estado = RUNNING;
//             running_process->tlcpu = current_time;
//             running_process->available_quantum = (running_process->tlcpu == high_priority_queue.quantum) ? high_priority_queue.quantum : low_priority_queue.quantum;
//             // if (running_process->response_time == -1) {
//                 // running_process->response_time = current_time - running_process->t_inicio;
//             //}
//         }
//     }
// }

// // Funcion que ejecuta el scheduler a partir de un loop while
// void run_scheduler(Process** processes, int num_processes) {
//     bool all_finished = false;

//     while (!all_finished) {
//         update_waiting_processes(processes, num_processes);
//         update_running_process();
//         add_new_processes(processes, num_processes);
//         check_low_to_high_priority();
//         select_next_process();

//         all_finished = true;
//         for (int i = 0; i < num_processes; i++) {
//             if (processes[i]->estado != FINISHED) {
//                 all_finished = false;
//                 break;
//             }
//         }

//         current_time++;
//     }
// }