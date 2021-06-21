#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
	
#define PORT 4950
#define BUFSIZE 1024

int count;

//Making an array of 11 clients//
struct cli {
        int status, sockfd;
        char name[20];
};

struct cli *arr[11];

//Send message to client//
void send_to_client(int j, int i, char *names, int sockfd, fd_set *master)
{
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			if (send(j, names, count, 0) == -1) {
				perror("send");
                        }
                }
        }
}



//Communication between server and client//		
void send_recv_client(int i, fd_set *master, int sockfd, int fdmax)
{
        //Get data from server on message and name of client//
        int nbytes_recvd, j, k, m;
        char recv_buf[BUFSIZE], buf[BUFSIZE], name[20], message[BUFSIZE];
        memset(name, 0, sizeof(name));
        if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
                if (nbytes_recvd == 0) {
                        for(j=0; j<11; ++j){
                            if(arr[j] != NULL && arr[j]->sockfd == i){
                                //Shows disconnected user in client//
                                memset(message, 0, sizeof(message));
                                strcat(message, arr[j]->name);
                                strcat(message, " disconnected from the chatroom\n");
                                for(m=0; m<11; m++){
                                    if(arr[m] != NULL && arr[m]->sockfd != i && arr[m]->status)
                                        if(send(arr[m]->sockfd, message, sizeof(message), 0) == -1)
                                            perror(send);
                                }
                                //Shows disconnected users in server//
                                printf("%s disconnected from server\n", arr[j]->name);
                                arr[j]->status = 0;
                                break;
                            }
                        }
                }else {
                        perror("recv");
                }
                
                close(i);
                FD_CLR(i, master);

        }else { 
                //Get data on name received//
                if(recv_buf[nbytes_recvd] = '\0');
                memset(name, 0, sizeof(name));
                for(k=0; recv_buf[k] != ' ' && recv_buf != '\0'; ++k)
                        name[k] = recv_buf[k];
                name[++k] = '\0';
                //Shows a list of connected users in the client//
                if(strcmp(name,"list") == 0){
                    memset(message, 0, sizeof(message));
                    strcat(message, "Connected users:\n");
                    for(k=0; k<11; ++k){
                        if(arr[k] != NULL && arr[k]->status){
                            strcat(message, arr[k]->name);
                            strcat(message, "\n");
                        }
                    }
                    strcat(message, "\n");
                    message[strlen(message)-1] = '\0';
                    if (send(i, message, strlen(message), 0) == -1) {
                            perror("send");
                    }
                        }
                        else{
                                //Set a name to send message to specific client//
                            for(k=0; k<11; ++k){
                            if(arr[k] != NULL && strcmp(arr[k]->name, name) == 0){
                                    for(j=0; j<11; ++j){
                                        if(arr[j] != NULL && arr[j]->sockfd == i){       
                                                perror("send");
                                                break;
                                                }
                                        }

                                    //Shows the name and messages of the sender//
                                    memset(message, 0, sizeof(message));
                                    strcat(message, arr[j]->name);
                                    strcat(message, " : ");
                                    strcat(message, recv_buf+strlen(name));
                                    if (send(arr[k]->sockfd, message, strlen(message), 0) == -1) {
                                            perror("send");
                                    }
                            }
                            }
                        }
        }
}




//Shows accepted connection on the server requested by Client//
void connection_client_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
        socklen_t addrlen;
        int newsockfd, i, m;
        char name[20], message[BUFSIZE];
        
        addrlen = sizeof(struct sockaddr_in);
        if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
                perror("accept");
                exit(1);
        }else {
                FD_SET(newsockfd, master);
                if(newsockfd > *fdmax){
                        *fdmax = newsockfd;
                }
                //Shows Client connected on Server//
                memset(name, 0, sizeof(name));
                int n = recv(newsockfd, name, 20, 0);
                name[n] = '\0';
<<<<<<< HEAD
                printf("%s : is now connected on port %d\n", name, ntohs(client_addr->sin_port));
                //%s to show connected user names//
=======
                printf("%s is now connected\n", name);
                //puts(name);
>>>>>>> 5b42a69aeb9f61179365d750a9b9f56de0265b76
                

                //Shows connected user in client//
                memset(message, 0, sizeof(message));
                strcat(message, name);
<<<<<<< HEAD
                strcat(message, " is now connected\n");
=======
                strcat(message, " is now enter the chatroom\n");
>>>>>>> 5b42a69aeb9f61179365d750a9b9f56de0265b76
                for(m=0; m<11; m++){
                    if(arr[m] != NULL && arr[m]->status)
                        if(send(arr[m]->sockfd, message, sizeof(message), 0) == -1)
                            perror(send);
                }
                //Get data from server and show the connected client in other client//
                for(i=0; i<11; ++i){
                        if(arr[i] != NULL && strcmp(name, arr[i]->name) == 0){
                                arr[i]->status = 1;
                                arr[i]->sockfd = newsockfd;
                                count++;
                                break;
                        }
                }
                

                if(i == 11){
                        for(i=0; i<11; ++i){
                                if(arr[i] == NULL){
                                        arr[i] = (struct cli*)malloc(sizeof(struct cli));
                                        strcpy(arr[i]->name, name);
                                        arr[i]->sockfd = newsockfd;
                                        arr[i]->status = 1;
                                        break;
                                }
                        }
                        //Limit the amount of client in a server//
                        if(i == 11){
                                printf("Cannot handle any more client.");
                                exit(1);
                        }
                }
		}
}


//Make connection on a server//	
void connect_request(int *sockfd, struct sockaddr_in *addr)
{
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Socket creation failed\n");
		exit(1);
	}
		printf("Socket created...\n");

	//Address of server//	
	addr->sin_family = AF_INET;
	addr->sin_port = htons(4950);
	addr->sin_addr.s_addr = INADDR_ANY;
	memset(addr->sin_zero, '\0', sizeof addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
	{
                //Create socket//
		perror("setsockopt");
		exit(1);
	}
		
	if (bind(*sockfd, (struct sockaddr *)addr, sizeof(struct sockaddr)) == -1) 
	{      
                //Port already in used//
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) 
	{
                //Start to listen connection request from client//
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
	
        //FD_ZERO initialize file descriptor and set FD_SET to have zero bits for all files//
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
                                        //Show Client connection to Server//
					connection_client_accept(&master, &fdmax, sockfd, &cliaddr);
				else
                                        //Send and Receive data from server//
					send_recv_client(i, &master, sockfd, fdmax);
                        }
                }
        }
        return 0;
}






        

