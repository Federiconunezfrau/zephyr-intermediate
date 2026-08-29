#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE       2048
#define SENSOR_COUNT       10
#define SENSOR_PERIOD_MS  100
#define LOGGER_PERIOD_MS   10

/* ================================================================== */
/*  ZBus channel for sensor data                                      */
/* ================================================================== */
struct sensor_data {
    int32_t  temperature_mc;
    uint32_t timestamp_ms;
    uint8_t  seq;
};

ZBUS_CHAN_DEFINE(sensor_chan,                                         // The channel's name
    struct sensor_data,                                               // The message type for this channel
    NULL,                                                             // Validator not used here
    NULL,                                                             // User data not used here
    ZBUS_OBSERVERS(display_lis, logger_sub),                          // The observers list for this channel
    ZBUS_MSG_INIT(.temperature_mc = 0, .timestamp_ms = 0, .seq = 0)); // The message's initial value

/* ================================================================== */
/*  Display Listener                                                  */
/* ================================================================== */
static void display_listener_cb(const struct zbus_channel *chan) {
    const struct sensor_data *msg = (const struct sensor_data *)zbus_chan_const_msg(chan);

    LOG_INF("[DISPLAY-LIS] thread=%s seq=%u temp=%d mC t=%d ms", k_thread_name_get(k_current_get()), msg->seq, msg->temperature_mc, msg->timestamp_ms);
}

ZBUS_LISTENER_DEFINE(display_lis, // The listener's name
    display_listener_cb);         // The listener's callback function

/* ================================================================== */
/*  Logger Subscriber                                                 */
/* ================================================================== */
ZBUS_SUBSCRIBER_DEFINE(logger_sub, // The subscriber's name
    4);                            // The subscriber's notification queue size

/* ================================================================== */
/*  Publisher: sensor thread, publishes data on the ZBus channel
 *  every 100 ms.                                                     */
/* ================================================================== */
static void sensor_thread_fn(void *p1, void *p2, void *p3) {
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    k_thread_name_set(k_current_get(), "sensor");

    // The zbus publishing is performed SENSOR_COUNT number of times
    for (int i = 0; i < SENSOR_COUNT; i++) {

        // This message will be published on the ZBus channel
        struct sensor_data data = {
            .temperature_mc = 24000 + (i * 350),
            .timestamp_ms = k_uptime_get_32(),
            .seq = (uint8_t)i,
        };

        LOG_INF("[SENSOR] publish seq=%u temp=%d mC, timestamp t=%d ms", data.seq, data.temperature_mc, data.timestamp_ms);

        int ret = zbus_chan_pub(&sensor_chan, &data, K_MSEC(100));
        if (ret != 0) {
            LOG_WRN("[SENSOR] publish failed ret=%d", ret);
        }

        k_msleep(SENSOR_PERIOD_MS);
    }
    LOG_INF("[SENSOR] done");
}

/* ================================================================== */
/*  Message subscriber: logger thread                                 */
/* ================================================================== */
static void logger_thread_fn(void *p1, void *p2, void *p3) {
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    k_thread_name_set(k_current_get(), "logger");

    const struct zbus_channel *chan;
    struct sensor_data msg;
    int received = 0;

    while (received < SENSOR_COUNT) {

        // Blocks until a there is a new notification on the ZBus channel
        int ret = zbus_sub_wait(&logger_sub, &chan, K_MSEC(3000));
        if (ret != 0) {
            LOG_INF("[LOGGER-SUB] timeout, done");
            break;
        }

        // Gets a copy of the message on the ZBus channel to the msg local variable
        ret = zbus_chan_read(chan, &msg, K_MSEC(100));
        if (ret != 0) {
            LOG_WRN("[LOGGER-SUB] read failed ret=%d", ret);
            continue;
        }

        received++;
        LOG_INF("[LOGGER-SUB] thread=%s seq=%u temp=%d mC t=%d ms", k_thread_name_get(k_current_get()), msg.seq, msg.temperature_mc, msg.timestamp_ms);
    }
    LOG_INF("[LOGGER-SUB] done received=%d", received);
}

/* ================================================================== */
/*  Threads defined for this task                                     */
/* ================================================================== */
K_THREAD_DEFINE(sensor_thread, STACK_SIZE, sensor_thread_fn, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(logger_thread, STACK_SIZE, logger_thread_fn, NULL, NULL, NULL, 6, 0, 0);

/* ================================================================== */
/*  Main                                                              */
/* ================================================================== */
int main(void) {
    LOG_INF("=== L4 Task 1: Build a small event-driven system with ZBus ===");
    LOG_INF("sensor_thread publishes every %dms", SENSOR_PERIOD_MS);
    LOG_INF("display listener runs in publisher context");
    LOG_INF("logger uses a regular subscriber");

    return 0;
}
