#include "process.h"
#include <stdlib.h>

// todo: const char* nombre quza no sea lo mejor, revisar
Process* create(int pid, char* nombre, int burst, int rafaga, int inter_IO_time, int deadline, int t_start) {
//, int quantum) {
	Process* p = (Process*)malloc(sizeof(Process));
	p->pid = pid;
	p->nombre = strdup(nombre);
	p->burst_time = burst ;
	p->burst_number = rafaga ;
	p->waiting_IO_time = inter_IO_time ;
	p->deadline_time = deadline ;
	p->tlcpu = 0;
	p->estado = READY;
	// p->available_quantum = quantum;
	// p->turnaround_time = 0;
	p->t_start = t_start;

	return p;
}


int priority(Process* p, int tnow) {
	return tnow - p->tlcpu - p->deadline_time;
}
