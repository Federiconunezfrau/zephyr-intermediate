#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

// Every thread shares the same stack size
#define STACK_SIZE 256

// Priorities of the three preemptive threads. A lower number means higher priority
#define PRIORITY_LOW  7
#define PRIORITY_MED  5
#define PRIORITY_HIGH 3

// Priority of the cooperative task
#define PRIORITY_COOP -1

//Sleep time for each of the 3 threads
#define SLEEP_TIME_T_LOW_MS  300
#define SLEEP_TIME_T_MED_MS  200
#define SLEEP_TIME_T_HIGH_MS 100

// Function for thread t_low
void t_low_fn(void *p1, void *p2, void *p3) {
    while (1) {
        LOG_INF("T_LOW running");
        k_msleep(SLEEP_TIME_T_LOW_MS);
    }
}

// Function for thread t_med
void t_med_fn(void *p1, void *p2, void *p3) {
    while (1) {
        LOG_INF("T_MED running");
        k_msleep(SLEEP_TIME_T_MED_MS);
    }
}

// Function for thread t_high
void t_high_fn(void *p1, void *p2, void *p3) {
    while (1) {
        LOG_INF("T_HIGH running");
        k_msleep(SLEEP_TIME_T_HIGH_MS);
    }
}

// Function for cooperative thread t_coop: It prints the sam
void t_coop_fn(void *p1, void *p2, void *p3) {
    for(int i = 0; i<5; i++) {
        LOG_INF("T_COOP running, step %d", i+1);
        k_busy_wait(500000);
    }
    k_yield();
}

// The 3 preemptive threads are created at build time
K_THREAD_DEFINE(t_low , STACK_SIZE, t_low_fn , NULL, NULL, NULL, PRIORITY_LOW , 0, 0);
K_THREAD_DEFINE(t_med , STACK_SIZE, t_med_fn , NULL, NULL, NULL, PRIORITY_MED , 0, 0);
K_THREAD_DEFINE(t_high, STACK_SIZE, t_high_fn, NULL, NULL, NULL, PRIORITY_HIGH, 0, 0);
K_THREAD_DEFINE(t_coop, STACK_SIZE, t_coop_fn, NULL, NULL, NULL, PRIORITY_COOP, 0, 0);

int main(void) {
    return 0;
}

