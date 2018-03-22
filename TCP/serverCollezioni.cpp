#include "socketTCP.hpp"
#include <list>

int main(int argc, char const *argv[])
{
	if(argc!=4){
		printf("USAGE:%s PORT LOOPBACK MSG\n",argv[0]);
	}

	list<Connessione*>* connessioni = new list<Connessione*>;
	bool loop = false;
	if(atoi(argv[2])==0){
		loop = true;
	}
	Server* server = new Server(atoi(argv[1]), loop);
	connessioni->insert(server->accetta());
	connessioni->insert(server->accetta());
	connessioni
	return 0;
}