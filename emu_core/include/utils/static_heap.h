#ifndef STATIC_HEAP_H
#define STATIC_HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <emu_error.h>
#include <platform/error_handling.h>

typedef int type;
typedef struct sminheap
{
    const size_t max_len;
    size_t len;
    bool (*comparator)(type e1, type e2);
    type *data;
} type_sminheap_t;

typedef struct sminheap_res
{
    const type result;
    const error_code_t status;    
} type_sminheap_res_t;

bool type_sminheap_is_empty(type_sminheap_t *heap) {return (heap->len == 0);}
bool type_sminheap_is_full(type_sminheap_t *heap) {return (heap->len == heap->max_len);}

void type_sminheap_push(type_sminheap_t *heap, type element)
{
    if (heap->len == heap->max_len){
        // die from too full, or return an error.
    }

    heap->data[heap->len++] = element;

    /* Early return when heap is initially empty */
    if (heap->len == 1){ return; }

    /* Heapify Up the heap */
    int curr_idx = heap->len - 1;
    int par_idx = (curr_idx - 1)/2;
    
    while (par_idx != -1 && heap->comparator(heap->data[par_idx], heap->data[curr_idx])){
        /* Swap if invariant is broken. */
        type temp = heap->data[par_idx];
        heap->data[par_idx] = heap->data[curr_idx];
        heap->data[curr_idx] = temp;
        curr_idx = par_idx;
        par_idx = (curr_idx - 1)/2;
    }
}

type type_sminheap_front(type_sminheap_t *heap)
{
    if (heap->len == 0){
        emu_die(STATUS_EMPTY_CONTAINER, "Container is empty!");  
    }

    return heap->data[0];
}

static inline int pick_priority(type_sminheap_t *heap, int root_idx){
    int left_idx = (root_idx << 2) + 1;
    int right_idx = left_idx + 1;
    type priority_val = heap->data[root_idx];
    int priority_idx = root_idx;

    if (left_idx < heap->len && heap->comparator(priority_val, heap->data[left_idx])){
            priority_val = heap->data[left_idx];
            priority_idx = left_idx;
        }

        if (right_idx < heap->len && heap->comparator(priority_val, heap->data[right_idx])){
            priority_val = heap->data[right_idx];
            priority_idx = right_idx;
        }

    return priority_idx;
} 

type type_sminheap_pop(type_sminheap_t *heap)
{
    if (heap->len == 0){
        emu_die(STATUS_EMPTY_CONTAINER, "Popped container that is empty!");
    }

    type front = heap->data[0];
    if (heap->len == 1){
        --heap->len;
        return front;
    }

    type last = heap->data[--heap->len];
    heap->data[0] = last;
    
    /* Heapify down the heap */ 
    int curr_idx = 0;
    int priority_idx = pick_priority(heap, curr_idx);
    while (priority_idx != curr_idx){
        type temp = heap->data[priority_idx];
        heap->data[priority_idx] = heap->data[curr_idx];
        heap->data[curr_idx] = temp;

        curr_idx = priority_idx;
        priority_idx = pick_priority(heap, curr_idx);
    }

    return front;
}



#define DECL_STATIC_HEAP(type) \
    typedef struct {                        \
        const size_t max_len;                     \
        size_t len;                         \
        type *data;                         \
    } type##_sminheap_t               \   
                                            \
    bool type##_sminheap_is_empty(type##_sminheap_t *heap) { return heap-> }\ 
                                            \
                                            \
                                            \
                                            \ 
                                            \

#define STATIC_HEAP_CREATE(container_name, type, max_size) \ 
    type container_name##_buf [max_size]; \                                                                                      
    type##_sminheap_t container_name = (type##_sminheap_t) { \                                                      
        .max_len = max_size,                                             \      
        .len     = 0,                                                    \
        .data    = container_name##_buf                                  \                 
    };

#endif // STATIC_HEAP_H



