#include "SSLClientSocket.h"
#include "openssl/bio.h"
#include <sstream>
#include <iostream>

SSLClientSocket::SSLClientSocket(std::string hostname, int port, bool useEncrypt):
certFile(""),
certKeyFile(""),
isNonBlocking(false){
	if(useEncrypt){
		initSSLSocket(hostname,port);
	}else{
		SSL_load_error_strings();
		ERR_load_BIO_strings();
		bio = BIO_new_connect(createTargetHost(hostname,port).c_str());
		if(bio==NULL){
				//ERROR
		}
	}
}

SSLClientSocket::SSLClientSocket(std::string hostname, int port, std::string certFile, std::string certKeyFile):
certFile(certFile),
certKeyFile(certKeyFile),
isNonBlocking(false){
	initSSLSocket(hostname,port);
}

SSLClientSocket::~SSLClientSocket(){
	if(sslContext!=NULL){
		SSL_CTX_free(sslContext);
		sslContext=NULL;
	}
	if(bio!=NULL){
		BIO_free_all(bio);//frees ssl
		bio=NULL;
	}
}

void SSLClientSocket::initSSLSocket(const std::string& hostname, const int& port){
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	sslContext=SSL_CTX_new(TLS_client_method());
	SSL_CTX_set_options(sslContext,SSL_OP_SINGLE_DH_USE);

	//use this for a secured connection with certified host
	//if(! SSL_CTX_load_verify_locations(sslContext, NULL, "/etc/ssl/certs/")){
	//	/* Handle failed load here */
	//}

	//supply your own certs
	if(certFile.size()>0&&certKeyFile.size()>0){
		bool use_cert_err;
		bool use_key_err;

		use_cer_errt = (SSL_CTX_use_certificate_file(sslContext, certFile.c_str(), SSL_FILETYPE_PEM)!=1);
		use_key_err = (SSL_CTX_use_PrivateKey_file(sslContext, certKeyFile.c_str(), SSL_FILETYPE_PEM)!=1);

		if(use_cert_err||use_key_err){
			//ERROR
			std::stringstream builder;
			builder << "Your";
			if(use_cert_err){
				builder << " certificate file (path) is erroneous.";
			}else if(use_key_err){
				builder << " certificate key file (path) is erroneous.";
			}else{
				builder << " the given cert file (paths) are erroneous."
			}
			throw std::runtime_error(builder.str());
		}
	}
	bio=BIO_new_ssl_connect(sslContext);
	BIO_set_close(bio,BIO_CLOSE); //BIO should be closed when freed
	BIO_set_nbio(bio,isNonBlocking); //set socket to blocking
	BIO_get_ssl(bio, &ssl);
	SSL_set_mode(ssl,SSL_MODE_AUTO_RETRY);
	BIO_set_conn_hostname(bio,createTargetHost(hostname,port).c_str());

	if(BIO_do_connect(bio) <= 0){
		//ERROR
		/* Handle failed connection */
		std::clog << "do connect failed" << std::endl;
		throw std::runtime_error("Target not reachable");
	}
	if(SSL_get_verify_result(ssl) != X509_V_OK){
		//SSL verification
		/* Handle the failed verification */
		std::clog << "ssl verify failed" << std::endl;
	}
}

std::string SSLClientSocket::createTargetHost(const std::string& host,const int& port){
	std::stringstream targetHost;
	targetHost << host << ":" << port;
	return targetHost.str();
}

bool SSLClientSocket::connect(){
	//connect with handshake
	return (BIO_do_connect(bio)>0);
}
void SSLClientSocket::disconnect(){
	BIO_reset(bio);
}

int SSLClientSocket::send(std::string message){
	return this->send(message.c_str(),message.size());
}

int SSLClientSocket::send(const char* message,const int length){
	int res;
	res=BIO_write(bio,message,length);
	if(res<=0){
		//ERROR
		std::clog << "writing failed" << std::endl;
		if(! BIO_should_retry(bio)){
			//Handle failed write
			std::clog << "final writing failed" << std::endl;
			throw std::runtime_error("Socket dead.");
		}
	}
	return res;
}

std::string SSLClientSocket::receive(){
	const short length=256;
	char buff[length];
	int read=length;
	std::stringstream out;

	while(read>=length){
		read=BIO_read(bio, buff,length);
		if(read>0){
			out.write(buff,read);
		}else if(read==0){
			//No data or closed
			std::clog << "no data read or socket closed" << std::endl;
		}else{
			//ERROR occured
			std::clog << "some reading error, trying again" << std::endl;
			if(! BIO_should_retry(bio)){
				//Handle failed read
				std::clog << "reading failed" << std::endl;
				throw std::runtime_error("Socket dead.");
			}
		}
	}
	out.flush();
	return out.str();
}
int SSLClientSocket::receive(char* buff,const int length){
	int read;
	while(read>=length){
		read=BIO_read(bio, buff,length);
		if(read==0){
			//No data or closed
			std::clog << "no data read or socket closed" << std::endl;
		}else if(read<0){
			//ERROR occured
			std::clog << "some reading error, trying again" << std::endl;
			if(! BIO_should_retry(bio)){
				//Handle failed read
				std::clog << "reading failed" << std::endl;
				throw std::runtime_error("Socket dead.");
			}
		}
		return read;
	}
}
