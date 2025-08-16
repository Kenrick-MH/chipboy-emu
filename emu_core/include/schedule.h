#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <common.h>
#include <emu_error.h>

typedef struct device_event_t {
    uint64_t timestamp; 
    error_code_t(* exec_event)();
} device_event;

void scheduler_init();

void execute_next_event();


/**
 *  Schedules the next event in the event queue
 */
void schedule_next_event(device_event event);

#endif // SCHEDULE_H