#include "mumble/MumbleConnector.h"
#include <google/protobuf/text_format.h>
#include <iostream>
#include <sstream>
#include <algorithm>

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
	std::cout << "connector ends" << std::endl;
	receiveLoopRuns=false;
	receiveThread.join();
	if(socket!=NULL){
		delete socket;
		socket=NULL;
	}
}

void MumbleConnector::sendTextMessage(std::string message){
}

void MumbleConnector::handleReceives(){
	while(receiveLoopRuns){
		std::string tmp=socket->receive();
		if(tmp.size()>0){
			std::string header_str=tmp.substr(0,HEADER_SIZE);
			const MumbleHeader* netHeader = reinterpret_cast<const MumbleHeader*>(header_str.c_str());
			const MumbleHeader header(ntohs(netHeader->getMessageType()),ntohl(netHeader->getMessageLength()));
			std::string message=tmp.substr(HEADER_SIZE+1,header.getMessageLength());
			dispatchMessage(header,message);
		}else{
				std::cout << "none" << std::endl;
		}
	}
}

void MumbleConnector::connect(){
	//version exchange (skippable)
	const int versionBytes=0b00000000000000010000001000011101;//const version id, connect as current stable (13.07.17)
	const std::string release = "1.2.29";
	MumbleProto::Version myVersion;
	std::string buffer;
	myVersion.set_version(versionBytes);
	myVersion.set_release(release.c_str());
	myVersion.SerializeToString(&buffer);
	sendProtoMessage(MumbleMessageType::Version,buffer);
	std::cout << "connected2" << std::endl;
}

void MumbleConnector::auth(){
	MumbleProto::Authenticate authMessage;
	authMessage.set_username("Test");
	std::cout << "authing" << std::endl;
	std::string buffer;
	authMessage.SerializeToString(&buffer);
	sendProtoMessage(MumbleMessageType::Authenticate,buffer);
	std::cout << "authed" << std::endl;
}

void MumbleConnector::disconnect(){
}

void MumbleConnector::sendProtoMessage(const MumbleMessageType& msgType,const std::string& message){
	std::stringstream protoMessage;
	const MumbleHeader header(msgType,(uint32_t)message.size());
	header.writeToStreamInNetworkFormat(protoMessage);
	protoMessage << message;
	protoMessage.flush();
	socket->send(protoMessage.str());
}

void MumbleConnector::dispatchMessage(const MumbleHeader& header, const std::string& message){
	switch(header.getMessageType()){
		case MumbleMessageType::Version:{
			std::clog<< "version: " <<std::endl;
			MumbleProto::Version v;
			v.ParseFromString(message);
			std::string versionDisplay;
			google::protobuf::TextFormat::PrintToString(v,&versionDisplay);
			std::clog<< versionDisplay <<std::endl;
			break;
		}
		case MumbleMessageType::Ping:{
			std::clog<< "ping" <<std::endl;
			MumbleProto::Ping p;
			p.ParseFromString(message);
			std::thread(&MumbleConnector::handlePing,this,std::ref(p)).detach();
			break;
		}
		case MumbleMessageType::Reject:{
			 std::clog<< "Reject" <<std::endl;
			break;
		}
		case MumbleMessageType::ServerSync:{
			 std::clog<< "ServerSync" <<std::endl;
			break;
		}
		case MumbleMessageType::ChannelState:{
			 std::clog<< "ChannelState" <<std::endl;
			break;
		}
		case MumbleMessageType::UserState:{
			 std::clog<< "UserState" <<std::endl;
			break;
		}
		case MumbleMessageType::TextMessage:{
			std::clog<< "TextMessage" <<std::endl;
			break;
		}
		case MumbleMessageType::PermissionDenied:{
			std::clog<< "PermissionDenied" <<std::endl;
			break;
		}
		case MumbleMessageType::QueryUsers:{
			std::clog<< "QueryUsers" <<std::endl;
			break;
		}
		case MumbleMessageType::CryptSetup:{
			std::clog<< "CryptSetup" <<std::endl;
			MumbleProto::CryptSetup cs;
			cs.ParseFromString(message);
			handleCrypto(cs);
			std::thread(&MumbleConnector::handleCrypto,this,std::ref(cs)).detach();
			break;
		}
		case MumbleMessageType::UserList:{
			std::clog<< "UserList" <<std::endl;
			break;
		}
		case MumbleMessageType::PermissionQuery:{
			std::clog<< "PermissionQuery" <<std::endl;
			break;
		}
		case MumbleMessageType::CodecVersion:{
			std::clog<< "CodecVersion" <<std::endl;
			break;
		}
		case MumbleMessageType::ServerConfig:{
			std::clog<< "ServerConfig" <<std::endl;
			break;
		}
		case MumbleMessageType::SuggestConfig:{
			std::clog<< "SuggestConfig" <<std::endl;
			break;
		}
		case MumbleMessageType::VoiceTarget: /*future feature?*/ std::clog<< "VoiceTarget" <<std::endl; break;
		case MumbleMessageType::ChannelRemove: /*probably irrelevant*/ std::clog<< "ChannelRemove" <<std::endl; break;
		case MumbleMessageType::UserRemove: /*probably irrelevant*/ std::clog<< "UserRemove" <<std::endl; break;
		case MumbleMessageType::BanList: /*irrelevant*/ std::clog<< "BanList" <<std::endl; break;
		case MumbleMessageType::ACL: /*irrelevant*/ std::clog<< "ACL" <<std::endl; break;
		case MumbleMessageType::ContextActionModify: /*irrelevant*/ std::clog<< "ContextActionAdd" <<std::endl; break;
		case MumbleMessageType::ContextAction: /*irrelevant*/ std::clog<< "ContextAction" <<std::endl; break;
		case MumbleMessageType::UserStats: /*irrelevant*/ std::clog<< "UserStats" <<std::endl; break;
		case MumbleMessageType::RequestBlob: /*irrelevant*/ std::clog<< "RequestBlob" <<std::endl; break;
		case MumbleMessageType::UDPTunnel: /*unused*/ break;
		case MumbleMessageType::Authenticate: /*should not be recieved by client*/ std::clog<< "received auth message?" <<std::endl; break;
		default: std::clog<< "dunno what this is" <<std::endl; break;
	}
}

void MumbleConnector::handlePing(MumbleProto::Ping pingMsg){
	std::cout << "send pong?" <<std::endl;
}
void MumbleConnector::handleCrypto(MumbleProto::CryptSetup cryptMsg){
}
