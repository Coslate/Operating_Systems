#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

const int phil_num = 5;
enum { thinking, hungry, eating } state[5];
pthread_mutex_t mutex;
pthread_cond_t self[5];

void test(int philosopher_number){
    pthread_mutex_lock(&mutex);
    if((state[(philosopher_number+phil_num-1)%phil_num]!=eating) && (state[philosopher_number]==hungry) && (state[(philosopher_number+1)%phil_num]!=eating)){
        state[philosopher_number] = eating;
        pthread_cond_signal(&self[philosopher_number]);
    }
    pthread_mutex_unlock(&mutex);
}

void pickup_forks(int philosopher_number){
    pthread_mutex_lock(&mutex);
    state[philosopher_number] = hungry;
    pthread_mutex_unlock(&mutex);

    test(philosopher_number);
    pthread_mutex_lock(&mutex);
        if(state[philosopher_number]!=eating){
            printf("Philosopher %d can't pick up forks and start waiting.\n", philosopher_number);
            pthread_cond_wait(&self[philosopher_number], &mutex);
        }
    pthread_mutex_unlock(&mutex);
}

void return_forks(int philosopher_number){
    int left_id = (philosopher_number+phil_num-1)%phil_num;
    int right_id = (philosopher_number+1)%phil_num;

    printf("Philosopher %d returns forks and then starts TESTING %d and %d.\n", philosopher_number, left_id, right_id);
    pthread_mutex_lock(&mutex);
        state[philosopher_number] = thinking;
    pthread_mutex_unlock(&mutex);
    
    test(left_id);
    test(right_id);
}

void* philosophers(void *n){
    int id = *(int*)n;
    int rand_time_think = (rand())%4;
    int rand_time_eat = (rand())%4;

    //thinking
    printf("Philosopher %d is now THINKING for %d seconds.\n", id, rand_time_think);
    sleep(rand_time_think);
    //become hungry
    printf("Philosopher %d is now HUNGRY and trying to pick up forks.\n", id);
    pickup_forks(id);
    //start eating
    printf("Philosopher %d is now EATING.\n", id);
    sleep(rand_time_eat);
    //end eating
    return_forks(id);
}

int main(){
    //Initialization
    pthread_t *thread_handles = malloc(sizeof(pthread_t)*phil_num);
    pthread_mutex_init(&mutex, NULL);
    int* id_arr = calloc(phil_num, sizeof(int));
    for(int i=0;i<phil_num;++i){
        state[i] = thinking;
        pthread_cond_init(&self[i], NULL);
        //printf("state[%d] = %d\n", i, state[i]);
    }

    //Thread Creation
    for(int i=0;i<phil_num;++i){
        id_arr[i] = i;
        pthread_create(&thread_handles[i], NULL, philosophers, (void *)(&id_arr[i]));
    }

    //Thread Termination
    for(int i=0;i<phil_num;++i){
        pthread_join(thread_handles[i], NULL);
    }
    
    free(thread_handles); 
    free(id_arr); 
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}
