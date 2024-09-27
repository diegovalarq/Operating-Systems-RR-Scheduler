#pragma once
#include "../process/process.h"

typedef struct {
	Process** arr;
	int len;
	int max_len;
	int quantum; // quantum = 2 * q, donde q se lo dan al programa, tambien llamado time slice, tiempo maximo de procesamiento por proceso
} Queue;

Queue create_Q(int quantum, int processes);

// Just swap the processes
void swap(Queue* q, int idx1, int idx2);

/**
Add the element to end of the heap.
Restore the heap property by the comparing the added element with its parent. If it can violates the heap property, swap them.
Continues this Process up the heap until the correct position is found or root is reached.
// ME: just insert, we need a function to float_up, this is because I don't want to keep this function alive after it finished everything
*/
void push(Queue* q, Process* p);

/*
 * The steps that the previous insert didn't have, here we need to actually understand what is happenening and how to prioritize
 */
void float_up(Queue* q, int tnow);
void float_down(Queue* q, int tnow);

/*
Replace the root of heap with the last element in the heap.
Reduce the size of the heap by the one.
Restore the heap property by the recursively comparing the new root with its children and swapping it
with the higher priority child in the max-heap or the lower priority child in the min heap.
Continues this Process down the heap until the correct position is found or the leaf is reached.
*/
void pop(Queue* q, int current_tick);

/*
* Return the element at the root of the heap.
* Don't call it if Queue is empty
*/
Process* peek(Queue* q);

/*
Locate the element whose the priority needs to be updated.
Update the priority of the element.
If the priority is increased in the max-heap or decreased in the min-heap and it can restore the heap property by the heapifying up from the element.
If the priority is decreased in the max-heap or increased in the min-heap and restore the heap property by the heapifying down from element.
Cambiar nombre a algo mejor
*/
// void move();

/*
* Todo: think more about this because how do you actually move the process to the other q?
* how do you fix the hole?, only the first processs of low should go to high?
*/
void prioritize(Queue* low, Queue* high, int tnow);
int get_highest_priority_process(Queue* q, int current_tick);