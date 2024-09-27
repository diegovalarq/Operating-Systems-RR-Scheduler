#pragma once

typedef enum {
	READY,
	WAITING,
	RUNNING,
	FINISHED,
} status;

// Agregue nuevas variables. Puede que hagan falta mas aun
// Revisar si corresponden ajustes
typedef struct {
	int pid;
	char* nombre;
	int burst_time; // Tiempo de ejecucion por rafaga!
	status estado;
	int burst_number; // RAFAGA!, tu tutututu (cantidad)
	int waiting_IO_time; // Tiempo de espera inicial
	int available_waiting_time; // Tiempo de espera actual
	int deadline_time; //deadline time
	int tlcpu; // Time last tick cpu
	int t_start; //start tick
	int available_quantum; //cuanto quantum tiene disponible
	int turnaround_time; //tiempo de respuesta
	int response_time; // tiempo que entro por primera vez en cpu
	int interruptions; //cantidad de interrupciones del proceso antes de finalizar
	int sum_past_deadline; //suma de los tiempos que el proceso ejecuto pasado su deadline
	int sum_waiting_time; // suma de los intervalos en que el proceso estuvo en estado waiting o ready
} Process;


Process* create(int pid, char* nombre, int burst, int rafaga, int inter_IO_time, int deadline, int t_start);//, int quantum);
int priority(Process* p, int tnow);
