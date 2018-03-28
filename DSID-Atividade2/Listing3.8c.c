#include <stdio.h>

main(void)
/*
**Listing3.8c.c - service: prints UDP datagrams from a remote client
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
		if(strncmp(buffer, "EXIT", 4) == 0)
		{
			puts("Bye!");
			break;
		}
	/* exit request */
	}
	fclose(log);
	return(0);
}