#include "ManagedSocket.h"
#include <unistd.h>
#include <sstream>
#include <iostream>

ManagedSocket::ManagedSocket(std::string hostname, int port):
port(port),
	this->targetAddress=new NameLookup::NameInfo(hostname);
	const addrinfo* tmp=this->targetAddress->getAddressInfo();
	socketfd = socket(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol);
	if(socketfd<0){
		//ERROR
		std::clog << "socketfd error" << std::endl;
	}

	int size;
	if(tmp->ai_family==AF_INET){
		size=sizeof(sockaddr_in);
		std::clog << "ipv4" << std::endl;
	}else if(tmp->ai_family==AF_INET6){
		size=sizeof(sockaddr_in6);
		std::clog << "ipv6" << std::endl;
	}else{
		size=sizeof(sockaddr);
		std::clog << "ipv dunno" << std::endl;
	}
	if(int error=connect(socketfd,tmp->ai_addr,size)<0){//TODO: ->sigpipe (pipe closed here)
		//ERROR
		std::clog << "error " << error << " errno " << errno << "size of" << size << std::endl;
		switch(errno){
			case EBADF: std::clog << "bad fd" << std::endl; break;
			case ENOTSOCK: std::clog << "no socket fd" << std::endl; break;
			case EADDRNOTAVAIL: std::clog << "addr not avail" << std::endl; break;
			case EAFNOSUPPORT: std::clog << "addres family not supported" << std::endl; break;
			case EISCONN: std::clog << "socket already connected" << std::endl; break;
			case ETIMEDOUT: std::clog << "timed out" << std::endl; break;
			case ECONNREFUSED: std::clog << "refused" << std::endl; break;
			case ENETUNREACH: std::clog << "not reachable" << std::endl; break;
			case EADDRINUSE: std::clog << "address in use" << std::endl; break;
			case EINPROGRESS: std::clog << "socket is non blocking" << std::endl; break;
			case EALREADY: std::clog << "connection pending and non blocking" << std::endl; break;
			default: std::clog << "none of the above" << std::endl;
		}
		std::clog << "connect error" << std::endl;
	}
}

ManagedSocket::~ManagedSocket(){
	std::clog << "deleting socket" << std::endl;
	disconnect();
	if(this->targetAddress!=NULL){
		delete this->targetAddress;
		this->targetAddress=NULL;
	}
}

void ManagedSocket::disconnect(){
	if(socketfd>=0){
		shutdown(socketfd,2);//stop sending and receiving
		socketfd=-1;
	}
}

int ManagedSocket::send(std::string message){
	int error=0;
	//save the number of bytes in error (or -1 for error)
	error=::send(socketfd,message.c_str(),message.size(),0);//use extern C socket send
	if(error<=0){
		//ERROR
		std::clog << "socket send string error" << std::endl;
	}
	return error;
}

int ManagedSocket::send(const char* message,int length){
	int error=0;
	//save the number of bytes in error (or -1 for error)
	std::clog << "none ssl" << std::endl;
	error=::send(socketfd,message,length,0);//use extern C socket send
	if(error<=0){
		//ERROR
		std::clog << "socket send error" << std::endl;
	}
	return error;
}

std::string ManagedSocket::receive(){
	std::stringstream messageBuilder;
	const int bufferSize=256;//arbitrary buffersize (chosen by fair dice roll, not)
	int bytesRead=bufferSize;
	char buffer[bufferSize];
	while(bytesRead>=bufferSize){ //receive until less than bufferSize bytes were received
		bytesRead=read(socketfd,&buffer,bufferSize);
		if(bytesRead>0){//only add string if something was actually received
			messageBuilder<<buffer;
		}else{
			break;
		}
	}
	return messageBuilder.str();
}
