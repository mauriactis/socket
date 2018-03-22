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
#define FILTRA "QUESTO E UN PACCHETTO DA FILTRARE"
#define DEFAULT_FILE "./proxy.log"

void* listen(void*);
char* inputStr();

typedef struct{
	SocketUDP* myself;
	Address* dest;
	FILE* log;
}Params;

class Proxy{
private:
	Address* dest;
	SocketUDP* myself;
	FILE* log;
	pthread_t pid;
	Params params;
public:
	Proxy(int,char*,int,char*);
	~Proxy();
	void start_listen();
	void stop_listen();
};

Proxy::Proxy(int port_in,char* add_out,int port_out, char* log = DEFAULT_FILE){
	this->myself = new SocketUDP(port_in,true);
	this->dest = new Address(add_out,port_out);
	this->log=fopen(log, "wa");
	this->pid = NULL;
}
Proxy::~Proxy(){
	this->stop_listen();
	fclose(this->log);
	delete(this->dest);
	delete(this->myself);
}

void Proxy::start_listen(){
	printf("dentro start\n");
	if(!this->pid){
		this->params.myself = this->myself;
		this->params.dest = this->dest;
		this->params.log = this->log;
		printf("creo thread\n");
		pthread_create(	&this->pid,NULL,
						listen,(void*) &this->params);
	}
}

void Proxy::stop_listen(){
	if(this->pid){
		pthread_cancel(this->pid);
		this->pid = NULL;
	}
}

void* listen(void* p){
	printf("abbiamo aperto il thread\n");
	Params* params = (Params*) p;
	SocketUDP* myself = (SocketUDP*)params->myself;
	Address* dest = (Address*)params->dest;
	FILE* log = (FILE*)params->log;
	Address* mitt; char* msg;
	while(true){
		printf("aspetto messaggio\n");

		msg = myself->ricevi(mitt);
		printf("msg ricevuto %s\n",msg );
		fprintf(log,msg);
		if(strstr(FILTRA,msg) == NULL){
			myself->invia(msg,*dest);
		}
	}
}

int main(int argc, char *argv[]){
	if(argc!=4 && argc!=5){
		printf("USAGE:%s PORT_IN ADD_OUT PORT_OUT [LOG_FILE]\n",argv[0]); return -1;
	}
	Proxy* proxy;
	int port_in = atoi(argv[1]);
	char* add_out = argv[2];
	int port_out = atoi(argv[3]);
	char* log_file;
	if(argc==5){
		log_file = argv[4];
		proxy = new Proxy(port_in,add_out,port_out,log_file);
	}else{
		proxy = new Proxy(port_in,add_out,port_out);
	}
	proxy->start_listen();
	printf("start listen\n");
	while (inputStr()!="exit"){}
	proxy->stop_listen();
	delete(proxy);
	return 0;
}

char* inputStr(){
	char buffer[MAX_STR + 1];
	int i;
	for(i=0;(buffer[i]=getchar())!='\n';i++){}
	buffer[i] = '\0';
	return strdup(buffer);
}
