#include "NameLookup.h"

namespace NameLookup{
NameInfo::NameInfo(std::string hostname){
	getaddrinfo(hostname.c_str(),NULL,NULL,&this->hostInfo);
}

NameInfo::~NameInfo(){
	freeaddrinfo(this->hostInfo);
}

std::string NameInfo::getIP(){
	std::string ip;
	if(this->hostInfo!=NULL){
		if(this->hostInfo->ai_family==AF_INET){
			char str[INET_ADDRSTRLEN];
			ip=inet_ntop(this->hostInfo->ai_family,&reinterpret_cast<sockaddr_in*>(this->hostInfo->ai_addr)->sin_addr,str,INET_ADDRSTRLEN);
		}else{
			char str[INET6_ADDRSTRLEN];
			ip=inet_ntop(this->hostInfo->ai_family,&reinterpret_cast<sockaddr_in6*>(this->hostInfo->ai_addr)->sin6_addr,str,INET6_ADDRSTRLEN);
		}
	}
	return ip;
}

sockaddr* NameInfo::getSocketAddr(){
	return this->hostInfo->ai_addr;
}

addrinfo* NameInfo::getAddressInfo(){
	return this->hostInfo;
}

int NameInfo::getAddressFamily(){
	return this->hostInfo->ai_family;
}

}
