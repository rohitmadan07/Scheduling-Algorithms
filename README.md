# Scheduling-Algorithms
C++ implementation of various scheduling algorithms for operating systems. The following algorithms have been implemented:-
1. FCFS - First Come First Serve
2. SJF (with preemption) - Shortest Job First
3. Round-Robbin Algorithm - Giving fixed time quanta/slice (Which can be contolled in the program) to each process and then preempting

### Input Parameters
A text file which contains the following details regarding all the processes.
1. Arrival Time - Point of time at which the process enters the ready queue.
2. Execution Time - Total CPU time required by the process to complete it's execution.
3. CPU Burst Time - CPU time required by the process in one burst after which it can go for input-output (IO).
4. IO Block Time - Time for which a process is in the input-output state. It does not require the CPU duing this time.
5. Priority - Priority of the process which would be used when they ahve same arrival times (in Round-Robbin).

### Output
1. Process wise output with their updated next arrivals.
2. Turnaround times of each process.
3. Waiting times of each process.
