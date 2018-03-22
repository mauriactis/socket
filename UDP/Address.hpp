#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

//Classe che indica un indirizzo
class Address {
	//Propriet� della classe
	private:
		char* ip;
		int port;
	//Prototipi dei metodi implementati in seguito
	public:
		Address(char* ip, int port);
		Address(struct sockaddr_in address);
		Address(const Address& dolly);
		~Address();
		void setIp(char* ip);
		void setPort(int port);
		void setAddress(struct sockaddr_in);
		char* getIp();
		int getPort();
		struct sockaddr_in getAddress();
		void show();
		char* toString();
};


//Metodo costruttore che riceve due parametri
Address::Address(char* ip = "0.0.0.0" , int port = 0 ){
	this->ip=strdup(ip);
	this->port=port;
}

//Metodo costruttore che riceve la struttura
Address::Address(struct sockaddr_in address){
	this->ip = strdup(inet_ntoa(address.sin_addr));
	this->port = htons(address.sin_port);
}

//Metodo che permette di duplicare un' istanza della classe
Address::Address(const Address& dolly){
	this->ip = strdup(dolly.getIp());
	this->port = dolly.getPort();
}

//Metodo che permette di cancellare una struttura, necessario perch� nella aton c'� la malloc
Address::~Address(){
	free(this->ip);
}

/*Non dovrebbe non esserci???*/

//metodo che permette di impostare una stringa ricevuta come parametro com indirizzo ip
void Address::setIp(char* ip){
	free(this->ip);
	this->ip = strdup(ip);
}

//Metodo che permette di modificare la porta
void Address::setPort(int port) {
	this->port = port;
}

void Address::setAddress(struct sockaddr_in addr) {
	if (ip) free(ip);
	ip = strdup(inet_ntoa(addr.sin_addr));
	port = ntohs(addr.sin_port);
}

//L' indirizzo della struttura
char* Address::getIp(){
	return strdup(this->ip);
}

//Metodo che restituisce la porta della struttura
int Address::getPort(){
	return this->port;
}

//Metodo che restituisce una struttura con indirizzo e porta prelevati dalle propriet� della classe
struct sockaddr_in Address::getAddress(){
	struct sockaddr_in address;

	address.sin_family=AF_INET;
	inet_aton(ip, &address.sin_addr);
	address.sin_port = htons(port);
	return address;
}

//Metodo che stampa a video ip e porta
void Address::show(){
	printf("%s:%d\n", ip, port);
}

//Metodo che restituisce una stringa contenente indirizzo ip e porta
char* Address::toString(){
	char s[22];
	//La sprintf � come la printf su una stringa
	sprintf(s, "%s:%d", ip, port);
	return strdup(s);
}
