#include "process.h"
#include <stdlib.h>

// todo: const char* nombre quza no sea lo mejor, revisar
Process* create(int pid, char* nombre, int burst, int rafaga, int inter_IO_time, int deadline, int t_start) {
	Process* p = (Process*)malloc(sizeof(Process));
	p->pid = pid;
	p->nombre = strdup(nombre); // strdup agregado porque no guaraba bien los nombres
	p->burst_time = burst ;
	p->burst_number = rafaga ;
	p->waiting_IO_time = inter_IO_time;
	p->available_waiting_time = inter_IO_time;
	p->deadline_time = deadline ;
	p->tlcpu = 0;
	p->estado = READY;
	p->available_quantum = 0;
	p->t_start = t_start;
	p->interruptions = 0;
	p->turnaround_time = 0;
	p->sum_past_deadline = 0;
	p->sum_waiting_time = 0;
	p->response_time = 0;

	return p;
}


int priority(Process* p, int tnow) {
	return (tnow - p->tlcpu) - p->deadline_time;
}
