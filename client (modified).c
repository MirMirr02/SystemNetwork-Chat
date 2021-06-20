#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
	
#define BUFSIZE 1024
		
void send_recv(int i, int sockfd)
{
	char snd_buf[BUFSIZE];
	char rcv_buf[BUFSIZE];
	int nbyte_rcvd;
	
	if (i == 0){
		fgets(snd_buf, BUFSIZE, stdin);
		if (strcmp(snd_buf , "quit\n") == 0) {
			printf("\n");
			printf("   **CLIENT HAS LEFT THE CHATROOM**   \n");
			printf("---------------------------------------\n");
			exit(0);
		}
		else
			send(sockfd, snd_buf, strlen(snd_buf), 0);
	}
	else {
		nbyte_rcvd = recv(sockfd, rcv_buf, BUFSIZE, 0);
		rcv_buf[nbyte_rcvd] = '\0';
		printf("%s\n" , rcv_buf);
		fflush(stdout);
	}
}

void connect_request(int *sockfd, struct sockaddr_in *servaddr)
{
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Socket creation failed");
		exit(1);
	}
		printf("Socket created...\n");
	
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(8080);
	servaddr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(servaddr->sin_zero, '\0', sizeof servaddr->sin_zero);
	
	int c = connect(*sockfd, (struct sockaddr *)servaddr, sizeof(struct sockaddr));
	
	if(c < 0)
	{
		perror("Connection error");
		exit(1);
	}
		printf("Connected to server...\n");
		printf("---------------------------------------\n");
		printf("   ***WELCOME TO THIS CHATROOM***   \n");
		printf("\n");
}
int main()
{
	int sockfd, fdmax, i;
	struct sockaddr_in server;
	fd_set master;
	fd_set read_fds;
	
	connect_request(&sockfd, &server);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;
	
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_recv(i, sockfd);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}