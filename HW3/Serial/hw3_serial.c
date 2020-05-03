#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <time.h>

//32 bits, -2147483647 ~ 2147483647
#define MAX_INT_CHAR_SIZE 11
//maximum number of integers in one testcase in argv[1]
#define MAX_INT_EACH_TESTCASE 10000
//assume max 10000 testcases in argv[1]
#define MAX_TESTCASES 10000
//maximum char size in one line of argv[1]
#define MAX_SIZE_EACH_TESTCASE MAX_INT_CHAR_SIZE*MAX_INT_EACH_TESTCASE

int  input_int_buff[MAX_TESTCASES][MAX_INT_EACH_TESTCASE];
int  output_int_buff[MAX_TESTCASES][MAX_INT_EACH_TESTCASE];

void PrintHelp(){
    printf("\nError: This hw3.o takes two input arguments.\n");
    printf("       Please use as the format: ./hw3.o [arg1] [arg1]\n");
    printf("       [arg1]: The input file name, e.g. testcase.txt, which contains lines of numbers to be sorted.\n");
    printf("       [arg2]: The output file name which contains the sorted restuls of each line in [arg1].\n");
}

void MoveArray(const int* from_arr, int* to_arr, const int to_size, const int to_begin_index, const int from_begin_index){
    int index = to_begin_index;
    for(int i=from_begin_index; i<from_begin_index+to_size; ++i){
        to_arr[index] = from_arr[i];
        ++index;
    }
}

void Merge2(const int low, const int mid, const int high, int* S){
    int i = low;
    int j = mid + 1;
    int k = 0;
    int *u = malloc(sizeof(int)*(high-low+1));

    while((i<=mid) && (j<=high)){
        if(S[i]<S[j]){
            u[k] = S[i];
            ++i;
        }else{
            u[k] = S[j];
            ++j;
        }
        ++k;
    }
    if(i > mid){
        MoveArray(S, u, (high - j + 1), k, j);
    }else{
        MoveArray(S, u, (mid - i + 1), k, i);
    }
    MoveArray(u, S, (high - low + 1), low, 0);

    free(u);
}

void MergeSort2(const int low, const int high, int* S){
    int mid;

    if(low < high){
        mid = floor((low+high)/2);
        MergeSort2(low, mid, S);
        MergeSort2(mid+1, high, S);
        Merge2(low, mid, high, S);
    }
}

int main(int argc, char* argv[]){
    char* input_file_name;
    char* output_file_name;
    FILE* fp_read;
    FILE* fp_write;
    ssize_t read_ind;
    size_t line_len = 0;
    char* line;
    char* piece_char;
    int line_count = 0;
    int int_count[MAX_INT_EACH_TESTCASE] = {0};
    double time_count[MAX_INT_EACH_TESTCASE] = {0.f};
    const char* err_str;

    if(argc != 3){
        PrintHelp();
        return EXIT_FAILURE;
    }

    /* -------------------I/O Processing------------------*/
    input_file_name = argv[1];
    output_file_name = argv[2];

    fp_read = fopen(input_file_name, "r");
    if(fp_read == NULL){
        printf("\nError: cannot read %s \n", input_file_name);
        return EXIT_FAILURE;
    }

    while((read_ind=getline(&line, &line_len, fp_read)) != -1 ){
        if(line_count > MAX_TESTCASES-1){
            printf("\nError: The number of testcase in %s exceeds the limit: MAX_TESTCASES\n", input_file_name);
            return EXIT_FAILURE;
        }

        //printf("line = %s", line);
        piece_char = strtok(line, " \n");
        if(piece_char != NULL){
            input_int_buff[line_count][int_count[line_count]] = strtol(piece_char, NULL, 10);
        }
        if(errno == ERANGE){
            printf("Error: range error.\n");
        }
        while(piece_char != NULL){
            int_count[line_count]++;
            piece_char = strtok(NULL, " \n");
            if(piece_char != NULL){
                input_int_buff[line_count][int_count[line_count]] = strtol(piece_char, NULL, 10);
            }
            if(errno == ERANGE){
                printf("Error: range error.\n");
            }
        }

        for(int i=0;i<int_count[line_count];++i){
            if(i == int_count[line_count]-1){
                printf("%d\n", input_int_buff[line_count][i]);
            }else{
                printf("%d, ", input_int_buff[line_count][i]);
            }
        }
        //printf("---------------\n");
        line_count++;
    }

    /* -------------------MergeSort Processing------------------*/
    for(int i=0;i<line_count;++i){//for each testcase
        memcpy(output_int_buff[i], input_int_buff[i], int_count[i]*sizeof(int));
        clock_t begin = clock();
        MergeSort2(0, int_count[i]-1, output_int_buff[i]);
        clock_t end = clock();
        time_count[i] = (end-begin)/(double)CLOCKS_PER_SEC;
    }
    /* -------------------I/O Processing------------------*/
    fp_write = fopen(output_file_name, "w");
    if(fp_write == NULL){
        printf("\nError: cannot write %s \n", output_file_name);
        return EXIT_FAILURE;
    }

    for(int i=0;i<line_count;++i){
        for(int j=0;j<int_count[i];++j){
            if(j==int_count[i]-1){
                fprintf(fp_write, "%d\n", output_int_buff[i][j]);
            }else{
                fprintf(fp_write, "%d ", output_int_buff[i][j]);
            }
        }
        fprintf(fp_write, "duration:%lf\n", time_count[i]);
        fprintf(fp_write, "\n");
    }

    fclose(fp_read);
    fclose(fp_write);
    return EXIT_SUCCESS;
}
