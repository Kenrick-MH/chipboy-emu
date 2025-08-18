
#include <schedule.h>
#include <common.h>
#include <utils/static_pqueue.h>
#include <platform/error_handling.h>

DECL_STATIC_PQUEUE_TYPE(device_event_t);

/* Needs to be min-heap */
static bool dev_event_comp(device_event_t e1, device_event_t e2){
    return e1.timestamp > e2.timestamp;
}

static device_event_t event_buffer[MAX_EVENT_QUEUE_LEN];

/**
 *  Rather hacky way to do this, but alas.
 */
static static_pqueue(device_event_t) event_pqueue = 
    static_pqueue_struct(device_event_t, MAX_EVENT_QUEUE_LEN,
                             event_buffer, dev_event_comp);

static const uint64_t TIMESTAMP_LOOPBACK = 0x0FFFFFFFFFFFFFFF;

void scheduler_init()
{

}

void execute_next_event()
{
    device_event_t next = event_pqueue.mts->pop(&event_pqueue);
    next.exec_event();
}


/**
 *  Schedules the next event in the event queue
 */
void schedule_next_event(device_event_t event)
{
    if (event_pqueue.mts->is_full(&event_pqueue)) {
        emu_die(STATUS_FULL_CONTAINER, "Too much device events scheduled!");
    }
    
    /* 
        Check if event timestamp > LOOPBACK threshold.
        If so, normalize everything to the minimum time. 
    */
    if (event.timestamp > TIMESTAMP_LOOPBACK){
        uint64_t min_time = event_pqueue.mts->front(&event_pqueue).timestamp;

        event.timestamp -= min_time;
        for (unsigned i = 0; i < event_pqueue.len; ++i){
            event_pqueue.data[i].timestamp -= min_time;
        }
    }

    event_pqueue.mts->push(&event_pqueue, event);
}
