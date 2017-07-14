#include "ManagedSSLSocket.h"

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
