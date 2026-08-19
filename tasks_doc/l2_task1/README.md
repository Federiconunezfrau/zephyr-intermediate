## Fixing the race condition with a mutex

Here are 2 different outputs I observed, without and with the mutex protection:

### Without the mutex protection

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: The expected final value is 2000000
[00:00:00.245,000] <inf> demo: Thread_A finished
[00:00:00.250,000] <inf> demo: Thread_B finished
[00:00:00.250,000] <inf> demo: The final value is 1159470
[00:00:00.250,000] <inf> demo: The total execution was: 250 ms

```
As was expected the final counter value is wrong.

### With the mutex protection
```
*** Booting Zephyr OS build v4.4.0 ***                  
[00:00:00.000,000] <inf> demo: The expected final value is 2000000
[00:00:51.961,000] <inf> demo: Thread_A finished        
[00:00:51.981,000] <inf> demo: Thread_B finished        
[00:00:51.981,000] <inf> demo: The final value is 2000000
[00:00:51.981,000] <inf> demo: The total execution was: 51981 ms

```

In this case the expected counter is correct, however at the expense of an enourmous execution time of approximately 1 minute!