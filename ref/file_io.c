#include "file_io.h"

msg Test_file_write(char* str, int option){
    FILE* fp = (option == CLEAR) ? fopen(Test_file, "w") : fopen(Test_file, "a");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return FILE_WRITE_FAIL;
    }

    fprintf(fp, "%s\n", str);

    fclose(fp);

    return FILE_WRITE_SUCCESS;
}