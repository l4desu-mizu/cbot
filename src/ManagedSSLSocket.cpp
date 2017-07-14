#include "ManagedSSLSocket.h"
#include "openssl/bio.h"
#include <sstream>
#include <iostream>

ManagedSSLSocket::ManagedSSLSocket(std::string hostname, int port){
	initSSL();
	bio = BIO_new_connect(createTargetHost(hostname,port).c_str());
	if(bio==NULL){
		//ERROR
		std::clog << "bio create failed" << std::endl;
	}
	//connect with handshake
	if(BIO_do_connect(bio)<=0){
		//ERROR
		std::clog << "do connect failed" << std::endl;
	}
}

ManagedSSLSocket::ManagedSSLSocket(std::string hostname, int port, std::string certFile, std::string certKeyFile):
certFile(certFile),
certKeyFile(certKeyFile){
	initSSL();
	sslContext=SSL_CTX_new(TLS_client_method());
	SSL_CTX_set_options(sslContext,SSL_OP_SINGLE_DH_USE);

	//use this for a secured connection with certified host
	//if(! SSL_CTX_load_verify_locations(sslContext, NULL, "/etc/ssl/certs/")){
	//	/* Handle failed load here */
	//}

	//supply your own certs
	int use_cert;
	int use_key;

	use_cert = SSL_CTX_use_certificate_file(sslContext, certFile.c_str(), SSL_FILETYPE_PEM);
	use_key = SSL_CTX_use_PrivateKey_file(sslContext, certKeyFile.c_str(), SSL_FILETYPE_PEM);
	if(use_cert!=1){
		//ERROR
		std::clog << "use_cert error" << std::endl;
	}
	if(use_key!=1){
		//ERROR
		std::clog << "use_key error" << std::endl;
	}
	bio=BIO_new_ssl_connect(sslContext);
	BIO_get_ssl(bio, &ssl);
	SSL_set_mode(ssl,SSL_MODE_AUTO_RETRY);
	BIO_set_conn_hostname(bio,createTargetHost(hostname,port).c_str());

	if(BIO_do_connect(bio) <= 0){
		//ERROR
		/* Handle failed connection */
		std::clog << "do connect failed" << std::endl;
	}
	if(SSL_get_verify_result(ssl) != X509_V_OK){
		//SSL verification
		/* Handle the failed verification */
		std::clog << "ssl verify failed" << std::endl;
	}
}

ManagedSSLSocket::~ManagedSSLSocket(){
	if(sslContext!=NULL){
		SSL_CTX_free(sslContext);
		sslContext=NULL;
	}
	if(bio!=NULL){
		BIO_free_all(bio);
		bio=NULL;
	}
}

void ManagedSSLSocket::initSSL(){
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
}

std::string ManagedSSLSocket::createTargetHost(std::string host, int port){
	std::stringstream targetHost;
	targetHost << host << ":" << port;
	return targetHost.str();
}

void ManagedSSLSocket::disconnect(){
	BIO_reset(bio);
}

int ManagedSSLSocket::send(std::string message){
	return this->send(message.c_str(),message.size());
}

int ManagedSSLSocket::send(const char* message,int length){
	int res;
	res=BIO_write(bio,message,length);
	if(res<=0){
		//ERROR
		std::clog << "writing failed" << std::endl;
		if(! BIO_should_retry(bio)){
			//Handle failed write
			std::clog << "rewriting failed" << std::endl;
		}
	}
	return res;
}

std::string ManagedSSLSocket::receive(){
	const short length=256;
	char buff[length];
	int read=length;
	std::stringstream out;

	while(read>=length){
		read=BIO_read(bio, buff,length);
		if(read>0){
			out << buff;
		}else if(read==0){
			//No data or closed
			std::clog << "no data read or socket closed" << std::endl;
		}else{
			//ERROR occured
			std::clog << "some reading error, trying again" << std::endl;
			if(! BIO_should_retry(bio)){
				//Handle failed read
				std::clog << "rereading failed" << std::endl;
			}
		}
	}
	return out.str();
}
