#include<utils/static_heap.h>

DECL_STATIC_PQUEUE_TYPE(int);

static bool comparator(int e1, int e2){
    return e1 < e2;
}

typedef struct twople
{
    int first;
    int second;
} twople;

DECL_STATIC_PQUEUE_TYPE(twople);
static bool twople_comparator(twople e1, twople e2){
    if (e1.first == e2.first){
        return e1.second < e2.second;
    }   

    return e1.first < e2.second;
}

int main(){
    int int_buf[20];
    int_spqueue_t my_pq = int_spqueue_create(20, int_buf, comparator);


    // int_spqueue_push(&my_heap, 0);
    // int_spqueue_push(&my_heap, 1);
    // int_spqueue_push(&my_heap, 4);

    // STATIC_PQUEUE_CREATE(my_twople_queue, twople, 20, twople_comparator);
    // twople_spqueue_is_empty(&my_twople_queue);
    // my_twople_queue.len;
}