#include "ManagedSocket.h"
#include <openssl/err.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

ManagedSocket::ManagedSocket(std::string hostname, int port):
ManagedSocket(hostname,port,"",""){}

ManagedSocket::ManagedSocket(std::string hostname, int port, std::string certFile, std::string certKeyFile):
port(port),
certFile(certFile),
certKeyFile(certKeyFile){
	this->targetAddress=new NameLookup::NameInfo(hostname);
	const addrinfo* tmp=this->targetAddress->getAddressInfo();
	socketfd = socket(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol);
	if(socketfd<0){
		//ERROR
		std::cout << "socketfd error" << std::endl;
	}

	//init ssl
	if(certFile.size()>0&&certKeyFile.size()>0){ //use ssl?
		std::cout << "init ssl" << std::endl;
		int use_cert;
		int use_key;
		int ssl_err;

		SSL_load_error_strings();
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		sslContext=SSL_CTX_new(TLS_method());
		SSL_CTX_set_options(sslContext,SSL_OP_SINGLE_DH_USE);
		use_cert = SSL_CTX_use_certificate_file(sslContext, certFile.c_str(), SSL_FILETYPE_PEM);
		use_key = SSL_CTX_use_PrivateKey_file(sslContext, certKeyFile.c_str(), SSL_FILETYPE_PEM);
		if(use_cert!=1){
			//ERROR
			std::cout << "use_cert error" << std::endl;
		}
		if(use_key!=1){
			//ERROR
			std::cout << "use_key error" << std::endl;
		}
		ssl=SSL_new(sslContext);
		SSL_set_fd(ssl,socketfd);
		std::cout << "fail here" << std::endl;
		SSL_set_connect_state(ssl);
		ssl_err=SSL_connect(ssl);
		std::cout << "no" << std::endl;
		if(ssl_err<=0){
			//ERROR
			std::cout << "SSL error" << std::endl;
			disconnect();
		}
	}else{
		int size;
		if(tmp->ai_family==AF_INET){
			size=sizeof(sockaddr_in);
			std::cout << "ipv4" << std::endl;
		}else if(tmp->ai_family==AF_INET6){
			size=sizeof(sockaddr_in6);
			std::cout << "ipv6" << std::endl;
		}else{
			size=sizeof(sockaddr);
			std::cout << "ipv dunno" << std::endl;
		}
		if(int error=connect(socketfd,tmp->ai_addr,size)<0){//TODO: ->sigpipe (pipe closed here)
			//ERROR
			std::cout << "error " << error << " errno " << errno << "size of" << size << std::endl;
			switch(errno){
				case EBADF: std::cout << "bad fd" << std::endl; break;
				case ENOTSOCK: std::cout << "no socket fd" << std::endl; break;
				case EADDRNOTAVAIL: std::cout << "addr not avail" << std::endl; break;
				case EAFNOSUPPORT: std::cout << "addres family not supported" << std::endl; break;
				case EISCONN: std::cout << "socket already connected" << std::endl; break;
				case ETIMEDOUT: std::cout << "timed out" << std::endl; break;
				case ECONNREFUSED: std::cout << "refused" << std::endl; break;
				case ENETUNREACH: std::cout << "not reachable" << std::endl; break;
				case EADDRINUSE: std::cout << "address in use" << std::endl; break;
				case EINPROGRESS: std::cout << "socket is non blocking" << std::endl; break;
				case EALREADY: std::cout << "connection pending and non blocking" << std::endl; break;
				default: std::cout << "none of the above" << std::endl;
			}
			std::cout << "connect error" << std::endl;
		}
	}
}

ManagedSocket::~ManagedSocket(){
	disconnect();
	delete this->targetAddress;

	//deinitialize ssl if used
	if(sslContext!=NULL){
		SSL_CTX_free(this->sslContext);
		ERR_free_strings();
		EVP_cleanup();
		sslContext=NULL;
	}
	std::cout << "deleting socket" << std::endl;
}

void ManagedSocket::disconnect(){
	if(this->ssl!=NULL){
		SSL_shutdown(this->ssl);
		SSL_free(this->ssl);
		this->ssl=NULL;
	}
	if(socketfd>=0){
		shutdown(socketfd,2);//stop sending and receiving
		socketfd=-1;
	}
}

int ManagedSocket::send(std::string message){
	int error=0;
	if(ssl!=NULL){
		//save error state >0 means success
		error=SSL_write(ssl,message.c_str(),message.size());
	}else{
		//save the number of bytes in error (or -1 for error)
		error=::send(socketfd,message.c_str(),message.size(),0);//use extern C socket send
	}
	if(error<=0){
		//ERROR
		std::cout << "socket send string error" << std::endl;
	}
	return error;
}

int ManagedSocket::send(const char* message,int length){
	int error=0;
	if(ssl!=NULL){
		//save error state >0 means success
		std::cout << "ssl" << std::endl;
		error=SSL_write(ssl,message,length);
	}else{
		//save the number of bytes in error (or -1 for error)
		std::cout << "none ssl" << std::endl;
		error=::send(socketfd,message,length,0);//use extern C socket send
	}
	if(error<=0){
		//ERROR
		std::cout << "socket send error" << std::endl;
	}
	std::cout << "all done sending" << std::endl;
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
