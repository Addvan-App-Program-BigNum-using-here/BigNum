#include "bench_util.h"

void print_data_set(){
    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n-----------------------------------------\n");
}

void printProgressBar(float percentage, int width){
    const char* blocks[] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
    int pos = (int)((percentage / 100.0f) * width * 8);
    int full_blocks = pos / 8;
    int remainder = pos % 8;

    printf("\r[");

    for (int i = 0; i < full_blocks; i++) {
        printf("%s", blocks[8]);
    }

    if (full_blocks < width) {
        printf("%s", blocks[remainder]);

        for (int i = full_blocks + 1; i < width; i++) {
            printf("%s", blocks[0]);
        }
    }

    printf("] %.1f%%", percentage);
    fflush(stdout);
}
