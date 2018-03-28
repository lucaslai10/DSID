#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

main(void)
/*
**
Listing3.4.c - child changes data in parent’s shared memory
*/
{
	struct area { /* shared memory area */
		int value; /* (a simple example) */
	} area, *ap;
	int flag = (IPC_CREAT | IPC_EXCL | 0660);
	int size = sizeof(struct area);
	key_t key = 0x01020304; /* example */
	int shmid; /* shared memory area id */
	int pid;
	/* child task process id */
	/* get a shared memory area */
	shmid = shmget(key, size, flag);
	/* attach the shared memory area */
	ap = (struct area *) shmat(shmid, 0, 0);
	/* set the value of the shared memory data */
	ap->value = 0; /* initialize value to zero */
	printf("\nParent initial value set to zero\n");
	if((pid = fork()) == 0)
		/* SUBTASK */
	{
		/* attach parent’s shared memory */
		ap = (struct area *) shmat(shmid, 0, 0);
		/* note: shmid is still set correctly */
		sleep(2); /* let parent brag a while */
		/* change the value of the shared memory data */
		ap->value = 1; /* something other than zero */
		/* child task exits normally */
		exit(0);
	}
	/* watch for value to change */
	while(1)
	{
		printf("Parent: value = %d,", ap->value);
		fflush(stdout);
		if(ap->value != 0)
		break;
		printf(" continuing...\n");
		sleep(1);
	}
	printf(" Bye!\n\n");
	/* parent awaits child task exit */
	waitpid(pid);
	/* release the shared memory area */
	shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
	return;
}