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
	const char* nombre;
	int burst_time; // Tiempo de ejecucion por rafaga!
	status estado;
	int burst_number; // RAFAGA!, tu tutututu (cantidad)
	int waiting_IO_time;
	int deadline_time; //deadline time
	int tlcpu; // Time last tick cpu
	int t_start; //start tick
	int available_quantum; //cuanto quantum tiene disponible
	int turnaround_time; //tiempo de respuesta
} Process;


Process* create(int pid, const char* nombre, int burst, int rafaga, int inter_IO_time, int deadline, int quantum, int tstart);
int priority(Process* p, int tnow);
