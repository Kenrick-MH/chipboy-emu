
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <queue>
#include <stdio.h>

extern "C" {
    #include <setjmp.h>
    #include <utils/static_pqueue.h>
    #include <platform/error_handling.h>
    #include <stdlib.h>   // for exit()

    jmp_buf jump_buf;

    void emu_die(error_code_t code, const char *msg)
    {
        printf(
            "A fatal error encountered!\n"
            "Error Code: %d\n"
            "Code Description: %s\n"
            "Message: %s\n",
            (int) code, "", msg);
        longjmp(jump_buf, code);
    }

    DECL_STATIC_PQUEUE_TYPE(int)
}

bool int_min_comparator(int e1, int e2){
    return e1 > e2;
}

void print_int_arr(int *arr, size_t n){
    printf("[");
    for (size_t i = 0; i < n; ++i){
        printf("%d, ", arr[i]);
    }

    printf("]\n");
}

TEST_CASE("Priority Queue Init") {
    int buf[16];
    static_pqueue(int) pq = static_pqueue_create(int)(16, buf, int_min_comparator);
    REQUIRE(pq.len == 0);
    REQUIRE(pq.max_len == 16);
    REQUIRE(int_spqueue_is_empty(&pq));
}

TEST_CASE("Priority Queue Push/Pop") {
    int buf[5];
    static_pqueue(int) pq = static_pqueue_create(int)(5, buf, int_min_comparator);

    REQUIRE(pq.max_len == 5);
    auto code = setjmp(jump_buf);
    if (code == 0){   print_int_arr(buf, 5);
        pq.mts->pop(&pq);
        FAIL("THIS SHOULD ERROR OUT");
    } 
    REQUIRE(code == STATUS_EMPTY_CONTAINER);

    std::cout << "PQ Len : " << pq.len << std::endl;

    pq.mts->push(&pq, 5);
    pq.mts->push(&pq, 2);
    pq.mts->push(&pq, 8);
    pq.mts->push(&pq, 4);
    pq.mts->push(&pq, 6);

    code = setjmp(jump_buf);
    if (code == 0){
        pq.mts->push(&pq, 10);
        FAIL("THIS SHOULD ERROR OUT");
    } 
    REQUIRE(code == STATUS_FULL_CONTAINER);

    /* Now start popping */ 
    REQUIRE(pq.mts->front(&pq) == 2);
    auto popped = pq.mts->pop(&pq);

    REQUIRE(popped == 2);
    REQUIRE(pq.len == 4);
    
    REQUIRE(pq.mts->pop(&pq) == 4);
    REQUIRE(pq.len == 3);

    /* After a bit of popping, push */
    pq.mts->push(&pq, 1);
    REQUIRE(pq.len == 4);

    pq.mts->push(&pq, 10);
    REQUIRE(pq.len == 5);

    REQUIRE(pq.mts->front(&pq) == 1);
    REQUIRE(pq.mts->pop(&pq) == 1);
    REQUIRE(pq.mts->pop(&pq) == 5);
    REQUIRE(pq.mts->pop(&pq) == 6);
    REQUIRE(pq.mts->pop(&pq) == 8);
    REQUIRE(pq.mts->pop(&pq) == 10);
    REQUIRE(pq.len == 0);

    code = setjmp(jump_buf);
    if (code == 0){
        pq.mts->pop(&pq);
        FAIL("THIS SHOULD ERROR OUT");
    } 
    REQUIRE(code == STATUS_EMPTY_CONTAINER);
}

