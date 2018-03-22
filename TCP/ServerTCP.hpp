#include "./Address.hpp"
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define LOOPBACK "127.0.0.1"
#define MYSELF_IP "0.0.0.0"

#define MAX_MSG 4096

class ServerTCP{
	private: int sock_id; int con_id = -1;
	public: ServerTCP();
		~ServerTCP();
		Address* accetta();
		bool invia(char* msg);
		char* ricevi();
		bool inviaRaw(void* msg, int length);
		void* riceviRaw(int* length);
		bool broadcast(bool b);
		void chiudi();
};

ServerTCP::ServerTCP(int port = 0, bool loopback = false){
	this->sock_id = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(loopback?LOOPBACK:MYSELF_IP, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i = 0; i < 8; i++) {
		myself.sin_zero[i] = 0;
	}
	bind(sock_id, (struct sockaddr*)&myself, (socklen_t)sizeof(struct sockaddr_in));
	listen(this->sock_id, 1);
}

ServerTCP::~ServerTCP(){
	if(con_id!=-1){
		this->chiudi();
	}
	close(sock_id);
}

Address* ServerTCP::accetta(){
	Address* client; struct sockaddr_in mitt;
	int length = sizeof(struct sockaddr_in);
	this->con_id = accept(this->sock_id, (struct sockaddr*)&mitt, (socklen_t*)&length);
	if(this->con_id == -1){
		return NULL;
	}
	client = new Address(mitt);
	return client;
}

bool ServerTCP::invia(char* msg){
	int len = strlen(msg) + 1;
	return this->inviaRaw((void*)msg,len);
}

char* ServerTCP::ricevi(){
	char* buffer;
	int ret;
	buffer = (char*)this->riceviRaw(&ret);
	if (!buffer) return NULL;
	buffer[ret + 1] = '\0';
	return buffer;
}

bool ServerTCP::inviaRaw(void* msg, int length){
	int ret = send(this->con_id, msg, length,0);
	return (ret != length);
}

void* ServerTCP::riceviRaw(int* length){
	void *buffer = malloc(MAX_MSG + 1);
	*length = recv(this->con_id, buffer, MAX_MSG, 0);
	if(*length <= 0){
		free(buffer);
		return NULL;
	}
	return buffer;
}

bool ServerTCP::broadcast(bool b){
	int len_so= sizeof(int); int val = b? 1: 0;
	return (!setsockopt(this->sock_id, SOL_SOCKET,SO_BROADCAST, &val,(socklen_t) len_so));
}

void ServerTCP::chiudi(){
	if(this->con_id!=-1)
		shutdown(this->con_id, SHUT_RDWR);
	this->con_id = -1;
}