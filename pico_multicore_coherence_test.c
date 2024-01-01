#include <stdio.h>
#include <stdlib.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#define ARRAY_WIDTH (32)
#define ARRAY_HEIGHT (32)
#define ARRAY_BYTE_COUNT (ARRAY_HEIGHT * ARRAY_WIDTH)
#define ARRAY_HALF_WIDTH (ARRAY_WIDTH / 2)

// How many bad values to print per test.
#define PRINT_ERROR_MAX (4)

// Writes values into an 2D array by sweeping down each column, rather than the
// more common approach of working across each row, for the purposes of
// reproducing a bug in a more complex algorithm.
void write_coords_row_wise(uint8_t* array, int width, int height, int start_x, int end_x) {
    for (int x = start_x; x < end_x; ++x) {
        for (int y = 0; y < height; ++y) {
            uint8_t* dst = array + (y * width) + x;
            *dst = (x + y) & 0xff;
        }
    }
}

int check_coords(const uint8_t* array, int width, int height) {
    int error_count = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const uint8_t expected = (x + y) & 0xff;
            const uint8_t* addr = array + (y * width) + x;
            const uint8_t actual = *addr;
            if (expected != actual) {
                error_count += 1;
                if (error_count <= PRINT_ERROR_MAX) {
                    printf("Error at (%d, %d): Expected %d but got %d\n", x, y, expected, actual);
                }
            }
        }
    }
    if (error_count == 0) {
        printf("No errors found\n");
    } else {
        printf("%d errors found\n", error_count);
    }
}

void test_single_core() {
    // Allocate a zero-initialized array on the stack.
    uint8_t* array[ARRAY_BYTE_COUNT] = {};

    // Fill in the array in two halves, but sequentially.
    write_coords_row_wise(array, ARRAY_WIDTH, ARRAY_HEIGHT, 0, ARRAY_HALF_WIDTH);
    write_coords_row_wise(array, ARRAY_WIDTH, ARRAY_HEIGHT, ARRAY_HALF_WIDTH, ARRAY_WIDTH);

    // Make sure the array contains the expected values.
    check_coords(array, ARRAY_WIDTH, ARRAY_HEIGHT);
}

uint8_t* g_array = NULL;

void multi_core_worker(void) {
    write_coords_row_wise(g_array, ARRAY_WIDTH, ARRAY_HEIGHT, ARRAY_HALF_WIDTH, ARRAY_WIDTH);
    multicore_fifo_push_blocking(0);
}

void test_multi_core() {
    // Allocate a zero-initialized array on the stack.
    uint8_t array[ARRAY_BYTE_COUNT] = {};

    multicore_reset_core1();
    // Fill in the second half of the array on the second core.
    g_array = array;
    multicore_launch_core1(multi_core_worker);

    // Fill in the first half of the array on the first core.
    write_coords_row_wise(array, ARRAY_WIDTH, ARRAY_HEIGHT, 0, ARRAY_HALF_WIDTH);

    // A result of 0 means success. Blocks until core 1 is done.
    const uint32_t core1_result = multicore_fifo_pop_blocking();
    if (core1_result != 0) {
        printf("Multicore processing failed.\n");
    }

    // Make sure the array contains the expected values.
    check_coords(array, ARRAY_WIDTH, ARRAY_HEIGHT);
}


int main(int argc, char* argv[]) {
    stdio_init_all();

    while (true) {
        printf("Testing single core sequential processing.\n");
        test_single_core();

        printf("Testing multi core parallel processing.\n");
        test_multi_core();

        sleep_ms(1000);
    }

    return 0;
}