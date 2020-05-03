#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

//maximum number of integers in one testcase in argv[1]
#define MAX_INT_EACH_TESTCASE 10000
//assume max 10000 testcases in argv[1]
#define MAX_TESTCASES 10000

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

int  input_int_buff[MAX_TESTCASES][MAX_INT_EACH_TESTCASE];
int  output_int_buff[MAX_TESTCASES][MAX_INT_EACH_TESTCASE];

typedef struct param_struc {
    int low;
    int high;
    int row_num;
} param_struc;


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

void Merge2Thread(const int low, const int mid, const int high, const int row_num){
    int i = low;
    int j = mid + 1;
    int k = 0;
    int *u = malloc(sizeof(int)*(high-low+1));

    while((i<=mid) && (j<=high)){
        if(output_int_buff[row_num][i]<output_int_buff[row_num][j]){
            u[k] = output_int_buff[row_num][i];
            ++i;
        }else{
            u[k] = output_int_buff[row_num][j];
            ++j;
        }
        ++k;
    }
    if(i > mid){
        MoveArray(output_int_buff[row_num], u, (high - j + 1), k, j);
    }else{
        MoveArray(output_int_buff[row_num], u, (mid - i + 1), k, i);
    }
    MoveArray(u, output_int_buff[row_num], (high - low + 1), low, 0);

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

void* MergeSort2ThreadCall(void* param_in){
    int mid;
    int low;
    int high;
    int row_num;

    param_struc* param = (param_struc*) param_in;
    low = param->low;
    high = param->high;
    row_num = param->row_num;

    if(low < high){
        mid = floor((low+high)/2);
        MergeSort2(low, mid, output_int_buff[row_num]);
        MergeSort2(mid+1, high, output_int_buff[row_num]);
        Merge2(low, mid, high, output_int_buff[row_num]);
    }
}

void* MergeSort2Thread(void* param_in){
    int mid;
    int low;
    int high;
    int row_num;
    param_struc param_in1;
    param_struc param_in2;
    pthread_t thread1, thread2;
    int thread1_ret, thread2_ret;

    param_struc* param = (param_struc*) param_in;
    low = param->low;
    high = param->high;
    row_num = param->row_num;

    if(low < high){
        mid = floor((low+high)/2);
        param_in1.low = low;
        param_in1.high = mid;
        param_in1.row_num = row_num;
        param_in2.low = mid+1;
        param_in2.high = high;
        param_in2.row_num = row_num;

        thread1_ret = pthread_create(&thread1, NULL, MergeSort2ThreadCall, (void*) &param_in1);
        if(thread1_ret != 0){
            handle_error_en(thread1_ret, "1st pthread_create in MergeSort2ThreadCall fails.");
        }

        thread2_ret = pthread_create(&thread2, NULL, MergeSort2ThreadCall, (void*) &param_in2);
        if(thread2_ret != 0){
            handle_error_en(thread2_ret, "2nd pthread_create in MergeSort2ThreadCall fails.");
        }

        thread1_ret = pthread_join(thread1, NULL);
        if(thread1_ret != 0){
            handle_error_en(thread1_ret, "1st pthread_join in MergeSort2ThreadCall fails.");
        }

        thread2_ret = pthread_join(thread2, NULL);
        if(thread2_ret != 0){
            handle_error_en(thread2_ret, "2nd pthread_join in MergeSort2ThreadCall fails.");
        }

        Merge2(low, mid, high, output_int_buff[row_num]);
        //Merge2Thread(low, mid, high, row_num);
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
    int thread_ret;
    pthread_t thread_handle;
    param_struc param_in;

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

        /*
        for(int i=0;i<int_count[line_count];++i){
            if(i == int_count[line_count]-1){
                printf("%d\n", input_int_buff[line_count][i]);
            }else{
                printf("%d, ", input_int_buff[line_count][i]);
            }
        }
        */
        //printf("---------------\n");
        line_count++;
    }

    /* -------------------MergeSort Processing------------------*/
    for(int i=0;i<line_count;++i){//for each testcase
        memcpy(output_int_buff[i], input_int_buff[i], int_count[i]*sizeof(int));
        clock_t begin = clock();
        param_in.low = 0;
        param_in.high = int_count[i]-1;
        param_in.row_num = i;

        thread_ret = pthread_create(&thread_handle, NULL, MergeSort2Thread, (void*) &param_in);
        if(thread_ret != 0){
            handle_error_en(thread_ret, "pthread_create in main fails.");
        }

        thread_ret = pthread_join(thread_handle, NULL);
        if(thread_ret != 0){
            handle_error_en(thread_ret, "pthread_join in main fails.");
        }
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
