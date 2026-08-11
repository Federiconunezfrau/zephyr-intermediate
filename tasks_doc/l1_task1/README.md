### Which thread runs most often?
Thread t_high runs most often.

### Does priority affect interleaving?
In order to answer this I ran 3 different cases, playing with the priorities and the sleep times. Here are the results:


CASE | PRIORITY_LOW | PRIORITY_MED | PRIORITY_HIGH | SLEEP_TIME_T_LOW_MS | SLEEP_TIME_T_MED_MS | SLEEP_TIME_T_HIGH_MS |
:---:|:------------:|:------------:|:-------------:|:-------------------:|:-------------------:|:--------------------:| 
1    | 7            |            5 |     3         | 300                 | 200                 | 100                  |
2    | 3            |            5 |     7         | 300                 | 200                 | 100                  |
3    | 7            |            5 |     3         | 100                 | 200                 | 300                  |

In cases 1 and 2 t_high is the one that runs most often, even though in case 2 it has the lowest priority of all. Also, by comparing logs_1.txt and logs_2.txt it can be seen that both sequence are equal except for the first calls to t_high, t_med and t_low.

In case 3 t_low was the one that ran most often and the sequence and interleaving of events was different (logs_3.txt).

I would say that it does affect interleaving but it is not the only thing that defines it.