#include "./Address.hpp"
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <std/list> // standard template library
#include <errno.h>

// il template è un tipo generico, ad esempio List è di tipo generico (come ArrayList), poi tra parentesi <> si mette il tipo di dato giusto

#define LOOPBACK "127.0.0.1"
#define MYSELF_IP "0.0.0.0"

#define MAX_CONN 50
#define MAX_MSG 4096

class SocketTCP{
protected:	int sock_id;

public:		SocketTCP();											// socket()
			~SocketTCP();											// close()
			bool 			broadcast(bool b);						// setsockopt()
};

SocketTCP::SocketTCP(){
	this->sock_id = socket(AF_INET,SOCK_STREAM,0);
}

SocketTCP::~SocketTCP(){
	close(this->sock_id);
}

bool SocketTCP::broadcast(bool b){
	int len_so= sizeof(int); int val = b? 1: 0;
	return (!setsockopt(this->sock_id, SOL_SOCKET,SO_BROADCAST, &val,(socklen_t) len_so));
}

/*
*		CONNESSIONE TCP
*		CONNESSIONE TCP
*		CONNESSIONE TCP
*		CONNESSIONE TCP
*		CONNESSIONE TCP
*		CONNESSIONE TCP
*/


class Connessione{
private:	int connId;
			Address* address;
			bool conn_server;

public:		Connessione(int connId, Address* address, bool conn_server);
			~Connessione();											// shutdown()
			bool 			invia(char* msg);						
			bool 			inviaRaw(void* msg, int length);		// send()
			char* 			ricevi();
			void* 			riceviRaw(int* length);					// recv()
};


Connessione::Connessione(int connId, Address* address, bool conn_server){
	this->connId = connId;
	this->address = new Address(*address);
	this->conn_server = conn_server;
}

Connessione::~Connessione(){
	delete(this->address);
	if(this->conn_server){
		shutdown(this->connId, SHUT_RDWR);
	}
}

bool Connessione::invia(char* msg){
	int len = strlen(msg) + 1;
	printf("invia connessione %i\n", len);
	return this->inviaRaw((void*)msg,len);
}

bool Connessione::inviaRaw(void* msg, int length){
	printf("inviaraw connessione\n");
	int ret = send(this->connId, msg, length,0);
	printf("ret %i\n",ret );
	return (ret != length);
}

char* Connessione::ricevi(){
	char* buffer;
	int ret;
	buffer = (char*)this->riceviRaw(&ret);
	if (!buffer) return NULL;
	buffer[ret + 1] = '\0';
	return buffer;
}

void* Connessione::riceviRaw(int* length){
	void *buffer = malloc(MAX_MSG + 1);
	*length = recv(this->connId, buffer, MAX_MSG, 0);
	if(*length <= 0){
		free(buffer);
		return NULL;
	}
	return buffer;
}

/*
*		SERVER TCP
*		SERVER TCP
*		SERVER TCP
*		SERVER TCP
*		SERVER TCP
*		SERVER TCP
*/

class ServerTCP: public SocketTCP{
public: 	ServerTCP(int port, bool loopback);						// bind(), listen()
			~ServerTCP();
			Connessione* 	accetta();								// accept()
};

ServerTCP::ServerTCP(int port = 0, bool loopback = false):SocketTCP(){
	struct sockaddr_in myself;
	// valorizza struttura indirizzo 
	myself.sin_family = AF_INET;
	inet_aton(loopback?LOOPBACK:MYSELF_IP, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i = 0; i < 8; i++) {
		myself.sin_zero[i] = 0;
	}

	/*
	*	BIND DEL SERVER
	*/
	bind(this->sock_id, (struct sockaddr*)&myself, (socklen_t)sizeof(struct sockaddr_in));

	/*
	*	LIMITA CONNESSIONI
	*/
	listen(this->sock_id, MAX_CONN);
}

/*
*	RICHIAMA DA SOLO IL DISTRUTTORE DI SOCKETTCP() COSÌ CHIUDE IL SOCKET
*/
ServerTCP::~ServerTCP(){

}

/*
*	ACCETTA 
*/
Connessione* ServerTCP::accetta(){
	Address* client; struct sockaddr_in mitt;
	int length = sizeof(struct sockaddr_in);
	int con_id = accept(this->sock_id, (struct sockaddr*)&mitt, (socklen_t*)&length);
	if(con_id == -1){
		return NULL;
	}
	client = new Address(mitt);
	Connessione* conn = new Connessione(con_id, client, true);
	return conn;
}

/*
*		CLIENT TCP
*		CLIENT TCP
*		CLIENT TCP
*		CLIENT TCP
*		CLIENT TCP
*		CLIENT TCP
*/

class ClientTCP: public SocketTCP{
private:	Connessione* 	connessione;


public: 	ClientTCP();
			~ClientTCP();
			bool 			connetti(Address server);				// connect()
			bool 			invia(char* msg);
			bool 			inviaRaw(void* msg, int length);
			char* 			ricevi();
			void* 			riceviRaw(int* length);
};


ClientTCP::ClientTCP(): SocketTCP(){

}

ClientTCP::~ClientTCP(){
	delete(this->connessione);
}

bool ClientTCP::connetti(Address server){
	struct sockaddr_in structServer = server.getAddress();
	bool ret = connect(this->sock_id,  (struct sockaddr*)&structServer, (socklen_t) sizeof(struct sockaddr_in));
	if(ret){
		this->connessione = new Connessione(this->sock_id, &server, false);
	}
	return ret;
}

bool ClientTCP::invia(char* msg){
	printf("invia client\n");
	return this->connessione->invia(msg);
}

bool ClientTCP::inviaRaw(void* msg, int length){
	return this->connessione->inviaRaw(msg,length);
}

char* ClientTCP::ricevi(){
	return this->connessione->ricevi();
}

void* ClientTCP::riceviRaw(int* length){
	return this->connessione->riceviRaw(length);
}