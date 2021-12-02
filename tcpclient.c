
/*****************************************************************************/
/*** tcpclient.c                                                           ***/
/***                                                                       ***/
/*** Demonstrate an TCP client.                                            ***/
/*****************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define N 256

void *threadfuntion_send(void *arg)                    
{	
	int i;
	int sd = *(int*)arg;            /* get & convert the socket */
	char buffer[N];
	while(1)
	{
		scanf("%s", buffer);
		if(buffer[0] == '/' && buffer[1] == 's' && buffer[2] == 'e' && buffer[3] == 'n' && buffer[4] == 'd' && buffer[5] == '.' && buffer[6] == 'o' && buffer[7] == 'u' && buffer[8] == 't' && buffer[9] == '-')
		{
			for(i = 0 ; i < 256; i++)
			{
				buffer[i] = buffer[i + 10];
			}
			send(sd,buffer,10,0);
		}
		else
		{
			printf("Undifined command line.\n");
		}
	}
	return 0;
}


void *threadfuntion(void *arg)                    
{	
	int sd = *(int*)arg;            /* get & convert the socket */
	char buffer[256];
	while(1)
	{
		recv(sd,buffer,sizeof(buffer),0);
		printf("Received: %s\n",buffer);
	}
	shutdown(sd,SHUT_RD);
	shutdown(sd,SHUT_WR);
	shutdown(sd,SHUT_RDWR);
			                  /* close the client's channel */
	return 0;                           /* terminate the thread */
}

void panic(char *msg);
#define panic(m)	{perror(m); abort();}
/****************************************************************************/
/*** This program opens a connection to a server using either a port or a ***/
/*** service.  Once open, it sends the message from the command line.     ***/
/*** some protocols (like HTTP) require a couple newlines at the end of   ***/
/*** the message.                                                         ***/
/*** Compile and try 'tcpclient lwn.net http "GET / HTTP/1.0" '.          ***/
/****************************************************************************/
int main(int count, char *args[])
{	struct hostent* host;
	struct sockaddr_in addr;
	int sd, port;

	if ( count != 3 )
	{
		printf("usage: %s <servername> <protocol or portnum>\n", args[0]);
		exit(0);
	}

	/*---Get server's IP and standard service connection--*/
	host = gethostbyname(args[1]);
	//printf("Server %s has IP address = %s\n", args[1],inet_ntoa(*(long*)host->h_addr_list[0]));
	if ( !isdigit(args[2][0]) )
	{
		struct servent *srv = getservbyname(args[2], "tcp");
		if ( srv == NULL )
			panic(args[2]);
		printf("%s: port=%d\n", srv->s_name, ntohs(srv->s_port));
		port = srv->s_port;
	}
	else
		port = htons(atoi(args[2]));

	/*---Create socket and connect to server---*/
	sd = socket(PF_INET, SOCK_STREAM, 0);        /* create socket */
	if ( sd < 0 )
		panic("socket");
	memset(&addr, 0, sizeof(addr));       /* create & zero struct */
	addr.sin_family = AF_INET;        /* select internet protocol */
	addr.sin_port = port;                       /* set the port # */
	addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);  /* set the addr */

	/*---If connection successful, send the message and read results---*/
	if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
	{	
		while(1)
		{
			pthread_t receive, send;
			pthread_create(&receive, 0, threadfuntion, &sd);       /* start thread */
			pthread_create(&send, 0, threadfuntion_send, &sd);       /* start thread */
			pthread_detach(receive);                      /* don't track it */
			pthread_detach(send);                      /* don't track it */
		}
	}
	else
		panic("connect");
}
