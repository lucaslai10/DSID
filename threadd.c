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

#include <pthread.h>

int Running = TRUE; /* boolean: reset in control_C() */
static int clientes [5];
static int disponivel [5]; 



void * work(void *ptr){
	

	
	int thread_number= (int) ptr;
	char buf[80];            /* socket receive buffer */
 
	printf("working thread %d?\n",thread_number);	
	while(1){
		
		
		if(disponivel[thread_number]==1){
		//Threads Wakes up
		/* receive an incoming query */
		int valor; 	        
		recv(clientes[thread_number], buf, sizeof(buf), 0);
		//sscanf(buf, "%d", &valor);
		
		printf("Message received by thread %d",thread_number);
		int count =0;
		while(valor<1000){
			valor=valor+1;
			count++;
		}
		send(clientes[thread_number],buf,strlen(buf)+1,0);
		close(clientes[thread_number]);
		  
		disponivel[thread_number]=0;
		clientes[thread_number]=-1;
		}

		else{
	
		usleep(1);
		}
	}


}





int main(){
int i;
printf("Inicializando socket\n");
//Extracted code from the book
   /*
    ** Listing7.4.c - Concurrent Server: Subtask Pool
    */

    //(int cli,int acc,sockaddr_in sock,int socklen,char buf[80])
    struct area   /* flag for every subtask */
    {
        int flag[MAX]; /* 0:idle, 1:good, -:exit */
    } area, *aptr;

    int flag = (IPC_CREAT | IPC_EXCL | 0660);
    int size = sizeof(struct area);
    key_t key = 0x5a5a5a5a;
    int shmid; /* shared memory area id */
    void control_C(); /* ctrl-C signal handler */
    struct sockaddr_in sock; /* structure for socket() */
    int socklen=sizeof(sock);/* struct len for accept() */
    FILE *fp; /* query text file pointer */
    fd_set fds; /* select waits for client */
    int acc; /* accept file descriptor */
    int cli=0; /* client file descriptor */
    int line; /* requested line number */
    int opt=1; /* setsockopt parameter */
    int ident; /* internal subtask id */
    int pids[MAX]; /* subtask process ids */
    char file[32]; /* text file from client */
    char buf[80]; /* socket receive buffer */
    /* initialize ^C handler */
    
    /* create a (server) socket to accept a client */
    acc = socket(AF_INET, SOCK_STREAM, 0);
    /* bind socket to port */
    sock.sin_family = AF_INET;
    sock.sin_port = htons(PORT);
    sock.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(acc, (struct sockaddr *) &sock, socklen);
    setsockopt(acc, SOL_SOCKET, SO_REUSEADDR,
               (char *) &opt, sizeof(opt));
    /* get the shared memory area */
    shmid = shmget(key, size, flag);
    /* attach subtask's shared memory array */
    aptr = (struct area *) shmat(shmid, 0, 0);
    /* pre-establish subtask pool */

//-----------------------------------------
printf("inicializando threadz\n");

for(i=0;i<5;i++){
	pthread_t p;
	clientes[i]=-1;
	disponivel[i]=0;
	pthread_create(&p, NULL, &work,(void *) i);
}


printf("main loop\n");
 
listen(acc, 10);
while(1){
       	printf("runnando\n");
	
	printf("depois de listening\n");
	cli = accept(acc, (struct sockaddr *) &sock, &socklen);
	printf("accepted");        
	if(cli!=0){
	printf("recebi");
	for(i=0;i<5;i++){
	if(disponivel[i]==0){
		clientes[i]=cli;
		disponivel[i]=1;
		break;	
	}	
		
	}}

	}

printf("i2\n");


return 0;
}
void control_C()
{
   Running = FALSE; /* reset flag to allow exit */
   return;
}

