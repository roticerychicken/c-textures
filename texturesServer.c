#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <openssl/sha.h>

#define ERROR -1
//amount of clients
#define MAX_CLIENTS	5
//size of the buffer
#define MAX_DATA	1024

int main(int argc,char *argv[]) {
	struct sockaddr_in server;
	struct sockaddr_in client;
	int sock;
	int new;
	int sockaddr_len = sizeof(struct sockaddr_in);
	int data_len;
	char data[MAX_DATA];

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("socket: ");
		exit(1);
	}


	server.sin_family = AF_INET;
    	server.sin_port = htons(8085);
    	//inaddr instructs the kernal to listen on all interfaces
    	server.sin_addr.s_addr = INADDR_ANY;
    	bzero(&server.sin_zero,8);
	
	if ((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR) {
        	perror("bind :");
        	exit(-1);
    	}

	if ((listen(sock, MAX_CLIENTS)) == ERROR) {
        	perror("listen");
        	exit(-1);
   	}
	
	while(1) {
		if ((new = accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == ERROR) {
			perror("accept");
			exit(-1);
		}
		printf("New client connected from port no %d and IP %s\n",ntohs(client.sin_port), inet_ntoa(client.sin_addr));

		data_len = 1;	
		while(data_len){
			data_len = recv(new,data,MAX_DATA,0);
			if(data_len) {
				data[data_len] = '\0';
				printf("sent msg: %s\n",data);
			}	
		}
		close(new);
	}
	close(sock);

	return 0;
}






