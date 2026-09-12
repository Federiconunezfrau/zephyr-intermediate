#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <stdalign.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE                2048
#define SENSOR_COUNT              10
#define PRODUCER_PERIOD_MS        100
#define HEALTH_CHECK_PERIOD_MS    200
#define K_MSGQ_DEPTH              10
#define TIME_CONSUMER_STUCK_S     1
#define RELOAD_TASK_WDT_PERIOD_MS 1000

/* ================================================================== */
/*  Data type and k_msgq used for communication between producer and  */
/*  consumer.                                                         */
/* ================================================================== */
struct data {
    uint32_t seq;
    int32_t  value;
    uint32_t timestamp_ms;
};

K_MSGQ_DEFINE(theMsgq, sizeof(struct data), K_MSGQ_DEPTH, alignof(struct data));

/* ================================================================== */
/*  Task watchdog callback: callback used by the producer thread,     */
/*  when registering a channel in the task watchdog.                  */
/*  PRODUCER_PERIOD_MS ms, up to a number of SENSOR_COUNT times.      */
/* ================================================================== */
static void producer_wdt_callback(int channel_id, void *user_data){
    LOG_INF("Task watchdog channel %d callback, thread: %s\n", channel_id, k_thread_name_get((k_tid_t)user_data));
}

/* ================================================================== */
/*  Producer: producer thread, publishes data on theMsgq every        */
/*  PRODUCER_PERIOD_MS ms, up to a number of SENSOR_COUNT times.      */
/* ================================================================== */
static void producer_thread_fn(void *p1, void *p2, void *p3) {

    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    // This message will be pushed to theMsgq
    struct data txData;

    // The producer thread adds a task watchdog channel for itself.
    // The watchdog is fed only when a txData is succesfully enqueued
    // in theMsgq. This is done here and not in the consumer because the
    // consumer could just wait indefinitely for a message that could never
    // arrive, which is ok, however this condition will trigger the task watchdog.
    int task_wdt_id = task_wdt_add(RELOAD_TASK_WDT_PERIOD_MS, producer_wdt_callback, (void *)k_current_get());

    // The producer pushes data to theMsq SENSOR_COUNT number of times
    for(uint32_t seq = 0; seq < SENSOR_COUNT ; seq++) {

        // Fills the new data to be pushed to theMsgq
        txData.seq = seq;
        txData.value = 100 + (int32_t)seq;
        txData.timestamp_ms = k_uptime_get_32();

        // Tries to push new data into theMsgq. It does not wait for
        // space to be available. In case it couldn't, a warning is
        // added to the log.
        if(k_msgq_put(&theMsgq, &txData, K_NO_WAIT) != 0) {
            LOG_WRN("[PRODUCER] queue full, dropped seq=%u", seq);
        }
        else {
            task_wdt_feed(task_wdt_id);
            LOG_DBG("[PRODUCER] transmitted seq=%u used=%u/%u", seq, k_msgq_num_used_get(&theMsgq), K_MSGQ_DEPTH);
        }

        // The thread goes to waiting for PRODUCER_PERIOD_MS ms
        k_msleep(PRODUCER_PERIOD_MS);
    }

    // The task watchdog is deleted
    task_wdt_delete(task_wdt_id);
    LOG_INF("[PRODUCER] done");
}

/* ================================================================== */
/*  Consumer: consumer thread, reads from theMsgq.                    */
/* ================================================================== */
static void consumer_thread_fn(void *p1, void *p2, void *p3) {

    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    struct data rxData;
    int received = 0;
    int ret;

    while (received < SENSOR_COUNT) {
        // Blocks here until new data is pushed to theMsgq. In case
        // ret != 0, an error is logged and it breaks out of the
        // while loop
        if((ret = k_msgq_get(&theMsgq, &rxData, K_FOREVER)) != 0) {
            LOG_ERR("[PROCESSOR] receive failed: %d", ret);
            break;
        }
        received++;
        LOG_DBG("[CONSUMER] received seq=%u, value=%d, timestamp=%u", rxData.seq, rxData.value, rxData.timestamp_ms);

        // This emulates a stucked consumer: After some values are correctly received, the thread
        // goes to sleep for a long time in which theMsgq will not be read
        if(received>=SENSOR_COUNT/2) {
            k_sleep(K_SECONDS(TIME_CONSUMER_STUCK_S));
        }
    }
    LOG_INF("[CONSUMER] done received=%d", received);
}

/* ================================================================== */
/*  Health check: checks theMsgq fill level, logs a warning at 75%.   */
/* ================================================================== */
static void healtch_check_thread_fn(void *p1, void *p2, void *p3) {

    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    uint32_t numUsed;

    while( (numUsed = k_msgq_num_used_get(&theMsgq)) <= (K_MSGQ_DEPTH * 3 / 4)) {
        k_msleep(HEALTH_CHECK_PERIOD_MS);
    }

    LOG_WRN("[HEALTH CHECK] theMsgq is at %d/%d", numUsed, K_MSGQ_DEPTH);
}

/* ================================================================== */
/*  Threads defined for this task:                                    */
/* 1) Producer thread: creates data and enqueues to a k_msgq          */
/* 2) Consumer thread: dequeues data from the k_msgq                  */
/* ================================================================== */
K_THREAD_DEFINE(producer_thread    , STACK_SIZE, producer_thread_fn     , NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(consumer_thread    , STACK_SIZE, consumer_thread_fn     , NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(health_check_thread, STACK_SIZE, healtch_check_thread_fn, NULL, NULL, NULL, 5, 0, 0);

/* ================================================================== */
/*  Main                                                              */
/* ================================================================== */
int main(void) {
    LOG_INF("=== L5 Task 1: Study reliability under pressure ===");
    LOG_INF("The producer_thread publishes every %dms", PRODUCER_PERIOD_MS);
    LOG_INF("The consumer thread reads everytime data is available on theMsgq");

    // Initializes the task watchdog. This is called from the main thread
    // as it is guaranteed that it has a higher priority than the other
    // threads (the main thread's priority is 0)
    task_wdt_init(NULL);

    return 0;
}
