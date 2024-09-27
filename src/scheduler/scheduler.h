#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../process/process.h"
#include "../queue/queue.h"

void declaring_queues(int quantum, int num_processes);
void update_process_status(Process** processes, int num_processes);
void update_running_process();
void add_process(Process** processes, int num_processes, int current_tick);
void low_to_high_priority(Queue* low, Queue* high, int current_tick);
void next_process(Queue* high, Queue* low, int current_tick);
void run_scheduler(Process** processes, int num_processes, int current_tick);