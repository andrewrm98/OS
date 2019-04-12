// Peter Christakos
// Andrew Morrison
#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct ancestry {
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
};

#define __NR_cs3013_syscall2 378

int main() {
	pid_t pid1, pid2, pid3; //process ID 
	int c = 0;
	int s = 0;
	int a = 0; // counter variables for each member in ancestry
	struct ancestry *tree = malloc(sizeof(struct ancestry));
	printf("~~~~~~~~~~KERNEL INTERCEPTION TEST~~~~~~~~~~\n");
	
	if ((pid1 = fork()) < 0) //-1 == fork error
	{
		perror("~~~Fork Failed~~~\n");
	} 
	else if (pid1 == 0) // sleep and exit if child
	{
		sleep(10);
		exit(0);
	} 
	else 
	{
		printf("First PID: %d\n", (int) pid1);
		if ((pid2 = fork()) < 0) //-1 == fork error
		{
			perror("~~~Fork Failed~~~\n");
		}
		else if (pid2 == 0) 
		{
			sleep (10);
			exit(0);
		 } 
		 else 
		 {
			printf("Second PID %d\n", (int) pid2);
			if ((pid3 = fork()) < 0) //-1 == fork error
			{
				perror("~~~Fork Failed~~~\n");
			} 
			else if (pid3 == 0) 
			{
				sleep (10);
				exit(0);
			} 
			else 
			{
				printf("Third PID %d\n\n", (int) pid3);
				int ref = syscall(__NR_cs3013_syscall2, &pid2, tree);
				if (ref) 
				{
					perror("SysCall Failed\n");
					return -1;
				} 
				else 
				{
					printf("Target PID: %d\n\n", (int) pid2);
					printf("~~~~~Children~~~~~\n");
					while(1) 
					{
						if (tree->children[c] < 1) 
						{
							break;
						} 
						else 
						{
							printf("Child [%d] PID: %d\n", (c+1), (int) tree->children[c]);
							c++;
						}
					}
					printf("\n~~~~~Siblings~~~~~\n");
					while(1) 
					{
						if (tree->siblings[s] < 1) //if no pid
						{
							break;
						} 
						else 
						{
							printf("Sibling [%d] PID: %d\n", (s+1), (int) tree->siblings[s]);
							s++;
						}
					}
					printf("\n~~~~~Ancestors~~~~~\n");
					while(1) 
					{
						if (tree->ancestors[a] < 1) { // if no pid
							break;
						} 
						else 
						{
							printf("Parent [%d] PID: %d\n", (a+1), (int) tree->ancestors[a]);
							a++;
						}
					}
				}
			}
		}
	}
	return 0;
}

						

