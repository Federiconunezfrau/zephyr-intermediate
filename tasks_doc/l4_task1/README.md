## Build a small event-driven system with zbus

In order to use ZBus the CONFIG_ZBUS was added to the prj.conf file.

By looking at the log it can be seen that every time the `sensor_thread` publishes an event, the `display_listener_cb` is called first, then the `logger_thread` receives the event from the channel and reads the message. This is expected behaviour, as the listeners are always called from the same context as the publisher, in this case the `sensor_thread`.

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: === L4 Task 1: Build a small event-driven system with ZBus ===
[00:00:00.000,000] <inf> demo: sensor_thread publishes every 100ms
[00:00:00.000,000] <inf> demo: display listener runs in publisher context
[00:00:00.000,000] <inf> demo: logger uses a regular subscriber                                      
[00:00:00.000,000] <inf> demo: [SENSOR] publish seq=0 temp=24000 mC, timestamp t=0 ms                
[00:00:00.000,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=0 temp=24000 mC t=0 ms                
[00:00:00.000,000] <inf> demo: [LOGGER-SUB] thread=logger seq=0 temp=24000 mC t=0 ms                 
[00:00:00.100,000] <inf> demo: [SENSOR] publish seq=1 temp=24350 mC, timestamp t=100 ms              
[00:00:00.100,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=1 temp=24350 mC t=100 ms              
[00:00:00.100,000] <inf> demo: [LOGGER-SUB] thread=logger seq=1 temp=24350 mC t=100 ms               
[00:00:00.200,000] <inf> demo: [SENSOR] publish seq=2 temp=24700 mC, timestamp t=200 ms              
[00:00:00.200,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=2 temp=24700 mC t=200 ms              
[00:00:00.200,000] <inf> demo: [LOGGER-SUB] thread=logger seq=2 temp=24700 mC t=200 ms               
[00:00:00.300,000] <inf> demo: [SENSOR] publish seq=3 temp=25050 mC, timestamp t=300 ms              
[00:00:00.300,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=3 temp=25050 mC t=300 ms              
[00:00:00.301,000] <inf> demo: [LOGGER-SUB] thread=logger seq=3 temp=25050 mC t=300 ms               
[00:00:00.401,000] <inf> demo: [SENSOR] publish seq=4 temp=25400 mC, timestamp t=401 ms              
[00:00:00.401,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=4 temp=25400 mC t=401 ms              
[00:00:00.401,000] <inf> demo: [LOGGER-SUB] thread=logger seq=4 temp=25400 mC t=401 ms               
[00:00:00.501,000] <inf> demo: [SENSOR] publish seq=5 temp=25750 mC, timestamp t=501 ms              
[00:00:00.501,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=5 temp=25750 mC t=501 ms              
[00:00:00.501,000] <inf> demo: [LOGGER-SUB] thread=logger seq=5 temp=25750 mC t=501 ms               
[00:00:00.601,000] <inf> demo: [SENSOR] publish seq=6 temp=26100 mC, timestamp t=601 ms
[00:00:00.601,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=6 temp=26100 mC t=601 ms
[00:00:00.601,000] <inf> demo: [LOGGER-SUB] thread=logger seq=6 temp=26100 mC t=601 ms
[00:00:00.701,000] <inf> demo: [SENSOR] publish seq=7 temp=26450 mC, timestamp t=701 ms
[00:00:00.701,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=7 temp=26450 mC t=701 ms
[00:00:00.701,000] <inf> demo: [LOGGER-SUB] thread=logger seq=7 temp=26450 mC t=701 ms
[00:00:00.801,000] <inf> demo: [SENSOR] publish seq=8 temp=26800 mC, timestamp t=801 ms
[00:00:00.801,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=8 temp=26800 mC t=801 ms
[00:00:00.802,000] <inf> demo: [LOGGER-SUB] thread=logger seq=8 temp=26800 mC t=801 ms
[00:00:00.902,000] <inf> demo: [SENSOR] publish seq=9 temp=27150 mC, timestamp t=902 ms
[00:00:00.902,000] <inf> demo: [DISPLAY-LIS] thread=sensor seq=9 temp=27150 mC t=902 ms
[00:00:00.902,000] <inf> demo: [LOGGER-SUB] thread=logger seq=9 temp=27150 mC t=902 ms
[00:00:00.902,000] <inf> demo: [LOGGER-SUB] done received=10
[00:00:01.002,000] <inf> demo: [SENSOR] done

```
