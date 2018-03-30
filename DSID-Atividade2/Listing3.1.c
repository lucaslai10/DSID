#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
main(void)
/*
**
Listing3.1.c - single module subtasking using fork()
*/
{
	int pid;
	/* child task’s process id */
	/* Parent task sends a process id message to the screen */
	printf("\nParent task active as process %d.\n", getpid());
	/* fork() creates an identical copy of the parent task */
	if((pid = fork()) == 0)
	{
		/* This block is ONLY executed by the subtask */
		/* Child task sends a process id message to the screen */
		printf("Child task active as process %d.\n", getpid());
		/* Child task exits normally */
		exit(0);
	}
	/* Parent awaits child task exit */
	waitpid(pid);
	return;
}