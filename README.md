# Collatz-Conjecture
A program, developed in C, that implements the Collatz Conjecture.

The program accepts input the command line in this format: ./collatz integer

The program first forks, the parent process then waits for the child process to end. The child process then opens up a shared memory location. The Collatz calculations are then applied to the user's input and prints the output to the shared memory location, then closes the shared memory location. 

Once the child finishes, the parent process opens the shared memory location again, reads the data from the shared memory location, then prints that to the console. Finally, the parent process unmaps the memory and closes the shared memory location.
