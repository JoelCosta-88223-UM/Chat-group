#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

void panic(char *msg);
#define panic(m)	{perror(m); abort();}
#define N 256

#define MAX_CLIENT_NUM 4

typedef struct client_socket_into client_socket_info_t;
struct client_socket_into
{
	int socket;
	int state;
	int index;
}; 

client_socket_info_t socket_table[MAX_CLIENT_NUM];

int threads = 0;

static void sendPeriodicUpdate(int signo)
{
	int i;

	if(signo==SIGALRM)
	{    
		for(i=0; i<MAX_CLIENT_NUM; i++)
		{
			if(socket_table[i].state)
			{
			}
		}
	}
}


void *threadfuntion_echo(void *arg)                    
{	
	int i;
	client_socket_info_t *info = (client_socket_info_t *)arg;   /* get & convert the socket */
	char buffer[256];
	while(1)
	{
		if(recv(info->socket, &buffer, sizeof(buffer), 0) != 0)
		{
			for(i = 0; i < MAX_CLIENT_NUM; i++)
			{
				if(info->index != socket_table[i].index) 
				{
			        	send(socket_table[i].socket, buffer, sizeof(buffer), 0);
		      		}
			}
        	}	
	}
	shutdown(info->socket,SHUT_RD);
	shutdown(info->socket,SHUT_WR);
	shutdown(info->socket,SHUT_RDWR);

	socket_table[info->index].state = 0;	/* marked as a already closed channel*/
	threads--;
			                  /* close the client's channel */
	return 0;                           /* terminate the thread */
}

/* *threadfuntion_send(void *arg)                    
{	
	int i;
	client_socket_info_t *info = (client_socket_info_t *)arg;
	char buffer[256];
	while(1)
	{
		scanf("%s", buffer);
		for(i = 0; i < MAX_CLIENT_NUM; i++)
		{
		        if(info->index != socket_table[i].index) 
		        {
		        	send(socket_table[i].socket, buffer, sizeof(buffer), 0);
		        }
		}
	}
	return 0;
}*/


/*void *threadfuntion_receive(void *arg)                    
{	
	char buffer[256];
	int sd = *(int*)arg;            /* get & convert the socket */
/*	while(1)
	{
		recv(sd,buffer,sizeof(buffer),0);
		printf("Received: %s\n",buffer);
	}
/*	shutdown(sd,SHUT_RD);
	shutdown(sd,SHUT_WR);
	shutdown(sd,SHUT_RDWR);*/
			                  /* close the client's channel */
/*	return 0;                           /* terminate the thread */
//}

int main(int count, char *args[])
{	struct sockaddr_in addr;
	int listen_sd, port;

	if ( count != 2 )
	{
		printf("usage: %s <protocol or portnum>\n", args[0]);
		exit(0);
	}

	/*---Get server's IP and standard service connection--*/
	if ( !isdigit(args[1][0]) )
	{
		struct servent *srv = getservbyname(args[1], "tcp");
		if ( srv == NULL )
			panic(args[1]);
		printf("%s: port=%d\n", srv->s_name, ntohs(srv->s_port));
		port = srv->s_port;
	}
	else
		port = htons(atoi(args[1]));

	/*--- create socket ---*/
	listen_sd = socket(PF_INET, SOCK_STREAM, 0);
	if ( listen_sd < 0 )
		panic("socket");

	/*--- bind port/address to socket ---*/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = INADDR_ANY;                   /* any interface */
	if ( bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
		panic("bind");

	/*--- make into listener with 10 slots ---*/
	if ( listen(listen_sd, 10) != 0 )
		panic("listen")

	/*--- begin waiting for connections ---*/
	else
	{	
		struct itimerval itv;

		signal(SIGALRM,sendPeriodicUpdate);

		//ualarm(300,300);	
		itv.it_interval.tv_sec = 5;
		itv.it_interval.tv_usec = 0;//4*10000;
		itv.it_value.tv_sec = 1;
		itv.it_value.tv_usec = 0;//4*10000;
		setitimer (ITIMER_REAL, &itv, NULL);
		
		while (1)                         /* process all incoming clients */
		{
			int n = sizeof(addr);
			int sd = accept(listen_sd, (struct sockaddr*)&addr, &n);     /* accept connection */
			if(sd!=-1)
			{
				pthread_t /*receive, send,*/ echo;
				
				socket_table[threads].socket=sd;
				socket_table[threads].state=1;		/*means connection opened*/
				socket_table[threads].index=threads;
				printf("New connection\n");
				//pthread_create(&receive, 0, threadfuntion_receive, &socket_table[threads++]);       /* start thread */
				//pthread_create(&send, 0, threadfuntion_send, &socket_table[threads++]);       /* start thread */
				pthread_create(&echo, 0, threadfuntion_echo, &socket_table[threads++]);       /* start thread */
				//pthread_detach(receive);                      /* don't track it */
				//pthread_detach(send);                      /* don't track it */
				pthread_detach(echo); 
				
			}
		}
	}
}
