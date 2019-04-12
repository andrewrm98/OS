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

int main(int argc, char *argv[]) {
	int a = 0;
	int s = 0;
	int c = 0;
	
	if (argc != 2) {
		printf("ERROR: Did not specify a PID\n");
		return -1;
	}
	pid_t pid = atoi(argv[1]);
	struct ancestry *tree = malloc(sizeof(struct ancestry));
	int ref = syscall(__NR_cs3013_syscall2, &pid, tree);
	if (ref) 
	{
		perror("SysCall Failed\n");
		return -1;
	} 
	while(1) 
	{
		if (tree->children[c] < 1) // no pid
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
	return 0;
}