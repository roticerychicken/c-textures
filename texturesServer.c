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
#define MAX_DATA	1024
int main(int argc,char *argv[]) {
	struct sockaddr_in server;
	struct sockaddr_in client;
	int sock;
	int new;
	int sockaddr_len = sizeof(struct sockaddr_in);
	int data_len;
	char data[MAX_DATA];
	
	if((sock = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("socket: ");
		exit(1);
	}
	
	server.sin_family = AF_INET;
	//to network byte order
	server.sin_port = htons(8085);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero,8);

	if((bind(sock,(struct sockaddr *)&server, sockaddr_len)) == -1) {
		perror("bind :");
		exit(-1);
	}

	if((listen(sock,2)) == -1) {
		perror("listen");
		exit(-1);
	}
	while(1) {
		if((new == accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == -1) {
			perror("accept");
			exit(-1);
		}
		printf("Client connected from port no %d and IP %s\n", ntohs(client.sin_port),inet_ntoa(client.sin_addr));
		close(new);
	}
	close(sock);
}






