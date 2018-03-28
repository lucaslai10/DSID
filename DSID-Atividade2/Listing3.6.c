#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char path[] = {"/tmp/socket3.3.6"};
/* socket name */
main(void)
/*
**
Listing3.6.c - UDP communication with a forked subtask
*/
{
	struct sockaddr_un sock;
	int len=sizeof(sock);
	int pid;
	/* child task’s process id */
	int fd;
	/* socket file descriptor */
	char buffer[80];
	/* establish and initialize UDP socket struct */
	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	memset((char *) &sock, 0, sizeof(sock));
	strcpy(sock.sun_path, path);
	sock.sun_family = AF_UNIX;
	/* create child subtask */
	if((pid = fork()) == 0) 
	{
		/* publish the port number we are listening to */
		bind(fd, (struct sockaddr *) &sock, len);
		/* echo messages received from parent back to user */
		while(1) {
			recvfrom(fd, buffer, sizeof(buffer), 0,
			(struct sockaddr *) &sock, &len);
			printf("Child: %s", buffer);
			if(strncmp(buffer, "EXIT", 4) == 0)
			/* exit request */
			{
				unlink(path);
				puts("Bye!");
				close(fd);
				break;
			}
		}
		/* Child exit */
		exit(0);
	}
	/* send a message typed from interactive user to child subtask */
	while(1)
	{
		sleep(1); /* response time */
		printf("\nEnter a message: ");
		fflush(stdout); 
		fgets(buffer, sizeof(buffer), stdin);
		sendto(fd, buffer, strlen(buffer)+1, 0,
		(struct sockaddr *) &sock, sizeof(sock));
		if(strncmp(buffer, "EXIT", 4) == 0)
		{
			close(fd);
			break;
		}
	/* exit request */
	}
	/* await Child exit */
	waitpid(pid);
	return; 
}