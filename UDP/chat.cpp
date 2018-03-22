#include "SocketUDP.hpp"
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_STR 4096
#define EXIT_CMD "bye"

char* inputStr(char*);
void* receiver(void*);

int main(int argc, char *argv[]){
	int port;
	char* broadcast_ip;
	SocketUDP* myself;
	Address* server;
	pthread_t receiver_id;

	if(argc!=4){
		printf("USAGE:%s BROADCAST_IP PORT NOME\n",argv[0]);
		return -1;
	}

	broadcast_ip = argv[1];
	port = atoi(argv[2]);
	char* nome_mio = argv[3];
	char* msg;

	server = new Address(broadcast_ip,port);
	myself = new SocketUDP(port);
	myself->broadcast(true);

	pthread_create(&receiver_id,NULL, receiver, (void*) myself);
	while(strcmp(msg = inputStr(nome_mio),EXIT_CMD)){
		myself->invia(msg, *server);
		free(msg);
	}
	free(msg);
	delete(myself);
	delete(server);
	return 0;
}


char* inputStr(char* nome){
	char buffer[MAX_STR + 1];
	int i; int j;
	for(i=0;(buffer[i]=getchar())!='\n';i++){}
	buffer[i] = ';'; i++;
	for(j=0;j<strlen(nome);j++){
		buffer[i + j] = nome[j];
	}
	buffer[i+j] = '\0';
	return strdup(buffer);
}

void* receiver(void* param){
	SocketUDP* myself = (SocketUDP*) param;
	char* msg; 
	Address sender; 
	while(msg = myself->ricevi(&sender)){
		msg[strlen(msg)+1]='\0';
		char nome[30] = "anonimo";
		int i=0; int j=0;
		while(msg[i]!='\0' && msg[i] != ';'){
			i++;
		}
		if(msg[i]!='\0'){
			int conta = i;
			i++; 
		while(msg[i]!='\0'){
			nome[j] = msg[i];
			i++; j++;
		}
		nome[j]='\0';
		msg[conta] = '\0';
		}
		printf("[%s]%s\n",nome, msg);
		free(msg);
	}
}