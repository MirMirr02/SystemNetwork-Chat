#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
	
#define PORT 8080
#define BUFSIZE 1024

void send_to_all(int j, int i, int sockfd, int nbytes_rcvd, char *rcv_buf, fd_set *master)
{
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			if (send(j, rcv_buf, nbytes_rcvd, 0) == -1) {
				perror("send");
			}
		}
	}
}
		
void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
	int nbytes_rcvd, j;
	char rcv_buf[BUFSIZE], buf[BUFSIZE];
	
	if ((nbytes_rcvd = recv(i, rcv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_rcvd == 0) 
		{
			printf("---------------------------------------\n");
			printf("Socket %d disconnected from the server\n", i);
			
		}
		else {
			perror("recv");
		}
		close(i);
		FD_CLR(i, master);
	} else { 
	//	printf("%s\n", rcv_buf);
		for(j = 0; j <= fdmax; j++){
			send_to_all(j, i, sockfd, nbytes_rcvd, rcv_buf, master );
		}
	}	
}
		
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *cliaddr)
{
	socklen_t addrlen;
	addrlen = sizeof(struct sockaddr_in);
	int newsockfd = accept(sockfd, (struct sockaddr *)cliaddr, &addrlen);
	
	if( newsockfd == -1) 
	{
		perror("accept error");
		exit(1);
	}
	else
	{
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax)
		{
			*fdmax = newsockfd;
		}
		printf("%s on port %d has connected to the server\n",inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port));
	}
}
	
void connect_request(int *sockfd, struct sockaddr_in *addr)
{
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Socket creation failed");
		exit(1);
	}
		printf("Socket created...\n");
		
	addr->sin_family = AF_INET;
	addr->sin_port = htons(8080);
	addr->sin_addr.s_addr = INADDR_ANY;
	memset(addr->sin_zero, '\0', sizeof addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}
		
	if (bind(*sockfd, (struct sockaddr *)addr, sizeof(struct sockaddr)) == -1) 
	{
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) 
	{
		perror("listen");
		exit(1);
	}
	printf("TCPServer connecting to the client\n");
	printf("---------------------------------------\n");
	fflush(stdout);
}

int main()
{
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in addr, cliaddr;
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &addr);
	FD_SET(sockfd, &master);
	
	fdmax = sockfd;
	while(1)
	{
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(4);
		}
		
		for (i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{
				if (i == sockfd)
					connection_accept(&master, &fdmax, sockfd, &cliaddr);
				else
					send_recv(i, &master, sockfd, fdmax);
			}
		}
	}
	return 0;
}
