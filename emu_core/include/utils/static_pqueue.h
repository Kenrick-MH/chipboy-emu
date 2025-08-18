#ifndef STATIC_HEAP_H
#define STATIC_HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <emu_error.h>
#include <platform/error_handling.h>

/* Generic type macro, akin to C++'s generic types */
#define static_pqueue(type)  type##_spqueue
#define __spqueue_methods_type__(type)   type##_spqueue_methods_t_
#define __spqueue_methods__(type)   type##_spqueue_methods_
#define __spqueue_comparator__(type) type##_spqueue_comparator_fn
#define __spqueue_func__(type, name) type##_spqueue_##name
#define static_pqueue_create(type)  __spqueue_func__(type, create)
#define static_pqueue_struct(type, max_size, dat_ptr, comp_fn)  \
        (static_pqueue(type)){                                                    \
        .max_len = (max_size),                                                         \
        .len = 0,                                                                    \
        .comparator = (comp_fn),                                                          \
        .data = (dat_ptr),                                                                \
        .mts = &__spqueue_methods__(type)                                        \
    };                                                                               \

#define DECL_STATIC_PQUEUE_TYPE(type)                                                \
typedef struct static_pqueue(type) static_pqueue(type);                              \
                                                                                    \
typedef bool (* __spqueue_comparator__(type))(type e1, type e2);                     \
                                                                                    \
typedef struct {                                                                     \
    bool (*is_empty)(static_pqueue(type)*);                                          \
    bool (*is_full)(static_pqueue(type)*);                                           \
    void (*push)(static_pqueue(type)*, type);                                        \
    type (*front)(static_pqueue(type)*);                                             \
    type (*pop)(static_pqueue(type)*);                                               \
} __spqueue_methods_type__(type);                                                    \
                                                                                    \
struct static_pqueue(type) {                                                         \
    const size_t max_len;                                                            \
    size_t len;                                                                      \
    /* Returns TRUE if e1 < e2, and makes heap a max-heap */                         \
    const __spqueue_comparator__(type) comparator;                                   \
    type *const data;                                                                \
    const __spqueue_methods_type__(type) *mts;                                   \
};                                                                                   \
                                                                                    \
static inline bool __spqueue_func__(type, is_empty)(static_pqueue(type) *heap) {     \
    return (heap->len == 0);                                                         \
}                                                                                    \
                                                                                    \
static inline bool __spqueue_func__(type, is_full)(static_pqueue(type) *heap) {      \
    return heap->len == heap->max_len;                                               \
}                                                                                    \
                                                                                    \
static void __spqueue_func__(type, push)(static_pqueue(type) *heap, type element) {  \
    if (heap->len == heap->max_len) {                                                \
        emu_die(STATUS_FULL_CONTAINER, "Pushing when container is full.");           \
    }                                                                                \
    heap->data[heap->len++] = element;                                               \
    if (heap->len == 1) { return; }                                                  \
    int curr_idx = heap->len - 1;                                                    \
    int par_idx = (curr_idx - 1)/2;                                                  \
    while (curr_idx > 0 && heap->comparator(heap->data[par_idx], heap->data[curr_idx])) { \
        type temp = heap->data[par_idx];                                             \
        heap->data[par_idx] = heap->data[curr_idx];                                  \
        heap->data[curr_idx] = temp;                                                 \
        curr_idx = par_idx;                                                          \
        par_idx = (curr_idx - 1)/2;                                                  \
    }                                                                                \
}                                                                                    \
                                                                                    \
static inline type __spqueue_func__(type, front)(static_pqueue(type) *heap) {        \
    if (heap->len == 0) {                                                            \
        emu_die(STATUS_EMPTY_CONTAINER, "Container is empty!");                      \
    }                                                                                \
    return heap->data[0];                                                            \
}                                                                                    \
                                                                                    \
static inline int __spqueue_func__(type, pick_priority)(static_pqueue(type) *heap, int root_idx) { \
    int left_idx = (root_idx << 1) + 1;                                              \
    int right_idx = left_idx + 1;                                                    \
    type priority_val = heap->data[root_idx];                                        \
    int priority_idx = root_idx;                                                     \
    if (left_idx < heap->len && heap->comparator(priority_val, heap->data[left_idx])) { \
        priority_val = heap->data[left_idx];                                         \
        priority_idx = left_idx;                                                     \
    }                                                                                \
    if (right_idx < heap->len && heap->comparator(priority_val, heap->data[right_idx])) { \
        priority_val = heap->data[right_idx];                                        \
        priority_idx = right_idx;                                                    \
    }                                                                                \
    return priority_idx;                                                             \
}                                                                                    \
                                                                                    \
static type __spqueue_func__(type, pop)(static_pqueue(type) *heap) {                 \
    if (heap->len == 0) {                                                            \
        emu_die(STATUS_EMPTY_CONTAINER, "Popped container that is empty!");          \
    }                                                                                \
    type front = heap->data[0];                                                      \
    if (heap->len == 1) {                                                            \
        --heap->len;                                                                 \
        return front;                                                                \
    }                                                                                \
    type last = heap->data[--heap->len];                                             \
    heap->data[0] = last;                                                            \
    int curr_idx = 0;                                                                \
    int priority_idx = __spqueue_func__(type, pick_priority)(heap, curr_idx);        \
    while (priority_idx != curr_idx) {                                               \
        type temp = heap->data[priority_idx];                                        \
        heap->data[priority_idx] = heap->data[curr_idx];                             \
        heap->data[curr_idx] = temp;                                                 \
        curr_idx = priority_idx;                                                     \
        priority_idx = __spqueue_func__(type, pick_priority)(heap, curr_idx);        \
    }                                                                                \
    return front;                                                                    \
}                                                                                    \
                                                                                    \
static const __spqueue_methods_type__(type) __spqueue_methods__(type) = {            \
    .is_empty = __spqueue_func__(type, is_empty),                                    \
    .is_full  = __spqueue_func__(type, is_full),                                     \
    .push     = __spqueue_func__(type, push),                                        \
    .front    = __spqueue_func__(type, front),                                       \
    .pop      = __spqueue_func__(type, pop)                                          \
};                                                                                   \
                                                                                    \
static inline static_pqueue(type) __spqueue_func__(type, create)(                   \
    size_t max_size,                                                                 \
    type *data,                                                                      \
    __spqueue_comparator__(type) comp                                                \
) {                                                                                  \
    return (static_pqueue(type)){                                                    \
        .max_len = max_size,                                                         \
        .len = 0,                                                                    \
        .comparator = comp,                                                          \
        .data = data,                                                                \
        .mts = &__spqueue_methods__(type)                                        \
    };                                                                               \
}



#endif // STATIC_HEAP_H
