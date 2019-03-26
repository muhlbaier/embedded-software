#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"

#define NUMBER_TESTS 8

typedef struct {
    bool (*test_function)();
    char *name;
    bool expected_result;
} test_t;

bool this_test_passes() {
    return true;
}

bool this_test_fails() {
    return false;
}

bool buffer_init() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    return Buffer_GetSize(&buffer) == 0;
}

bool buffer_init_enqueue() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    if (Buffer_GetSize(&buffer) != 0) return false;

    Buffer_Enqueue(&buffer, 10);

    return Buffer_GetSize(&buffer) == 1;
}

bool buffer_init_enqueue_dequeue() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    if (Buffer_GetSize(&buffer) != 0) return false;

    Buffer_Enqueue(&buffer, 10);

    if (Buffer_GetSize(&buffer) != 1) return false;

    uint8_t popped = Buffer_Dequeue(&buffer);

    return popped == 10 && Buffer_GetSize(&buffer) == 0;
}

bool this_gets_set_to_true_by_buffer_callback = false;

void buffer_callback(buffer_t *buf) {
    this_gets_set_to_true_by_buffer_callback = true;
}

bool buffer_init_enqueue_callback() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    if (Buffer_GetSize(&buffer) != 0) return false;

    this_gets_set_to_true_by_buffer_callback = false;

    Buffer_SetCallback(&buffer, buffer_callback);

    Buffer_Enqueue(&buffer, 10);

    if (Buffer_GetSize(&buffer) != 1) return false;

    return this_gets_set_to_true_by_buffer_callback == true;
}

bool buffer_init_enqueue_callback_clear_callback() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    if (Buffer_GetSize(&buffer) != 0) return false;

    this_gets_set_to_true_by_buffer_callback = false;

    Buffer_SetCallback(&buffer, buffer_callback);

    Buffer_Enqueue(&buffer, 10);

    if (Buffer_GetSize(&buffer) != 1) return false;

    if (this_gets_set_to_true_by_buffer_callback != true) return false;

    Buffer_ClearCallback(&buffer);

    this_gets_set_to_true_by_buffer_callback = false;

    Buffer_Enqueue(&buffer, 14);

    return this_gets_set_to_true_by_buffer_callback == false;
}

bool buffer_init_enqueue_data() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    if (Buffer_GetSize(&buffer) != 0) return false;

    uint8_t data[4] = {1, 2, 3, 4};

    Buffer_EnqueueData(&buffer, data, 4);

    if (Buffer_GetSize(&buffer) != 4) return false;

    uint8_t popped1 = Buffer_Dequeue(&buffer);
    if (popped1 != 1 || Buffer_GetSize(&buffer) != 3) return false;

    uint8_t popped2 = Buffer_Dequeue(&buffer);
    if (popped2 != 2 || Buffer_GetSize(&buffer) != 2) return false;

    uint8_t popped3 = Buffer_Dequeue(&buffer);
    if (popped3 != 3 || Buffer_GetSize(&buffer) != 1) return false;

    uint8_t popped4 = Buffer_Dequeue(&buffer);
    if (popped4 != 4 || Buffer_GetSize(&buffer) != 0) return false;

    return true;
}

int main(int argc, char *argv[]) {
    printf("Buffer testing system initializing\n");

    test_t tests [NUMBER_TESTS] = {
        {
            this_test_passes,
            "This test passes",
            true
        },
        {
            this_test_fails,
            "This test fails",
            false
        },
        {
            buffer_init,
            "Buffer Init",
            true,
        },
        {
            buffer_init_enqueue,
            "Buffer Init and Enqueue",
            true,
        },
        {
            buffer_init_enqueue_dequeue,
            "Buffer Init, Enqueue, Dequeue",
            true,
        },
        {
            buffer_init_enqueue_callback,
            "Buffer Init, Enqueue, Callback",
            true,
        },
        {
            buffer_init_enqueue_callback_clear_callback,
            "Buffer Init, Enqueue, Callback, Clear Callback",
            true,
        },
        {
            buffer_init_enqueue_data,
            "Buffer Init, Enqueue Data",
            true,
        },
    };

    uint8_t tests_passed = 0;

    printf("Running %d tests\n\n", NUMBER_TESTS);

    for (uint8_t i = 0; i < NUMBER_TESTS; i++) {
        test_t test = tests[i];

        printf("%s: ", test.name);

        bool result = test.test_function();

        if (result == test.expected_result) {
            tests_passed += 1;
            printf("pass\n");
        } else {
            printf("fail\n");
        }
    }

    printf("\nOf the %d tests tested, %d test(s) passed and %d test(s) failed\n", NUMBER_TESTS, tests_passed, NUMBER_TESTS - tests_passed);

    return NUMBER_TESTS - tests_passed;
}

