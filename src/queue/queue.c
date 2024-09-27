#include "queue.h"
#include <stdlib.h>
#include <stdbool.h>

Queue create_Q(int quantum, int processes) {
	Queue q;
	q.arr = (Process**)malloc(sizeof(Process*) * processes),
	q.len = 0;
	q.max_len = processes;
	q.quantum = quantum;
	return q;
}

void swap(Queue* q, int idx1, int idx2) {
// 	// if (idx1 > q.max_len || idx2 > q.max_len || idx1 < 0 || idx2 < 0) {
// 	// 	return 1;
// 	// }

	Process* temp = q->arr[idx1];
	q->arr[idx1] = q->arr[idx2];
	q->arr[idx1] = temp;
// 	// return 0;
}

// Is this go?

// nos sirve para flotar un proceso hacia arriba
// ajusta la posicion de un proceso en la cola
// de acuerdo a su prioridad
void float_up(Queue* q, int tnow) {
	int idx = q->len - 1;
	// while (idx > 0) {
	// 	int parent_idx = (idx - 1) / 2;
	// 	if (compare_priority(q->arr[idx], q->arr[parent_idx], tnow) > 0) {
	// 		swap(q, idx, parent_idx);
	// 		idx = parent_idx;
	// 	} else {
	// 		break;
	// 	}
	while (idx > 0) {
		int child_priority = priority(q->arr[idx], tnow);
		int parent_priority = priority(q->arr[(idx - 1) / 2], tnow);
		if (child_priority < parent_priority) {
			break;
		}
		if (child_priority == parent_priority && q->arr[idx]->pid > q->arr[(idx - 1) / 2]->pid) {
			break;
		}
		swap(q, idx, (idx - 1) / 2);
		idx = (idx - 1) / 2;
	}	
}


// funcion que sirve para flotar un proceso hacia abajo
// ajusta la posicion de un proceso en la cola
// de acuerdo a su prioridad
void float_down(Queue* q, int tnow) {
	int idx = 0;
	int idx_to_swap = 0;
	int left_child_priority;
	int right_child_priority;
	int parent_priority;
	int child_priority;

	while (idx * 2 + 1 < q->len) {
		right_child_priority = -10000000;
		left_child_priority = priority(q->arr[idx * 2 + 1], tnow);
		if (idx * 2 + 2 < q->len) {
			right_child_priority = priority(q->arr[idx * 2 + 2], tnow);
		}

		if (left_child_priority > right_child_priority || (left_child_priority == right_child_priority && q->arr[idx * 2 + 1]->pid < q->arr[idx * 2 + 2]->pid)) {
			idx_to_swap = idx * 2 + 1;
		} else {
			idx_to_swap = idx * 2 + 2;
		}

		parent_priority = priority(q->arr[idx],tnow);
		child_priority = priority(q->arr[idx_to_swap], tnow);
		
		if (parent_priority < child_priority || (parent_priority == child_priority && q->arr[idx]->pid > q->arr[idx_to_swap]->pid)) {
			swap(q, idx, idx_to_swap);
		} else {
			break;
		}

		idx = idx_to_swap;
	}
	// int index = 0;
	// while(true){
	// 	int left = 2 * index + 1;
	// 	int right = 2 * index + 2;
	// 	int high_priority = index;

	// 	if (left < q->len && compare_priority(q->arr[left], q->arr[high_priority], tnow) > 0) {
	// 		high_priority = left;
	// 	}

	// 	if (right < q->len && compare_priority(q->arr[right], q->arr[high_priority], tnow) > 0) {
	// 		high_priority = right;
	// 	}

	// 	if (high_priority == index) {
	// 		swap(q, index, high_priority);
	// 		index = high_priority;
	// 	} else {
	// 		break;
	// 	}
	// }
}

// funcion que sirve para agregar un proceso a la cola
// se agrega al final de la cola
void push(Queue* q, Process* p) {
	// q.arr[q.len] = p;
	// ++q.len;
	if (q->len < q->max_len) {
		q->arr[q->len] = p;
		++q->len;
		float_up(q, p->tlcpu);
	}
}


// funcion que nos sirve para 
// eliminar un proceso de la cola
// y ajustar la cola
void pop(Queue* q, int current_tick) {
	q->arr[0] = q->arr[q->len-1];
	q->len--;
	float_down(q, current_tick);
	// if (q->len > 0) {
	// 	q->arr[0] = q->arr[q->len - 1];
	// 	--q->len;
	// 	}
}


// funcion que nos sirve para obtener el proceso
// con mayor prioridad en la cola
Process* peek(Queue* q) {
	// return q.arr[0];
	if (q->len > 0) {
		return q->arr[0];
	}
	return NULL;
}

// funcion que nos sirve para mover un proceso
// de una cola a otra
// basados en la prioridad del proceso
// desde la cola LOW a la cola HIGH
void prioritize(Queue* high_q, Queue* low_q, int tnow) {
    for (int i = 0; i < low_q->len; i++) {
        Process* p = low_q->arr[i];
        if (2 * p->deadline_time < tnow - p->tlcpu) {
			//lo sacamos de la queue low
            swap(low_q, i, low_q->len - 1);
			// reducimos el size de la queue
            low_q->len--;
			// flotamos el proceso hacia abajo
			// o sea, reordenamos la cola
            float_down(low_q, tnow);
			// lo agregamos a la cola high
            push(high_q, p);

            i--;  // Recheck this index as we've swapped a new process here
        }
    }
}

int get_highest_priority_process(Queue* q, int current_tick)
{
	int max_priority = 10000000;
	int highest_priority_process_pid = 0;
	for (int i = 0; i < q->len; i++) {
		if (priority(q->arr[i], 0) < max_priority) {
			max_priority = priority(q->arr[i], current_tick);
			highest_priority_process_pid = q->arr[i]->pid;
		}
	}
	return highest_priority_process_pid;
}


// /** EJEMPLO **/
// #include <stdio.h>
//
// // Define maximum size of the priority queue
// #define MAX 100
//
// // Define PriorityQueue structure
// typedef struct {
//     int items[MAX];
//     int size;
// } PriorityQueue;
//
// // Define heapifyUp function to maintain heap property
// // during insertion
// void heapifyUp(PriorityQueue* pq, int index)
// {
//     if (index && pq->items[(index - 1) / 2] > pq->items[index]) {
//         swap(&pq->items[(index - 1) / 2], &pq->items[index]);
//         heapifyUp(pq, (index - 1) / 2);
//     }
// }
//
// // Define enqueue function to add an item to the queue
// void enqueue(PriorityQueue* pq, int value)
// {
//     if (pq->size == MAX) {
//         printf("Priority queue is full\n");
//         return;
//     }
//
//     pq->items[pq->size++] = value;
//     heapifyUp(pq, pq->size - 1);
// }
//
// // Define heapifyDown function to maintain heap property
// // during deletion
// int heapifyDown(PriorityQueue* pq, int index)
// {
//     int smallest = index;
//     int left = 2 * index + 1;
//     int right = 2 * index + 2;
//
//     if (left < pq->size
//         && pq->items[left] < pq->items[smallest])
//         smallest = left;
//
//     if (right < pq->size
//         && pq->items[right] < pq->items[smallest])
//         smallest = right;
//
//     if (smallest != index) {
//         swap(&pq->items[index], &pq->items[smallest]);
//         heapifyDown(pq, smallest);
//     }
// }
//
// // Define dequeue function to remove an item from the queue
// int dequeue(PriorityQueue* pq)
// {
//     if (!pq->size) {
//         printf("Priority queue is empty\n");
//         return -1;
//     }
//
//     int item = pq->items[0];
//     pq->items[0] = pq->items[--pq->size];
//     heapifyDown(pq, 0);
//     return item;
// }
//
// // Define peek function to get the top item from the queue
// int peek(PriorityQueue* pq)
// {
//     if (!pq->size) {
//         printf("Priority queue is empty\n");
//         return -1;
//     }
//     return pq->items[0];
// }
//
// // Define main function
// int main()
// {
//     // Initialize priority queue
//     PriorityQueue pq = { { 0 }, 0 };
//     // Add items to the queue
//     enqueue(&pq, 3);
//     enqueue(&pq, 2);
//     enqueue(&pq, 15);
//     enqueue(&pq, 5);
//     enqueue(&pq, 4);
//     enqueue(&pq, 45);
//
//     // Dequeue an item and print it
//     printf("%d dequeued from queue\n", dequeue(&pq));
//     // Print the top item of the queue
//     printf("Top element is %d\n", peek(&pq));
//
//     return 0;
// }
