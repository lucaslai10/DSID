#include <sys/signal.h>
#include <stdio.h>
#include <unistd.h>
int count = 0; /* counter for the number of ^C events handled */
main(void)
/*
**
Listing3.3.c - handles signal from ^C typed on keyboard
*/
{
	void control_C();
	/* initialize ^C handler */
	signal(SIGINT, control_C);
	while(count < 3) {
		/* tell interactive user what to do */
		printf("\n\tType a ^C when ready...");
		fflush(stdout);
		pause();
		printf("handled event #%d.\n", count);
	}
	printf("\nThree events were handled - bye!\n\n");
}

void control_C()
{
	/* reestablish handler */
	signal(SIGINT, control_C);
	/* count ^Cs */
	count++;
}