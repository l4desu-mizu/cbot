#pragma once
#include <string>
#include <arpa/inet.h> //sockaddr_in
#include <netdb.h> //gethostbyname,hostent

namespace NameLookup{
class NameInfo{
	public:
		NameInfo(std::string hostname);
		~NameInfo();
		std::string getIP();
		sockaddr* getSocketAddr();
		addrinfo* getAddressInfo();
		int getAddressFamily();
	private:
		addrinfo* hostInfo;
};
}
