#include "mumble/MumbleConnector.h"
#include <google/protobuf/text_format.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>

#define TIME_IN_MS std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count()
#define PING_TIMEOUT 20 //max 30

MumbleConnector::MumbleConnector(std::string host,int port){
	//open socket
	//handshake
	//socket=new ManagedSSLSocket(host,port,"./cert/mumble_bot.cert","./cert/mumble_bot_cert.key");//presumably done...
	socket=new ManagedSSLSocket(host,port);//presumably done...
	receiveLoopRuns=true;
	receiveThread=std::thread(&MumbleConnector::handleReceives,this);
	std::clog << "connecting to Mumble" << std::endl;
	connect();
	auth();
	ping=true;
	pingThread=std::thread(&MumbleConnector::pingLoop,this);
}

MumbleConnector::~MumbleConnector(){
	std::clog << "connector ends" << std::endl;
	receiveLoopRuns=false;
	receiveThread.join();
	ping=false;
	pingThread.join();
	if(socket!=NULL){
		delete socket;
		socket=NULL;
	}
}

void MumbleConnector::sendTextMessage(const std::string& message){
}

void MumbleConnector::handleReceives(){
	while(receiveLoopRuns){
		std::stringstream tmp(socket->receive());
		char pHeader[HEADER_SIZE];
		std::string msg;

		while(tmp.tellg()>=0){
			tmp.read(pHeader,HEADER_SIZE);

			//construct header (should be optimized by compiler?)
			const uint16_t type=ntohs(*reinterpret_cast<uint16_t*>(pHeader));
			const uint32_t length=ntohl(*reinterpret_cast<uint32_t*>(pHeader+2));
			const MumbleHeader header(type,length);

			msg.resize(header.getMessageLength());
			tmp.read(&msg[0],header.getMessageLength());
			dispatchMessage(header,msg);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
}

void MumbleConnector::connect(){
	//version exchange (skippable)
	const int versionBytes=0x0102060;
	const std::string release = "1.2.6";
	const std::string os = "linux";
	const std::string os_version = "cbot";
	MumbleProto::Version myVersion;
	std::string buffer;
	myVersion.set_version(versionBytes);
	myVersion.set_release(release.c_str());
	myVersion.set_os(os.c_str());
	myVersion.set_os_version(os_version.c_str());
	myVersion.SerializeToString(&buffer);
	sendProtoMessage(MumbleMessageType::Version,buffer);
}

void MumbleConnector::auth(){
	MumbleProto::Authenticate authMessage;
	authMessage.set_username("Test");
	std::string buffer;
	authMessage.SerializeToString(&buffer);
	sendProtoMessage(MumbleMessageType::Authenticate,buffer);
}

void MumbleConnector::sendProtoMessage(const MumbleMessageType& msgType,const std::string& message){
	std::stringstream protoMessage;
	const MumbleHeader header(msgType,(uint32_t)message.size());
	header.writeToStreamInNetworkFormat(protoMessage);
	protoMessage << message;
	protoMessage.flush();
	socket->send(protoMessage.str());
}
void MumbleConnector::sendProtoMessage(const MumbleMessageType& msgType,const ::google::protobuf::Message& message){
	std::string str_message;
	message.SerializeToString(&str_message);
	sendProtoMessage(msgType,str_message);
}

//{{{
void MumbleConnector::dispatchMessage(const MumbleHeader& header, const std::string& message){
	switch(header.getMessageType()){
		case MumbleMessageType::Version:{
			std::clog<< "version: " <<std::endl;
			MumbleProto::Version v;
			v.ParseFromString(message);
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
			udpCrypto.ParseFromString(message);
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
		case MumbleMessageType::Ping:{ /*irrelevant*/ std::clog<< "pong" <<std::endl; break; }
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
//}}}

void MumbleConnector::pingLoop(){
	while(ping){//TODO: count ping packages and inform server about delays
		MumbleProto::Ping pingPackage;
		pingPackage.set_timestamp(TIME_IN_MS);
		sendProtoMessage(MumbleMessageType::Ping,pingPackage);
		std::this_thread::sleep_for(std::chrono::seconds(PING_TIMEOUT));
	}
}
