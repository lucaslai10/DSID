#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#define INIT 3579
#define WORK 9753

unsigned char broadcast[4] = {0xC0, 0xA8, 0x0F, 0xFF}; /* 192.64.9.255 */

main(int argc, char *argv[])
/*
**   Listing7.5.c - MASTER: broadcast UDP datagram activates SLAVE services 
*/
{
   int len = sizeof(struct sockaddr_in);
   struct sockaddr_in init;
   struct sockaddr_in work;
   struct hostent *hp;
   int ifd, wfd;
   int opt=1;
   char host[32];
   char buf[80], *p;

   /* establish initialization socket */
   ifd = socket(AF_INET, SOCK_DGRAM, 0);
   memset((char *) &init, 0, sizeof(init));
   init.sin_family = AF_INET;
   init.sin_port = htons(INIT);
   memcpy(&init.sin_addr, broadcast, 4);
   setsockopt(ifd, SOL_SOCKET, SO_BROADCAST,
             (char *) &opt, sizeof(opt));

   /* establish the work-to-do UDP socket struct */
   wfd = socket(AF_INET, SOCK_DGRAM, 0);
   memset((char *) &work, 0, sizeof(work));
   work.sin_addr.s_addr = htonl(INADDR_ANY);
   work.sin_port = htons(WORK);
   work.sin_family = AF_INET;
   bind(wfd, (struct sockaddr *) &work, len);

   /* msg starts remote SLAVES */
   gethostname(host, sizeof(host));
   if((p=strchr(host, '.')) != NULL)
      *p = '\0'; /* trunc .domain */
   sprintf(buf, "%s %d", host, WORK);
   sendto(ifd, buf, strlen(buf)+1, 0,
         (struct sockaddr *) &init, sizeof(init));
   close(ifd);

   /*
   **   Display any registration response info
   */
   printf("\n\tType ^C to exit...\n\n");
   while(1)
   {
      buf[0] = '\0'; /* clear buffer */
      recvfrom(wfd, buf, sizeof(buf), 0,
        (struct sockaddr *) &work, &len);
      printf("\tReceived: '%s'\n", buf);
   }
   close(wfd);
   return(0);
}
