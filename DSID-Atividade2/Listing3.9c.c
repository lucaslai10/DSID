#include <stdio.h>

main(void)
/*
**
Listing3.9b.c - service: prints TCP packets from a remote client
*/
{
	FILE *log;
	char buffer[80];
	/* open a message log file */
	log = fopen("/tmp/log", "w");
	/* echo messages received from client into log file */
	while(1)
	{
		read(0, buffer, sizeof(buffer));
		fprintf(log, "Service: %s", buffer);
		if(strncmp(buffer, "EXIT", 4) == 0) /* exit request */
		{
			puts("Bye!");
			break;
		}
	}
	fclose(log);
	return(0);
}