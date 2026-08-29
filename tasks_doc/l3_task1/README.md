## Run the starter and count wake-ups in the log
After running the starter code as is, I observed that the `polling_thread` wakes up a total amount of 100 times. From those 100 times, only 10 times the polling thread actually detects `sensor_flag == true` and prints its message in the log.

Here is a copy-paste of the log printed:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L3 Homework: Polling to Workqueue ===
[00:00:00.000,000] <inf> homework: Starter: polling every 10ms, sensor fires every 100ms
[00:00:00.000,000] <inf> homework: Expected wasted wakeups: ~9 per event
[00:00:00.000,000] <inf> homework: Run this, count wakeups, then convert to workqueue.
[00:00:00.100,000] <inf> homework: [SENSOR] event 0  tick=100
[00:00:00.101,000] <inf> homework: [CONSUMER] processed event 1  wakeups_so_far=10  tick=101
[00:00:00.200,000] <inf> homework: [SENSOR] event 1  tick=200
[00:00:00.202,000] <inf> homework: [CONSUMER] processed event 2  wakeups_so_far=20  tick=202
[00:00:00.300,000] <inf> homework: [SENSOR] event 2  tick=300
[00:00:00.303,000] <inf> homework: [CONSUMER] processed event 3  wakeups_so_far=30  tick=303
[00:00:00.400,000] <inf> homework: [SENSOR] event 3  tick=400
[00:00:00.404,000] <inf> homework: [CONSUMER] processed event 4  wakeups_so_far=40  tick=404
[00:00:00.500,000] <inf> homework: [SENSOR] event 4  tick=500
[00:00:00.505,000] <inf> homework: [CONSUMER] processed event 5  wakeups_so_far=50  tick=505
[00:00:00.600,000] <inf> homework: [SENSOR] event 5  tick=600
[00:00:00.606,000] <inf> homework: [CONSUMER] processed event 6  wakeups_so_far=60  tick=606
[00:00:00.700,000] <inf> homework: [SENSOR] event 6  tick=700
[00:00:00.707,000] <inf> homework: [CONSUMER] processed event 7  wakeups_so_far=70  tick=707
[00:00:00.801,000] <inf> homework: [SENSOR] event 7  tick=801
[00:00:00.808,000] <inf> homework: [CONSUMER] processed event 8  wakeups_so_far=80  tick=808
[00:00:00.901,000] <inf> homework: [SENSOR] event 8  tick=901
[00:00:00.909,000] <inf> homework: [CONSUMER] processed event 9  wakeups_so_far=90  tick=909
[00:00:01.001,000] <inf> homework: [SENSOR] event 9  tick=1001
[00:00:01.001,000] <inf> homework: [SENSOR] all events produced
[00:00:01.010,000] <inf> homework: [CONSUMER] processed event 10  wakeups_so_far=100  tick=1010
[00:00:01.010,000] <inf> homework: 

[00:00:01.010,000] <inf> homework: [SUMMARY] events=10  total_wakeups=100  wasted=90
[00:00:01.010,000] <inf> homework: [SUMMARY] wasted wakeups = 90% of all wakeups

```
## Confirm the handler runs only on real events
After removing the polling thread and replacing it with the work item submit to the system workqueue, the log shows that the `sensor_handler` is called only when the `sensor_sim` thread fires. This can be verified by looking at the ticks count printed by the `sensor_handler` and the `sensor_sim thread`. On each log entry both tick counts have the same value.

Here is a copy-paste of the log printed:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L3 Homework: Polling to Workqueue ===
[00:00:00.100,000] <inf> homework: [SENSOR] event 0  tick=100
[00:00:00.100,000] <inf> homework: [HANDLER] processed event 1  tick=100
[00:00:00.200,000] <inf> homework: [SENSOR] event 1  tick=200
[00:00:00.200,000] <inf> homework: [HANDLER] processed event 2  tick=200
[00:00:00.300,000] <inf> homework: [SENSOR] event 2  tick=300
[00:00:00.300,000] <inf> homework: [HANDLER] processed event 3  tick=300
[00:00:00.400,000] <inf> homework: [SENSOR] event 3  tick=400
[00:00:00.400,000] <inf> homework: [HANDLER] processed event 4  tick=400
[00:00:00.501,000] <inf> homework: [SENSOR] event 4  tick=501
[00:00:00.501,000] <inf> homework: [HANDLER] processed event 5  tick=501
[00:00:00.601,000] <inf> homework: [SENSOR] event 5  tick=601
[00:00:00.601,000] <inf> homework: [HANDLER] processed event 6  tick=601
[00:00:00.701,000] <inf> homework: [SENSOR] event 6  tick=701
[00:00:00.701,000] <inf> homework: [HANDLER] processed event 7  tick=701
[00:00:00.801,000] <inf> homework: [SENSOR] event 7  tick=801
[00:00:00.801,000] <inf> homework: [HANDLER] processed event 8  tick=801
[00:00:00.901,000] <inf> homework: [SENSOR] event 8  tick=901
[00:00:00.901,000] <inf> homework: [HANDLER] processed event 9  tick=901
[00:00:01.002,000] <inf> homework: [SENSOR] event 9  tick=1002
[00:00:01.002,000] <inf> homework: [HANDLER] processed event 10  tick=1002
[00:00:01.002,000] <inf> homework: [SENSOR] all events produced

```
## Bonus: debounce five events within 20 ms using k_work_reschedule ##

For the bonus debounce task a delayable work item was created, `dwork_sensor`. The same `sensor_handler` was assigned as the handler. The reschedule of the deleyable work item was added in the same `sensor_sim` thread, just after the thread finishes with the 10 calls to `k_work_submit()`. 

By looking at the log it can be seen that the `sensor_sim` thread reschedules the delayable work item `dwork_sensor` 5 times, once every 20 ms, and only after a 30 ms silence is that the `sensor_handler()` is called just once.

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L3 Homework: Polling to Workqueue ===
[00:00:00.100,000] <inf> homework: [SENSOR] event 0  tick=100
[00:00:00.100,000] <inf> homework: [HANDLER] processed event 1  tick=100
[00:00:00.200,000] <inf> homework: [SENSOR] event 1  tick=200
[00:00:00.200,000] <inf> homework: [HANDLER] processed event 2  tick=200
[00:00:00.300,000] <inf> homework: [SENSOR] event 2  tick=300
[00:00:00.300,000] <inf> homework: [HANDLER] processed event 3  tick=300
[00:00:00.400,000] <inf> homework: [SENSOR] event 3  tick=400
[00:00:00.400,000] <inf> homework: [HANDLER] processed event 4  tick=400
[00:00:00.501,000] <inf> homework: [SENSOR] event 4  tick=501
[00:00:00.501,000] <inf> homework: [HANDLER] processed event 5  tick=501
[00:00:00.601,000] <inf> homework: [SENSOR] event 5  tick=601
[00:00:00.601,000] <inf> homework: [HANDLER] processed event 6  tick=601
[00:00:00.701,000] <inf> homework: [SENSOR] event 6  tick=701
[00:00:00.701,000] <inf> homework: [HANDLER] processed event 7  tick=701
[00:00:00.801,000] <inf> homework: [SENSOR] event 7  tick=801
[00:00:00.801,000] <inf> homework: [HANDLER] processed event 8  tick=801
[00:00:00.901,000] <inf> homework: [SENSOR] event 8  tick=901
[00:00:00.901,000] <inf> homework: [HANDLER] processed event 9  tick=901
[00:00:01.002,000] <inf> homework: [SENSOR] event 9  tick=1002
[00:00:01.002,000] <inf> homework: [HANDLER] processed event 10  tick=1002
[00:00:01.002,000] <inf> homework: [SENSOR] all events produced
[00:00:01.002,000] <inf> homework:  
[00:00:01.002,000] <inf> homework: [SENSOR] Event debouncing: rescheduled every 20ms with a delay of 30 ms
[00:00:01.022,000] <inf> homework: [SENSOR] reschedule 0  tick=1022
[00:00:01.042,000] <inf> homework: [SENSOR] reschedule 1  tick=1042
[00:00:01.062,000] <inf> homework: [SENSOR] reschedule 2  tick=1062
[00:00:01.082,000] <inf> homework: [SENSOR] reschedule 3  tick=1082
[00:00:01.102,000] <inf> homework: [SENSOR] reschedule 4  tick=1102
[00:00:01.132,000] <inf> homework: [HANDLER] processed event 11  tick=1132

```
