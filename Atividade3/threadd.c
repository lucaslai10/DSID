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
//--
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define PORT 9876
#define FALSE 0
#define TRUE 1
#define MAX 5
#include <pthread.h>

int Running = TRUE; /* boolean: reset in control_C() */
int valores [5];
pthread_t threadz [5];
int disponivel [5]; 

void * work(void *number){
	printf("working ?	");
	int thread_number= (int) number;
	while(1){
		
		usleep(1);
		if(disponivel[thread_number]==1){
		//Threads Wakes up
	
		int valor = valores[thread_number];
		printf("Message received by thread %d",thread_number);
		while(valor<1000){
			valor=valor+1;
		}
		  
		disponivel[thread_number]=0;
		valores[thread_number]=-1;
		}
	}


}
int main(void)
{
int i;
printf("inicializando threadz");
for(i=0;i<5;i++){
	valores[i]=-1;
	disponivel[i]=0;
	pthread_create(&threadz[i], NULL, *work, (void *) i);
}
printf("Inicializando socket");
//Extracted code from the book
   void control_C();        /* ctrl-C signal handler */
   struct sockaddr_in sock; /* structure for socket() */
   int socklen=sizeof(sock);/* struct len for accept() */
   FILE *fp;                /* query text file pointer */
   fd_set fds;              /* select waits for client */
   int acc;                 /* accept file descriptor */
   int cli;                 /* client file descriptor */
   int valueee;                /* requested line number */
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
//-----------------------------------------
	printf("main loop");

	while(Running){
	listen(acc, 1);
	cli = accept(acc, (struct sockaddr *) &sock, &socklen);
        /* receive an incoming query */
        recv(cli, buf, sizeof(buf), 0);
	sscanf(buf, "%d", &valueee);
	for(i=0;i<5;i++){
	if(disponivel[i]==0){
		valores[i]=valueee;
		disponivel[i]=1;
		break;	
	}	
		
	}

	}

return 0;
}


void control_C()
{
   Running = FALSE; /* reset flag to allow exit */
   return;
}
