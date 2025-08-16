#ifndef STATIC_HEAP_H
#define STATIC_HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <emu_error.h>
#include <platform/error_handling.h>

#define DECL_STATIC_PQUEUE_TYPE(type)                                                \
typedef bool (* type##_comparator_t)(type e1, type e2);                              \
typedef struct type##_spqueue {                                                      \
    const size_t max_len;                                                             \
    size_t len;                                                                       \
    /* Returns TRUE if e1 < e2, and makes heap a max-heap */                          \
    const type##_comparator_t comparator;                                             \
    type *const data;                                                                 \
} type##_spqueue_t;                                                                  \
                                                                                      \
static inline bool type##_spqueue_is_empty(type##_spqueue_t *heap) {                \
    return (heap->len == 0);                                                          \
}                                                                                     \
static inline bool type##_spqueue_is_full(type##_spqueue_t *heap) {                 \
    return heap->len == heap->max_len;                                              \
}                                                                                     \
                                                                                      \
static inline void type##_spqueue_push(type##_spqueue_t *heap, type element) {      \
    if (heap->len == heap->max_len) {                                                 \
        emu_die(STATUS_FULL_CONTAINER, "Pushing when container is full.");            \
    }                                                                                 \
    heap->data[heap->len++] = element;                                                \
    /* Early return when heap is initially empty */                                   \
    if (heap->len == 1) { return; }                                                   \
    /* Heapify Up the heap */                                                         \
    int curr_idx = heap->len - 1;                                                     \
    int par_idx = (curr_idx - 1)/2;                                                   \
    /* Comparator returns TRUE if e1 < e2 by default (max-heap), so swap when par < curr_idx */ \
    while (curr_idx > 0 && heap->comparator(heap->data[curr_idx], heap->data[par_idx])) { \
        /* Swap if invariant is broken. */                                            \
        type temp = heap->data[par_idx];                                              \
        heap->data[par_idx] = heap->data[curr_idx];                                   \
        heap->data[curr_idx] = temp;                                                  \
        curr_idx = par_idx;                                                           \
        par_idx = (curr_idx - 1)/2;                                                   \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline type type##_spqueue_front(type##_spqueue_t *heap) {                   \
    if (heap->len == 0) {                                                             \
        emu_die(STATUS_EMPTY_CONTAINER, "Container is empty!");                       \
    }                                                                                 \
    return heap->data[0];                                                             \
}                                                                                     \
                                                                                      \
static inline int type##_pick_priority(type##_spqueue_t *heap, int root_idx) {       \
    int left_idx = (root_idx << 1) + 1;                                               \
    int right_idx = left_idx + 1;                                                     \
    type priority_val = heap->data[root_idx];                                         \
    int priority_idx = root_idx;                                                      \
    /* Trigger when priority_val < left */                                            \
    if (left_idx < heap->len && heap->comparator(priority_val, heap->data[left_idx])) { \
        priority_val = heap->data[left_idx];                                          \
        priority_idx = left_idx;                                                      \
    }                                                                                 \
    /* Trigger when priority_val < right */                                           \
    if (right_idx < heap->len && heap->comparator(priority_val, heap->data[right_idx])) { \
        priority_val = heap->data[right_idx];                                         \
        priority_idx = right_idx;                                                     \
    }                                                                                 \
    return priority_idx;                                                              \
}                                                                                     \
                                                                                      \
static inline type type##_spqueue_pop(type##_spqueue_t *heap) {                     \
    if (heap->len == 0) {                                                             \
        emu_die(STATUS_EMPTY_CONTAINER, "Popped container that is empty!");           \
    }                                                                                 \
    type front = heap->data[0];                                                       \
    if (heap->len == 1) {                                                             \
        --heap->len;                                                                  \
        return front;                                                                 \
    }                                                                                 \
    type last = heap->data[--heap->len];                                              \
    heap->data[0] = last;                                                             \
    /* Heapify down the heap */                                                       \
    int curr_idx = 0;                                                                 \
    int priority_idx = type##_pick_priority(heap, curr_idx);                          \
    while (priority_idx != curr_idx) {                                                \
        type temp = heap->data[priority_idx];                                         \
        heap->data[priority_idx] = heap->data[curr_idx];                              \
        heap->data[curr_idx] = temp;                                                  \
        curr_idx = priority_idx;                                                      \
        priority_idx = type##_pick_priority(heap, curr_idx);                          \
    }                                                                                 \
    return front;                                                                     \
}                                                                                     \  
                                                                                    \
static inline type##_spqueue_t type##_spqueue_create(                   \
    size_t max_size,                                                    \
    type *data,                                                         \
    type##_comparator_t comp                                            \
) {                                                                     \
    return (type##_spqueue_t){                                          \
        .max_len = max_size,                                            \
        .len = 0,                                                       \
        .comparator = comp,                                             \
        .data = data                                                    \
    };                                                                  \
}

#endif // STATIC_HEAP_H



