#include <stdio.h>
#include <unistd.h>
main(void)
/*
**
Listing3.2a.c - parent: subtasking using fork() and execl()
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
		/* Child task replaces itself with disk image of child */
		execl("/home/grifo/Documents/DSID/DSID-Atividade2/l3.2b", "l3.2b", NULL);
		printf("A successful execl() call will never return!\n");
		exit(-1);
	}
	/* Parent awaits child task exit */
	waitpid(pid);
	return;
}