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

class ClientTCP{
private: int sock_id;
public: ClientTCP();
		~ClientTCP();
		bool connetti(Address server);
		bool invia(char* msg);
		char* ricevi();
		bool inviaRaw(void* msg, int length);
		void* riceviRaw(int* length);
		bool broadcast(bool b);
};

ClientTCP::ClientTCP(){
	this->sock_id = socket(AF_INET,SOCK_STREAM,0);
}

ClientTCP::~ClientTCP(){
	close(this->sock_id);
}

/*
	return falso se va tutto bene
	vero nel caso si siano verificati degli errori
*/
bool ClientTCP::connetti(Address server){
	// instanzia una connessione mettendo come conn_id il sock_id
	struct sockaddr_in structServer = server.getAddress();
	return connect(this->sock_id,  (struct sockaddr*)&structServer, (socklen_t) sizeof(struct sockaddr_in));
}

bool ClientTCP::invia(char* msg){
	int len = strlen(msg) + 1;
	return this->inviaRaw((void*)msg,len);
}

/*
	return falso se va tutto bene
	vero nel caso si siano verificati degli errori
*/
bool ClientTCP::inviaRaw(void* msg, int length){
	int ret = send(this->sock_id, msg, length,0);
	return (ret != length);
}

char* ClientTCP::ricevi(){
	char* buffer;
	int ret;
	buffer = (char*)this->riceviRaw(&ret);
	if (!buffer) return NULL;
	buffer[ret + 1] = '\0';
	return buffer;
}

void* ClientTCP::riceviRaw(int* length){
	void *buffer = malloc(MAX_MSG + 1);
	*length = recv(this->sock_id, buffer, MAX_MSG, 0);
	if(*length <= 0){
		free(buffer);
		return NULL;
	}
	return buffer;
}

bool ClientTCP::broadcast(bool b){
	int len_so= sizeof(int); int val = b? 1: 0;
	return (!setsockopt(this->sock_id, SOL_SOCKET,SO_BROADCAST, &val,(socklen_t) len_so));
}

