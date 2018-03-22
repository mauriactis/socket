#include "socketTCP.hpp"


int main(int argc, char const *argv[])
{
	ClientTCP* client = new ClientTCP();
	Address* server = new Address("127.0.0.1",8080);
	if(client->connetti(*server)){
		printf("connesso.\n");
	}
	printf("invio ciao\n");
	client->invia("ciao");
	printf("inviato ciao\n");
	delete(client);
	printf("disconnesso.\n");
	return 0;
}