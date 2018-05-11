#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>

main(void)
/*
**
Listing3.5.c - simultaneous handling of deposits and withdrawals
*/
{
	struct area { /* shared memory area */
		int balance;
		int deposit;
		int withdrawal;
	} area, *ap;
	#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)\
	/* union semun is usually defined by including <sys/sem.h> */
	#else
	/* otherwise we have to define it ourselves, per <bits/sem.h> */
	union semun
	{
		int val;
		/* value for SETVAL */
		struct semid_ds *buf;
		/* buffer for IPC_STAT, IPC_SET */
		unsigned short int *array; /* array for GETALL, SETALL */
		struct seminfo *__buf;
		/* buffer for IPC_INFO */
	};
	#endif
	
	union semun un; /* semaphore union and buffer */
	struct sembuf buf;
	int flag = (IPC_CREAT | IPC_EXCL | 0660);
	int size = sizeof(struct area);
	key_t skey = 0x04030201; /* sample keys */
	key_t mkey = 0x01020304;

	int semid; /* semaphore id */
	int shmid; /* shared memory id */
	int pidD; /* deposit child task pid */
	int pidW; /* withdrawal child task pid */
	int amount;
	char inbuf[80];
	/* get a shared memory area */
	shmid = shmget(mkey, size, flag);
	/* attach the shared memory area */
	ap = (struct area *) shmat(shmid, 0, 0);
	/* initialize account info in shared memory */
	ap->balance = 1000; /* initialize balance to $1000 */
	ap->withdrawal = 0;
	ap->deposit = 0;
	/* get and initialize a semaphore */
	semid = semget(skey, 1, flag);
	/* set semaphore available */
	un.val = 1;
	semctl(semid, 0, SETVAL, un);
	/* first and only semaphore */
	buf.sem_num = 0;
	/* wait if set */
	buf.sem_flg = 0;
	if((pidD = fork()) == 0)
	/* DEPOSITS */
	{
		/* attach parent’s shared memory */
		ap = (struct area *) shmat(shmid, 0, 0);
		/* note: shmid is still set correctly */
		/* handle deposit */
		while(1)
		{
			sleep(1);
			/* set semaphore */
			buf.sem_op = -1;
			semop(semid, (struct sembuf *) &buf, 1);
			if(ap->deposit < 0)
			break; /* exit req */
			if(ap->deposit > 0)
			{
				/* handle the deposit */
				ap->balance += ap->deposit;
				ap->deposit = 0; /* clear it */
			}
			/* clear semaphore */
			buf.sem_op = 1;
			semop(semid, (struct sembuf *) &buf, 1);
		}
		/* child task exits normally */
		exit(0);
	}
	if((pidD = fork()) == 0)
	/* WITHDRAWALS */
	{
		/* attach parent’s shared memory */
		ap = (struct area *) shmat(shmid, 0, 0);
		/* note: shmid is still set correctly */
		/* handle withdrawal */
		while(1)
		{
			sleep(1);
			/* set semaphore */
			buf.sem_op = -1;
			semop(semid, (struct sembuf *) &buf, 1);
			if(ap->withdrawal < 0)
			break; /* exit signal */
			if(ap->withdrawal > 0)
			{
				/* handle the withdrawal */
				ap->balance -= ap->withdrawal;
				ap->withdrawal = 0; /* clear it */
			}
			/* clear semaphore */
			buf.sem_op = 1;
			semop(semid, (struct sembuf *) &buf, 1);
		}
		/* child task exits normally */
		exit(0);
	}
	/* parent: handle deposit and withdrawal transactions */
	printf("\n\n\n\tWELCOME TO THE FIRST INTERACTIVE BANK\n\n");
	while(1)
	{
		printf("\nEnter D for deposit, W for withdrawal: ");
		fflush(stdout);
		fgets(inbuf, sizeof(inbuf), stdin);
		if(inbuf[0] == 'D' || inbuf[0] == 'd')
		{
			printf("\tCurrent account balance is $%d\n", ap->balance);
			printf("\tEnter deposit amount (0 to exit): ");
			fflush(stdout);
			fgets(inbuf, sizeof(inbuf), stdin);
			/* set the semaphore */
			buf.sem_op = -1;
			semop(semid, (struct sembuf *) &buf, 1);
			amount = atoi(inbuf);
			if(amount <= 0) /* exit requested */
			{
				/* signal subtasks */
				ap->deposit = -1;
				ap->withdrawal = -1;
				break; /* exit infinite loop */
			}
			ap->deposit = amount; /* deposit it */
			/* clear semaphore */
			buf.sem_op = 1;
			semop(semid, (struct sembuf *) &buf, 1);
		}
		else if(inbuf[0] == 'W' || inbuf[0] == 'w')
		{
			printf("\tCurrent account balance is $%d\n", ap->balance);
			printf("\tEnter withdrawal amount (0 to exit): ");
			fflush(stdout);
			fgets(inbuf, sizeof(inbuf), stdin);
			/* set the semaphore */
			buf.sem_op = -1;
			semop(semid, (struct sembuf *) &buf, 1);
			amount = atoi(inbuf);
			if(amount <= 0) /* exit requested */
			{
				/* signal subtasks */
				ap->deposit = -1;
				ap->withdrawal = -1;
				break; /* exit infinite loop */
			}
			else if(amount <= ap->balance)
			{
				ap->withdrawal = amount; /* withdraw it */
			}
			else
			{
				printf("ERROR: Insufficient funds!\n");
			}
/* clear semaphore */
			buf.sem_op = 1;
			semop(semid, (struct sembuf *) &buf, 1);
		}
		else
		{
			printf("Invalid transaction code ‘%c’\n", inbuf[0]);
		}
	}
	/* await child exits */
	waitpid(pidD);
	waitpid(pidW);
	printf("\nYour final account balance is %d\nBye!\n\n", ap->balance);
	/* remove shared memory, semaphore, and exit */
	shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
	semctl(semid, 0, IPC_RMID, un);
	return;
}