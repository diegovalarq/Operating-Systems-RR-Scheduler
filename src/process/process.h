#pragma once

typedef enum {
	READY,
	WAITING,
	RUNNING,
	FINISHED,
} status;

typedef struct {
	int pid;
	const char* nombre;
	int burst; // Tiempo de ejecucion por rafaga!
	status estado;
	int rafaga; // RAFAGA!, tu tutututu (cantidad)
	int inter_IO_time;
	int deadline;
	int tlcpu; // Time last tick cpu
} Process;


Process* create(int pid, const char* nombre, int burst, int rafaga, int inter_IO_time, int deadline);
int priority(Process* p, int tnow);
