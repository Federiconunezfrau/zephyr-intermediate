# Explain and reduce a Zephyr scheduling delay
## Run the provided producer-consumer application from branch l6-homework

The provided application was built and executed as is, building for native_sim. After running the application the following log was printed until it was stopped by calling CTLR+C:

```
uart connected to pseudotty: /dev/pts/3
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L6 Homework: Runtime Investigation ===
[00:00:00.000,000] <inf> homework: Control work must start within 10 ms
[00:00:00.000,000] <inf> homework: Inspect, measure, trace, explain, and correct the delay
[00:00:00.545,000] <inf> homework: [CONTROL] processed seq=0
[00:00:00.795,000] <inf> homework: [CONTROL] processed seq=1
[00:00:01.045,000] <inf> homework: [CONTROL] processed seq=2
[00:00:01.295,000] <inf> homework: [CONTROL] processed seq=3
[00:00:01.545,000] <inf> homework: [CONTROL] processed seq=4
[00:00:01.795,000] <inf> homework: [CONTROL] processed seq=5
[00:00:02.045,000] <inf> homework: [CONTROL] processed seq=6
[00:00:02.295,000] <inf> homework: [CONTROL] processed seq=7
[00:00:02.545,000] <inf> homework: [CONTROL] processed seq=8
[00:00:02.795,000] <inf> homework: [CONTROL] processed seq=9
[00:00:03.045,000] <inf> homework: [CONTROL] processed seq=10
[00:00:03.295,000] <inf> homework: [CONTROL] processed seq=11
[00:00:03.545,000] <inf> homework: [CONTROL] processed seq=12
[00:00:03.795,000] <inf> homework: [CONTROL] processed seq=13
[00:00:04.045,000] <inf> homework: [CONTROL] processed seq=14
```

## Define and detect its response-time guarantee

There is a log message entry generated at the `main()` function which says that the control work must start within 10 ms. By looking at the log's timestamps the control thread is called for the first time after 545 ms. After the first 500 ms have passed, timer started in the `main()` expires and the callback function is called for the first time, adding a message to the queue. After this, the semaphore is given and the callback returns. At this point the maintenance thread is changed to the running state, as it has higher priority than the control thread. This maintenance thread adds a 45 ms blocking delay. Only after this 45 ms is that the maintenance thread goes back to waiting, allowing the control thread to run and dequeue the message. **The 45 ms delay imposed by the maintenance thread is what is making the response-time guarantee fail.**

## Add sequence-based logs and deadline-miss counting

A new message entry was added to the log, inside the timer callback, in order to identify when the event is was added to the queue. Also a deadline-miss counting log was added in the control thread. In order to rate limit this log message the configuration in prj.conf, CONFIG_LOG_RATELIMIT_INTERVAL_MS, was used.

After adding this 2 modifications the following log was printed:

```
uart connected to pseudotty: /dev/pts/2
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L6 Homework: Runtime Investigation ===
[00:00:00.000,000] <inf> homework: Control work must start within 10 ms
[00:00:00.000,000] <inf> homework: Inspect, measure, trace, explain, and correct the delay
[00:00:00.500,000] <inf> homework: [PRODUCER] enqueued  seq=0
[00:00:00.545,000] <inf> homework: [CONTROL ] processed seq=0
[00:00:00.750,000] <inf> homework: [PRODUCER] enqueued  seq=1
[00:00:00.795,000] <inf> homework: [CONTROL ] processed seq=1
[00:00:01.000,000] <inf> homework: [PRODUCER] enqueued  seq=2
[00:00:01.045,000] <inf> homework: [CONTROL ] processed seq=2
[00:00:01.045,000] <wrn> homework: Skipped 2 messages
[00:00:01.045,000] <wrn> homework: deadline_miss count=3 seq=2 latency_ms=40
[00:00:01.250,000] <inf> homework: [PRODUCER] enqueued  seq=3
[00:00:01.295,000] <inf> homework: [CONTROL ] processed seq=3
[00:00:01.500,000] <inf> homework: [PRODUCER] enqueued  seq=4
[00:00:01.545,000] <inf> homework: [CONTROL ] processed seq=4
[00:00:01.750,000] <inf> homework: [PRODUCER] enqueued  seq=5
[00:00:01.795,000] <inf> homework: [CONTROL ] processed seq=5
[00:00:02.000,000] <inf> homework: [PRODUCER] enqueued  seq=6
[00:00:02.045,000] <inf> homework: [CONTROL ] processed seq=6
[00:00:02.045,000] <wrn> homework: Skipped 3 messages
[00:00:02.045,000] <wrn> homework: deadline_miss count=7 seq=6 latency_ms=40
[00:00:02.250,000] <inf> homework: [PRODUCER] enqueued  seq=7
[00:00:02.295,000] <inf> homework: [CONTROL ] processed seq=7
[00:00:02.500,000] <inf> homework: [PRODUCER] enqueued  seq=8
[00:00:02.545,000] <inf> homework: [CONTROL ] processed seq=8
[00:00:02.750,000] <inf> homework: [PRODUCER] enqueued  seq=9
[00:00:02.795,000] <inf> homework: [CONTROL ] processed seq=9
[00:00:03.000,000] <inf> homework: [PRODUCER] enqueued  seq=10
[00:00:03.045,000] <inf> homework: [CONTROL ] processed seq=10
[00:00:03.045,000] <wrn> homework: Skipped 3 messages
[00:00:03.045,000] <wrn> homework: deadline_miss count=11 seq=10 latency_ms=40
[00:00:03.250,000] <inf> homework: [PRODUCER] enqueued  seq=11
[00:00:03.295,000] <inf> homework: [CONTROL ] processed seq=11
```

## Inspect thread priorities and states with the kernel shell


## Capture CTF and decode it with Babeltrace
## Apply one correction and repeat the measurement
