#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

#define MAX_PROCESS 100

/* Shit to do
 * - [ ] Los procesos HIGH tienen preferencia sobre los de la cola LOW
 * - [ ] Todo proceso nuevo ingresa en la cola HIGH y lo hace con estado READY
 * - [ ] Si dos procesos o mas tienen prioridad y estan en la misma cola deberan ser ejecutados segun raundo robingu
	 * - [x] Los processos con mayor prioridad son aquellos que estan mas cerca de su deadline (T_actial - T_last_tick) - T_deadline mayor
	 * - [ ] Los procesos en estado WAITING no son elegibles para ejecutar
	 * - [x] Si hay empate en deadline se debe escoger el que tenga menor PID
 * - [ ] Un proceso en estado RUNNING solo sale del cpu si se acaba su quantum o cede la CPU
 * - [ ] Si un proceso consume todo su quantum pasa a la cola LOW, si ya se encontraba en esta cola se mantiene
 * - [ ] Si un proceso cede la CPU este se mantiene en la misma cola en la que se encontraba y el quantum no se reinicia
 * - [ ] Si se cumple para un proceso en la cola LOW que 2 * T_deadline < T_actual - T_last_tick este proceso sube a la cola HIGH
 */

/* Flujo del scheduler, por cada tick se debe realizar:
 * 1. Actualizar los procesos que hayan terminado su tiempo de espera de IO de WAITING a READY
 * 2. Si hay un proceso en estado RUNNING, actualizar su estado segun corresponda. Esto podra incluir sacarlo de la cpu si su quantum ha expirado o ha terminado su rafaga de ejecucion
 * 3. Ingresar los processos a las colas segun corresponda
 * 3.1 si un proceso salio de la cpi, ingresalo a la cola que corresponda.
 * 3.2 Para cada proceso p, comprobar si tick = T_inicio_p, e ingresarlo en la cola HIGH
 * 3.3 Para cada proceso de la cola LOW, revisar si se cumple la condicion para subir a la cola HIGH y cambiarlos de cola segun corresponda
 * 4. Si no hay un proceso en estado RUNNING ingresa el proceso de mayor prioridad en estado READY a la CPU, esto implica ignorar a todos los que se encuentren en estado WAITING, sin moverlos de su posicion actual
 *
 * Consideraciones:
 * Si el proceso termina su rafaga de cpu al mismo momenot que se acaba su quantum se considera que el proceso cedio la cpu
 * si el proceso termina su ejecucion al mismo momento que se acaba su quantum, este pasa a estado FINISHED
 *
 * Consideraciones:
 * Si el proceso termina su rafaga de cpu al mismo momenot que se acaba su quantum se considera que el proceso cedio la cpu
 * si el proceso termina su ejecucion al mismo momento que se acaba su quantum, este pasa a estado FINISHED
 */

/* Lectura archivo
 * cantidad de entradas
 * pid T_inicio T_cpu_burst N_burst IO_wait T_deadline
 */

Queue high_priority_queue;
Queue low_priority_queue;
Process* running_process = NULL;
int current_time = 0;
int q;

void clear(Process** p, int len) {
	for (int i = 0; i < len; ++i) {
		free(p[i]);
	}
	free(p);
}

// iniciamos las colas de alta y baja prioridad
// con sus respectivos quantums
void initialize_scheduler(int quantum, int num_processes) {
    q = quantum;
    high_priority_queue = create_Q(2 * q, num_processes);
    low_priority_queue = create_Q(q, num_processes);
}

// Si un proceso esta en estado waiting, se actualiza su tiempo de espera
// Aquellos que terminan su tiempo de espera, pasan a estado READY
void update_waiting_processes(Process** processes, int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i]->estado == WAITING) {
            processes[i]->waiting_IO_time--;
            if (processes[i]->waiting_IO_time <= 0) {
                processes[i]->estado = READY;
            }
        }
    }
}

// Si hay un proceso en estado RUNNING, se actualiza su estado
// Disminuye el burst time cuando tienen su turno
// Disminuye el quantum disponible cuando tienen su turno
// Aquellos que terminan su rafaga de cpu, pasan a estado WAITING
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
                running_process->waiting_IO_time = running_process->waiting_IO_time;
            }
            running_process = NULL;
        } else if (running_process->available_quantum <= 0) {
            running_process->estado = READY;
            if (running_process->tlcpu == high_priority_queue.quantum) {
                push(&low_priority_queue, running_process);
            } else {
                push(&high_priority_queue, running_process);
            }
            // running_process->interruptions++;
            running_process = NULL;
        }
    }
}

// Agrega los procesos a las colas segun corresponda
// Si cumple la condicion, el proceso se agrega la cola HIGH
void add_new_processes(Process** processes, int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i]->t_start == current_time) {
            push(&high_priority_queue, processes[i]);
        }
    }
}

// Si un proceso cumple la condicion para subir de cola, se cambia de cola
void check_low_to_high_priority() {
    for (int i = 0; i < low_priority_queue.len; i++) {
        Process* p = low_priority_queue.arr[i];
        if (2 * p->deadline_time < current_time - p->tlcpu) {
            pop(&low_priority_queue, current_time);
            push(&high_priority_queue, p);
            i--;  // Adjust index after removal
        }
    }
}

// Selecciona el siguiente proceso a ejecutar
// Si no hay un proceso en estado RUNNING, se selecciona el proceso de mayor prioridad
// Actualiza el estado del proceso a RUNNING y los tiempos de respuesta
void select_next_process() {
    if (running_process == NULL) {
        if (high_priority_queue.len > 0) {
            running_process = peek(&high_priority_queue);
            pop(&high_priority_queue, current_time);
        } else if (low_priority_queue.len > 0) {
            running_process = peek(&low_priority_queue);
            pop(&low_priority_queue, current_time);
        }

        if (running_process != NULL) {
            running_process->estado = RUNNING;
            running_process->tlcpu = current_time;
            running_process->available_quantum = (running_process->tlcpu == high_priority_queue.quantum) ? high_priority_queue.quantum : low_priority_queue.quantum;
            // if (running_process->response_time == -1) {
                // running_process->response_time = current_time - running_process->t_inicio;
            //}
        }
    }
}

// Funcion que ejecuta el scheduler a partir de un loop while
void run_scheduler(Process** processes, int num_processes) {
    bool all_finished = false;

    while (!all_finished) {
        update_waiting_processes(processes, num_processes);
        update_running_process();
        add_new_processes(processes, num_processes);
        check_low_to_high_priority();
        select_next_process();

        all_finished = true;
        for (int i = 0; i < num_processes; i++) {
            if (processes[i]->estado != FINISHED) {
                all_finished = false;
                break;
            }
        }

        current_time++;
    }
}

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	// char *file_name = (char*)argv[1];
	// int quantum = (int)*(char*)argv[2];
	// InputFile *input_file = read_file(file_name);
	char *file_name = (char*)argv[1];
    char *output_file = (char*)argv[2];
    q = atoi(argv[3]);


	// FILE* input_file = fopen(file_name, "r");

	// // Process helper
	// int processes;
	// int pid;
	// char nombre[100];
	// int burst;
	// int rafaga;
	// int inter_IO_time;
	// int deadline;
	// int t_inicio;

	// // Process array
	// Process** process_array;
	// int current = 0;

	// // Reading file and to imitate when the processes should enter the queue
	// fscanf(input_file, "%d\n", &processes);
	// process_array = (Process**)malloc(processes * sizeof(Process*));

	// for (int i = 0; i < processes; ++i) {
	// 	fscanf(input_file, "%s %d %d %d %d %d %d\n", nombre, &pid, &t_inicio, &burst, &rafaga, &inter_IO_time, &deadline);
	// 	printf("%s %d %d %d %d %d %d\n", nombre, pid, t_inicio, burst, rafaga, inter_IO_time, deadline);
	// 	process_array[i] = create(pid, nombre, burst, rafaga, inter_IO_time, deadline);
	// }

	// fclose(input_file);

	// Sort processes array

	// Start ticking

	// Alternative main
	// Based on Claude. REVISARLO BIEN
	InputFile *input_file = read_file(file_name);
    int num_processes = input_file->len - 1;
    Process* processes[MAX_PROCESS];

    for (int i = 0; i < num_processes; i++) {
        char** line = input_file->lines[i + 1];
        processes[i] = create(atoi(line[1]), line[0], atoi(line[3]), atoi(line[4]), atoi(line[5]), atoi(line[6]), 0, 0);
        processes[i]->t_start = atoi(line[2]);
        processes[i]->turnaround_time = -1;//revisar esta linea
    }

    initialize_scheduler(q, num_processes);
    run_scheduler(processes, num_processes);

    // Output results
    FILE *output = fopen(output_file, "w");
    for (int i = 0; i < num_processes; i++) {
        fprintf(output, "%s,%d,%d,%d,%d,\n",
                processes[i]->nombre,
                processes[i]->pid,
                // processes[i]->interruptions,//revisar esta linea
                processes[i]->turnaround_time,
                // processes[i]->response_time, //revisar esta linea
                processes[i]->waiting_IO_time,
                (processes[i]->turnaround_time > processes[i]->deadline_time) ? processes[i]->turnaround_time - processes[i]->deadline_time : 0);
    }
    fclose(output);
	input_file_destroy(input_file);
    for (int i = 0; i < num_processes; i++) {
        free(processes[i]);
    }

    return 0;

	// Clean up the mess
	// clear(process_array, processes);
}
