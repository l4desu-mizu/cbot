#include "SSLClientSocket.h"
#include "Connector.h"
#include "SimpleMumbleBot.h"
#include "mumble/MumbleConnector.h"
#include <string>
#include <iostream>

#define MUMBLE_DEFAULT_PORT 64738
#define NAME "Test"
int main(){
	std::string s = "sneaky-ninja.de";
	s = "localhost";
	std::cout << "creating connector" << std::endl;
	SSLClientSocket mySocket(s,MUMBLE_DEFAULT_PORT);
	Connector* c = new MumbleConnector(&mySocket,NAME);
	SimpleMumbleBot b(c);
	b.preRun();
	std::cout << "done" << std::endl;
	while(1){
		b.run();
	}
	delete c;
}
