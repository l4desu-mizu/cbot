#include "mumble/MumbleConnector.h"
#include "mumble.pb.h"
#include <iostream>

MumbleConnector::MumbleConnector(std::string host,int port){
	//open socket
	//handshake
	//socket=new ManagedSSLSocket(host,port,"./cert/mumble_bot.cert","./cert/mumble_bot_cert.key");//presumably done...
	socket=new ManagedSSLSocket(host,port);//presumably done...
	receiveLoopRuns=true;
	receiveThread=std::thread(&MumbleConnector::receiveLoop,this);
	std::cout << "socket created" << std::endl;
	std::cout << "connecting" << std::endl;
	connect();
	std::cout << "authenticating" << std::endl;
	auth();
}

MumbleConnector::~MumbleConnector(){
	receiveLoopRuns=false;
	receiveThread.join();
	if(socket!=NULL){
		delete socket;
		socket=NULL;
	}
}

void MumbleConnector::sendTextMessage(std::string message){
}

void MumbleConnector::connect(){
	//version exchange (skippable)
	const int versionBytes=0b00000000000000010000001000011101;//const version id, connect as current stable (13.07.17)
	int bufferSize;
	std::cout << "connected1" << std::endl;
	MumbleProto::Version myVersion; //MumbleProto::Version myVersion=MumbleProto::Version();
	myVersion.set_version(versionBytes);
	bufferSize=myVersion.ByteSize();
	char buffer[bufferSize];
	myVersion.SerializeToArray(&buffer,bufferSize);
	std::cout << "connected2" << std::endl;
	socket->send(buffer,bufferSize);
}

void MumbleConnector::receiveLoop(){
	while(receiveLoopRuns){
		std::string tmp = socket->receive();
		if(tmp.size()>0){
			std::cout << tmp << std::endl;
		}
	}
}

void MumbleConnector::auth(){
	int bufferSize;
	MumbleProto::Authenticate authMessage;
	authMessage.set_username("Test");
	bufferSize=authMessage.ByteSize();
	char buffer[bufferSize];
	authMessage.SerializeToArray(&buffer,bufferSize);
	socket->send(buffer,bufferSize);
}

void MumbleConnector::disconnect(){
}
