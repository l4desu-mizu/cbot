#include "Connector.h"
#include "mumble/MumbleConnector.h"
#include "ManagedSocket.h"
#include <string>
#include <iostream>

int main(){
	std::string s = "sneaky-ninja.de";
	s = "localhost";
	std::cout << "creating connector" << std::endl;
	Connector* c = new MumbleConnector(s);
	std::cout << "done" << std::endl;
	delete c;
}
