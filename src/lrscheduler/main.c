#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"
#include "scheduler.c"

// #define MAX_PROCESS 8 //placeholder

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

void clear(Process** p, int len) {
	for (int i = 0; i < len; ++i) {
		free(p[i]);
	}
	free(p);
}

int main(int argc, char const *argv[])
{
    // Codigo para verificar que se ingresa el input correctamente
    if (argc < 3) {
        fprintf(stderr, "Input debe seguir formato: %s <file_name> <quantum>\n", argv[0]);
        return 1;
    }
	/*Lectura del input*/
	char *file_name = (char*)argv[1];
	int quantum = atoi(argv[2]);
	// InputFile *input_file = read_file(file_name);
	// char *file_name = (char*)argv[1];
    // char *output_file = (char*)argv[2];
    // q = atoi(argv[3]);


	FILE* input_file = fopen(file_name, "r");
    // if (input_file == NULL) {
    // fprintf(stderr, "Error: Could not open file %s\n", file_name);
    // exit(1);
// }

	// Process helper
	int processes;
	int pid;
	char nombre[100];
	int burst;
	int rafaga;
	int inter_IO_time;
	int deadline;
	int t_inicio;

	// Process array
	Process** process_array;
	// int current = 0;

	// Reading file and to imitate when the processes should enter the queue
    printf("Leyendo el archivo...\n");
	fscanf(input_file, "%d\n", &processes);
    printf("Alocamos memoria para los procesos...\n");
	process_array = (Process**)malloc(processes * sizeof(Process*));

    printf("Leyendo los procesos...\n");
	for (int i = 0; i < processes; ++i) {
		fscanf(input_file, "%s %d %d %d %d %d %d\n", nombre, &pid, &t_inicio, &burst, &rafaga, &inter_IO_time, &deadline);
		printf("%s %d %d %d %d %d %d\n", nombre, pid, t_inicio, burst, rafaga, inter_IO_time, deadline);
		process_array[i] = create(pid, nombre, burst, rafaga, inter_IO_time, deadline, t_inicio);
	}
    // hasta aqui todo funciona bien


    printf("Accediendo a los procesos...\n");
    for (int i = 0; i < processes; ++i) {
        printf("Proceso %d: %s %d %d %d %d %d %d\n", i, process_array[i]->nombre, process_array[i]->pid, process_array[i]->t_start, process_array[i]->burst_time, process_array[i]->burst_number, process_array[i]->waiting_IO_time, process_array[i]->deadline_time);
    }
    // hasta aqui funciona bien


    declaring_queues(q, processes);
    // Start ticking
    run_scheduler(processes, processes, 0);

	

    fclose(input_file);


    

    // Output results
    // FILE *output = fopen(output_file, "w");
    // for (int i = 0; i < processes; i++) {
    //     fprintf(output, "%s,%d,%d,%d,%d,\n",
    //             processes->nombre,
    //             processes->pid,
    //             // processes[i]->interruptions,//revisar esta linea
    //             processes[i]->turnaround_time,
    //             // processes[i]->response_time, //revisar esta linea
    //             processes[i]->waiting_IO_time,
    //             (processes[i]->turnaround_time > processes[i]->deadline_time) ? processes[i]->turnaround_time - processes[i]->deadline_time : 0);
    // }
    fclose(output);
	input_file_destroy(input_file);
    for (int i = 0; i < processes; i++) {
        free(processes);
    }

    // return 0;

	// Clean up the mess
	clear(process_array, processes);
}
