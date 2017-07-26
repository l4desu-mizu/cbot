#pragma once
#include <openssl/ssl.h>
#include <string>

class SSLClientSocket{
	public:
		SSLClientSocket(std::string hostname, int port, bool useEncrypt=true);
		SSLClientSocket(std::string hostname, int port, std::string certFile, std::string certKeyFile);
		~SSLClientSocket();
		bool connect();
		void disconnect();
		int send(std::string message);
		int send(const char* message,const int length);
		std::string receive();
		int receive(char* buff,const int length);
	private:
		const bool isNonBlocking;
		const std::string certFile;
		const std::string certKeyFile;
		BIO* bio;
		SSL* ssl=NULL;
		SSL_CTX* sslContext=NULL;
		void initSSLSocket(const std::string& hostname,const int& port);
		std::string createTargetHost(const std::string& host,const int& port);
};
