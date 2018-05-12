#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 9876

main(void)
{
   /*
   **   Listing7.2.c - Server: Return one line from a file
   */
   struct sockaddr_in sock; /* structure for socket() */
   int socklen=sizeof(sock);/* struct len for accept() */
   FILE *fp;                /* sample.txt file pointer */
   int acc;                 /* accept file descriptor */
   int cli;                 /* client file descriptor */
   int line;                /* requested line number */
   int opt=1;               /* setsockopt parameter */
   char buf[80];            /* socket receive buffer */
   char file[32];           /* text file from client */

   /* create a (server) socket to accept a client */
   acc = socket(AF_INET, SOCK_STREAM, 0);

   /* bind socket to port */
   sock.sin_family = AF_INET;
   sock.sin_port = htons(PORT);
   sock.sin_addr.s_addr = htonl(INADDR_ANY);
   bind(acc, (struct sockaddr *) &sock, socklen);
   setsockopt(acc, SOL_SOCKET, SO_REUSEADDR,
      (char *) &opt, sizeof(opt));

   while(1)
   {
      listen(acc, 5);

      /* get a random line of data requested by the client */
      cli = accept(acc, (struct sockaddr *) &sock, &socklen);
      recv(cli, buf, sizeof(buf), 0);

      /* open the requested file */
      sscanf(buf, "%s %d", file, &line);
      fp = fopen(file, "r");
      while(line--)
         fgets(buf, sizeof(buf), fp);
      fclose(fp);

      buf[strlen(buf)-1] = '\0';
      send(cli, buf, strlen(buf)+1, 0);
      close(cli);
   }
}
