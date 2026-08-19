#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

// Every thread shares the same stack size
#define STACK_SIZE 1024

// How many times each thread increments the shared counter
#define NUMBER_OF_INCREMENTS 1000000

// Priorities of the 2 preemptive threads.
#define PRIORITY 5

// Counter variable used
static volatile int counter = 0;

// Semaphore for waiting until both threads finish before printing out the result
static K_SEM_DEFINE(sem, 0, 2);

// The mutex used for protecting the shared data, the counter variable
static K_MUTEX_DEFINE(mut); 

// Thread function for both threads
void thread_fn(void *p1, void *p2, void *p3) {

    const char *threadName = k_thread_name_get(k_current_get());
    
    for(int i = 0; i < NUMBER_OF_INCREMENTS; i++) {
        k_mutex_lock(&mut, K_FOREVER);
        counter++;
        k_mutex_unlock(&mut);
    }

    LOG_INF("%s finished", threadName);
    
    k_sem_give(&sem);
}

// The 2 preemptive threads are created
K_THREAD_DEFINE(Thread_A , STACK_SIZE, thread_fn , NULL, NULL, NULL, PRIORITY , 0, 0);
K_THREAD_DEFINE(Thread_B , STACK_SIZE, thread_fn , NULL, NULL, NULL, PRIORITY , 0, 0);

int main(void) {
    
    // Gets the execution time until now, in miliseconds
    int64_t time = k_uptime_get();

    // Prints the expected final value which is double the times the number of increments
    LOG_INF("The expected final value is %d", NUMBER_OF_INCREMENTS * 2);

    // Waits until both threads finish
    k_sem_take(&sem, K_FOREVER);
    k_sem_take(&sem, K_FOREVER);

    // Prints the final value
    LOG_INF("The final value is %d", counter);

    // Prints the total execution time. Gets the elapsed time in ms since
    // the call to k_uptime_get()
    LOG_INF("The total execution was: %lld ms", k_uptime_delta(&time));

    return 0;
}