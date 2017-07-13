#include "ManagedSocket.h"
#include <openssl/err.h>
#include <unistd.h>
#include <sstream>

ManagedSocket::ManagedSocket(std::string hostname, int port):
ManagedSocket(hostname,port,NULL,NULL){}

ManagedSocket::ManagedSocket(std::string hostname, int port, std::string certFile="", std::string certKeyFile=""):
port(port),
certFile(certFile),
certKeyFile(certKeyFile){
	this->targetAddress=new NameLookup::NameInfo(hostname);
	const addrinfo* tmp=this->targetAddress->getAddressInfo();
	socketfd = socket(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol);
	if(socketfd<0){
		//ERROR
	}

	if(connect(socketfd,tmp->ai_addr,sizeof(tmp->ai_addr))<0){
		//ERROR
	}

	//init ssl
	if(certFile.size()>0&&certKeyFile.size()>0){ //use ssl?
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
		}
		if(use_key!=1){
			//ERROR
		}
		ssl=SSL_new(sslContext);
		SSL_set_fd(ssl,socketfd);
		ssl_err=SSL_accept(ssl);
		if(ssl_err<=0){
			//ERROR
			disconnect();
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
	}
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
