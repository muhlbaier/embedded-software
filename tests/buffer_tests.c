#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"

#define NUMBER_TESTS 4

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

bool buffer_init_enque() {
    buffer_t buffer;
    uint8_t buffer_array[8];

    Buffer_Init(&buffer, buffer_array, 8);

    Buffer_Enqueue(&buffer, 10);

    return Buffer_GetSize(&buffer) == 1;
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
            buffer_init_enque,
            "Buffer Init and Enque",
            true,
        }
    };

    uint8_t tests_passed = 0;

    printf("Running %d tests\n", NUMBER_TESTS);

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

    printf("Of the %d tests tested, %d test(s) passed and %d tests failed\n", NUMBER_TESTS, tests_passed, NUMBER_TESTS - tests_passed);

    return NUMBER_TESTS - tests_passed;
}

