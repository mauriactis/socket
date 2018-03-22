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

class SocketTCP{
protected:	int sock_id;

public:		SocketTCP();											// socket()
			~SocketTCP();											// close()
			bool 			broadcast(bool b);						// setsockopt()
}


class Connessione{
private:	int connId;
			Address* address;
			bool conn_server;

public:		Connessione(int connId, Address* address);
			~Connessione();											// shutdown()
			bool 			invia(char* msg);						
			bool 			inviaRaw(void* msg, int length);		// send()
			char* 			ricevi();
			void* 			riceviRaw(int* length);					// recv()
};

class ServerTCP: public SocketTCP{
public: 	ServerTCP(int port = 0, bool loopback = false);			// bind(), listen()
			~ServerTCP();
			Connessione* 	accetta();								// accept()
};

class ClientTCP: public SocketTCP{
private:	Connessione* connessione;

public: 	ClientTCP();
			~ClientTCP();
			bool 			connetti(Address server);				// connect()
			bool 			invia(char* msg);
			bool 			inviaRaw(void* msg, int length);
			char* 			ricevi();
			void* 			riceviRaw(int* length);
};

Connessione::Connessione(int connId, Address* address){
	this->connId = connId;
	this->address = new Address(address);
}

Connessione::~Connessione(){
	delete(this->address);
	shutdown(this->connId);
}