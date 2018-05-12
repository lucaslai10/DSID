#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 9876
#define NUM 20

main(int argc, char *argv[])
{
   /*
   **   Listing7.1.c - Client: request a line from a remote file
   */
   struct timeval bef, aft;   /* timer value before/after */
   struct sockaddr_in sock;   /* structure for socket */
   struct hostent *hp;        /* structure for IP address */
   double random;             /* random reals: 0.0->1.0 */
   long timeleft;             /* time remaining in IAT */
   long secs=0L;              /* inter-arrival time seconds */
   long usec=0L;              /* inter-arrival microseconds */
   int pids[NUM];             /* subtask process ids */
   int opt=1;                 /* setsockopt parameter */
   int fd;                    /* socket file descriptor */
   int ii, jj, kk=0;
   char buf[80], *p;

   srand((unsigned int) getpid()); /* seed rand() */

   /*
   **   Operands are remote HOST name and IAT (Inter-Arrival Time)
   */
   if(argc != 3)
   {
      printf("\n\tUsage: %s <HOST> <IAT>\n\n", argv[0]);
      exit(-1);
   }

   /* uSeconds part of inter-arrival */
   if((p=strchr(argv[2], '.')) != NULL)
   {
      *p = '\0'; /* end whole number at decimal */
      p++; /* bump pointer to start of mantissa */
      while(strlen(p) < 6)
         strcat(p, "0"); /* pad out to 6 digits */
      p[6] = '\0';  /* truncate to 6 digits max */
      usec = atol(p);
   }
   secs = atol(argv[2]); /* seconds part of IAT */

   /*
   **   LOOP: send and receive NUM packets
   */
   for(ii=0; ii<NUM; ii++)
   {
      /* get time before send */
      gettimeofday(&bef, NULL);

      /* random integers 1 through 99 */
      random = rand() / (double) RAND_MAX;
      jj = (int) ((double) (99.0) * random) + 1;
      if(jj == 100)
         jj = 99;
      sprintf(buf, "/home/chief/sample.txt %d", jj);

      if((pids[kk++]=fork()) == 0)
      {
         /* set up socket info for connect */
         fd = socket(AF_INET, SOCK_STREAM, 0);
         memset((char *) &sock, 0, sizeof(sock));
         hp = gethostbyname(argv[1]);
         memcpy(&sock.sin_addr, hp->h_addr, hp->h_length);
         sock.sin_family = hp->h_addrtype;
         sock.sin_port = htons(PORT);
         setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
            (char *) &opt, sizeof(opt));

         /* connect to server */
         connect(fd, (struct sockaddr *) &sock, sizeof(sock));
         send(fd, buf, strlen(buf)+1, 0);
         buf[0] = 0; /* clear buffer */
         recv(fd, buf, sizeof(buf), 0);

         /* print out response to our query */
         printf("\t%d. Line %d: '%s' ", kk, jj, buf);
         /* check for correct line */
         p = strrchr(buf, ' ') + 2;
         if(jj != atoi(p))
            printf("*");
         printf("\n");
         close(fd);
         exit(0);
      }

      /*
      **   Sleep for remainder of IAT
      */
      gettimeofday(&aft, NULL);
      aft.tv_sec -= bef.tv_sec;
      aft.tv_usec -= bef.tv_usec;
      if(aft.tv_usec < 0L)
      {
         aft.tv_usec += 1000000L;
         aft.tv_sec -= 1;
      }
      bef.tv_sec = secs;
      bef.tv_usec = usec;
      bef.tv_sec -= aft.tv_sec;
      bef.tv_usec -= aft.tv_usec;
      if(bef.tv_usec < 0L)
      {
         bef.tv_usec += 1000000L;
         bef.tv_sec -= 1;
      }
      timeleft = (bef.tv_sec * 1000000L ) + bef.tv_usec;
      if(timeleft < 0)
      {
         printf("\tERROR: A higher IAT value is required - exiting.\n");
         break;
      }
      usleep(timeleft);
   }
   for(ii=0; ii<kk; ii++)
      wait(pids[ii]);
   return(0);
}
