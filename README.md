# FairShareScheduler
A fair share scheduler implemented in C++

This was a simple assignment for OS class in Concordia University. I uploaded it since there are many examples of round robin
schedulers, etc... but I could not find an implementation of fair share scheduler.

We are simulating a single cpu, cingle core, single threaded system.
The scheduler schedules processes to run according to fair share scheduling policy.
For more information on what that is, google it.

This takes input file which stores data relating to the program.

I.e:

Input.txt
4
A 1
3 2
B 1
0 4
0 2
End of Input.txt

Reading the above input file will be interpreted as such:
Time quantum = 4
User A has 1 process
User A process 1 is released at time 3 and has execution time 2
User B has 2 processes
User B process 1 is released at time 0 and has execution time 4
User B process 1 is released at time 0 and has execution time 2

----------

The main function is executed at runtime and reads in the input file and parses it into respective data elements.
These are then passed by reference to the scheduler through a thread.
The scheduler schedules according to fair share policy and runs the processes through the "processor" simulated as a method.
The stack trace is printed to terminal output each time the processor processes a process.




