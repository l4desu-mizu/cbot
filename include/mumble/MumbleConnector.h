#pragma once
#include "Connector.h"
#include "ManagedSSLSocket.h"
#include <thread>

class MumbleConnector: public Connector{
	public:
		MumbleConnector(std::string host,int port=64738); //mumble default port
		~MumbleConnector();
		void sendTextMessage(std::string message);
	private:
		ManagedSSLSocket* socket=NULL;
		bool receiveLoopRuns=false;
		std::thread receiveThread;
		void receiveLoop();
		void connect();
		void auth();
		void disconnect();
};
