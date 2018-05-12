#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/signal.h>

#define PORT 9876
#define FALSE 0
#define TRUE 1
#define MAX 5

int Running = TRUE; /* boolean: reset in control_C() */

main(void)
{
   /*
   **   Listing7.4.c - Concurrent Server: Subtask Pool
   */
   struct area {            /* flag for every subtask */
      int flag[MAX];        /* 0:idle, 1:good, -:exit */
   } area, *aptr;
   int flag = (IPC_CREAT | IPC_EXCL | 0660);
   int size = sizeof(struct area);
   key_t key = 0x5a5a5a5a;
   int shmid;               /* shared memory area id */

   void control_C();        /* ctrl-C signal handler */
   struct sockaddr_in sock; /* structure for socket() */
   int socklen=sizeof(sock);/* struct len for accept() */
   FILE *fp;                /* query text file pointer */
   fd_set fds;              /* select waits for client */
   int acc;                 /* accept file descriptor */
   int cli;                 /* client file descriptor */
   int line;                /* requested line number */
   int opt=1;               /* setsockopt parameter */
   int ident;               /* internal subtask id */
   int pids[MAX];           /* subtask process ids */
   char file[32];           /* text file from client */
   char buf[80];            /* socket receive buffer */
 
   /* initialize ^C handler */
   signal(SIGINT, control_C);

   /* create a (server) socket to accept a client */
   acc = socket(AF_INET, SOCK_STREAM, 0);

   /* bind socket to port */
   sock.sin_family = AF_INET;
   sock.sin_port = htons(PORT);
   sock.sin_addr.s_addr = htonl(INADDR_ANY);
   bind(acc, (struct sockaddr *) &sock, socklen);
   setsockopt(acc, SOL_SOCKET, SO_REUSEADDR,
      (char *) &opt, sizeof(opt));
   listen(acc, 5);

   /* get the shared memory area */
   shmid = shmget(key, size, flag);
   /* attach subtask's shared memory array */
   aptr = (struct area *) shmat(shmid, 0, 0);

   /* pre-establish subtask pool */
   for(ident=0; ident<MAX; ident++)
   {
      aptr->flag[ident] = 0; /* set flags idle */

      if((pids[ident]=fork()) == 0) /* SUBTASK */
      {
         /* attach parent's shared memory arrray */
         aptr = (struct area *) shmat(shmid, 0, 0);
         /* notice: shmid is still set correctly */
 
         /* nullify ^C handler */
         signal(SIGINT, SIG_DFL);

         /*
         **   Poll memory array for a non-zero flag
         */
         while(1)
         {
            if(aptr->flag[ident] == 0)
            {
               usleep(1);   /* release processor */
               continue;    /* stay in poll loop */
            }
            else if(aptr->flag[ident] < 0)
            {
               exit(0); /* subtask's normal exit */
            }
            else /* action flag must be positive */
            {
               cli = accept(acc, (struct sockaddr *) &sock, &socklen);
               /* receive an incoming query */
               recv(cli, buf, sizeof(buf), 0);

               /* open the requested text file */
               sscanf(buf, "%s %d", file, &line);
               fp = fopen(file, "r");
               while(line--)
                  fgets(buf, sizeof(buf), fp);
               fclose(fp);

               /* send back a response */
               buf[strlen(buf)-1] = '\0';
               send(cli, buf, strlen(buf)+1, 0);
               close(cli);

               /* set to available */
               aptr->flag[ident] = 0;
            }
         }
      }
   }

   /*
   **   Parent task passes incoming connections to a subtask
   */
   while(Running) /* set FALSE in control_C signal handler */
   {
      FD_ZERO(&fds);
      FD_SET(acc, &fds);
      /* block until client is ready for connect */
      if(select(acc+1, &fds, NULL, NULL, NULL) < 0)
         break;
      /*
      **   Assign incoming query to first available subtask
      */
      for(ident=0; ident<MAX; ident++)
      {
         if(aptr->flag[ident] == 0)
         {
            aptr->flag[ident] = 1;
            break;
         }
      }
   }

   /* wait for each subtask exit */
   for(ident=0; ident<MAX; ident++)
   {
      aptr->flag[ident] = -1;
      waitpid(pids[ident]);
   }

   /* remove unused shared memory area and exit */
   shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
   return(0);
}
/*
**   ^C signal handler
*/
void control_C()
{
   Running = FALSE; /* reset flag to allow exit */
   return;
}
