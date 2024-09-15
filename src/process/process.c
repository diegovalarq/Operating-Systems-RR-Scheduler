#include "process.h"
#include <stdlib.h>

// todo: const char* nombre quza no sea lo mejor, revisar
Process* create(int pid, const char* nombre, int burst, int rafaga, int inter_IO_time, int deadline) {
	Process* p = (Process*)malloc(sizeof(Process));;
	p->pid = pid;
	p->burst = burst ;
	p->rafaga = rafaga ;
	p->inter_IO_time = inter_IO_time ;
	p->deadline = deadline ;
	p->tlcpu = 0;
	p->nombre = nombre;
	p->estado = READY;

	return p;
}


int priority(Process* p, int tnow) {
	return tnow - p->tlcpu - p->deadline;
}
