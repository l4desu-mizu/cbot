#include "SSLClientSocket.h"
#include "Connector.h"
#include "SimpleMumbleBot.h"
#include "mumble/MumbleConnector.h"
#include <string>
#include <iostream>
#include <exception>

#define MUMBLE_DEFAULT_PORT 64738
#define NAME "Test"
int main(){
	std::string s = "sneaky-ninja.de";
	s = "localhost";
	std::cout << "creating connector" << std::endl;
	SSLClientSocket mySocket(s,MUMBLE_DEFAULT_PORT);
	Connector* c = new MumbleConnector(&mySocket,NAME);
	try{
		SimpleMumbleBot b(c);
		b.preRun();
		while(1){
			b.run();
		}
	}catch(std::runtime_error& error){
		std::cout << "connection failed, " << error.what() << std::endl;
		std::cout << "shutting down" << std::endl;
	}
	delete c;
}
