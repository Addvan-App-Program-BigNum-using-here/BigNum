#include "file_io.h"

/*************************************************
* Name:        Test_file_write
*
* Description: File write string
*
* Arguments:   - char* str: save string
*              - int option: CLEAR or APPEND (CLEAR => clear file, APPEND => append file)
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg Test_file_write(IN char* Test_file, IN char* str, const IN int option){
    FILE* fp = (option == CLEAR) ? fopen(Test_file, "w") : fopen(Test_file, "a");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return FILE_WRITE_FAIL;
    }

    fprintf(fp, "%s\n", str);
    fclose(fp);
    return FILE_WRITE_SUCCESS;
}

/*************************************************
* Name:        Test_file_write_non_enter
*
* Description: File write string whitout enter
*
* Arguments:   - char* str: save string
*              - int option: CLEAR or APPEND (CLEAR => clear file, APPEND => append file)
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg Test_file_write_non_enter(IN char* Test_file, IN char *str, const IN int option){
    FILE *fp = (option == CLEAR) ? fopen(Test_file, "w") : fopen(Test_file, "a");
    if (fp == NULL){
        printf("Failed to run command\n");
        return FILE_WRITE_FAIL;
    }
    fprintf(fp, "%s", str);
    fclose(fp);
    return FILE_WRITE_SUCCESS;
}