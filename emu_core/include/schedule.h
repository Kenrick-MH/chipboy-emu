#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <common.h>
#include <emu_error.h>

typedef struct device_event_t {
    uint64_t timestamp; 
    void (* exec_event)();
} device_event_t;

device_event_t event_queue[MAX_DEVICE_NUMBER];

error_code_t execute_next_event();
error_code_t schedule_next_event();

#endif // SCHEDULE_H