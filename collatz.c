#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/shm.h>

//Global variables
#define SIZE 4096
#define NAME "/shmem-collatz"

int main(int argc, char * argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "No argument provided.\n");
		return 1;
	}
	else if(atoi(argv[1]) <= 0)
	{
		fprintf(stderr, "The argument provided was not a positive integer.\n");
		return 1;
	}
	else
	{
		//Fork
		pid_t pid;
		pid = fork();
		
		//Error has occurred while forking
		if (pid < 0)
		{
			fprintf(stderr, "Fork has failed.\n");
			return 1;
		}
		//Parent Process
		else if (pid > 0)
		{
			printf("Parent process start.\n");
			
			//Wait for child process
			wait(NULL);
			
			//Open shared memory location
			int fd = shm_open(NAME, O_RDONLY, 0666);
			if (fd < 0) 
			{
				perror("shm_open()");
				return EXIT_FAILURE;
			}
			
			//Obtain and print the data from the mapped file in memory
			void * data = (int *)mmap(0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
			printf("%s\n", (char *)data);
			
			//Unmap memory and close and unlink shared memory location
			munmap(data, SIZE);
			close(fd);
			shm_unlink(NAME);
			
			printf("Parent process end.\n");
			
			return EXIT_SUCCESS;
		}
		//Child Process
		else
		{
			printf("Child process start.\n");
			
			//Open shared memory location
			int fd = shm_open(NAME, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
			if (fd < 0)
			{
				perror("shm_open()");
				return EXIT_FAILURE;
			}
			
			//Set the shared memory location to specified size and map memory
			ftruncate(fd, SIZE);
			void * data = (int *)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
			
			//Convert input from string to int
			int n = atoi(argv[1]);
			
			while(n != 1)
			{
				//Print to the shared memory location
				sprintf(data + strlen(data), "%i\n", n);
				
				//Collatz calculation
				if(n % 2 == 0)
				{
					n = n / 2;
				}
				else
				{
					n = 3 * n + 1;
				}
			}
			
			//Print to the shared memory location
			sprintf(data + strlen(data), "%i", n);
			
			//Unmap memory and close shared memory location
			munmap(data, SIZE);
			close(fd);
			
			printf("Child process end.\n");
			return EXIT_SUCCESS;
		}
	}
}