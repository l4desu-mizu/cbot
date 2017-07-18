#include "mumble/MumbleConnector.h"
#include <google/protobuf/text_format.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>

#define TIME_IN_MS std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count()
#define PING_TIMEOUT 20 //max 30

MumbleConnector::MumbleConnector(std::string host,int port):username("Test"),password(""){
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
			std::thread(&MumbleConnector::dispatchMessage,this,header,msg).detach();
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
	authMessage.set_username(username);
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
	//easy switchcase generation, call a handle method for each Proto-Type
#define MUMBLE_MESSAGE_TYPE(x) case MumbleMessageType:: x:{\
		MumbleProto:: x tmp;\
		tmp.ParseFromString(message);\
		handle(tmp);\
		break;\
	}
	switch(header.getMessageType()){
		MUMBLE_MESSAGE_TYPES_ALL
		default: std::clog<< "dunno what this is" <<std::endl; break;
	}
#undef MUMBLE_MESSAGE_TYPE
}
//}}}

void MumbleConnector::pingLoop(){
	while(ping){//TODO: count ping packages and inform server about delays
		pingPackage.set_timestamp(TIME_IN_MS);
		sendProtoMessage(MumbleMessageType::Ping,pingPackage);
		std::this_thread::sleep_for(std::chrono::seconds(PING_TIMEOUT));
	}
}

//message handler
void MumbleConnector::handle(const MumbleProto::Version& version){
	std::clog<< "version: " <<std::endl;
	std::string output;
	google::protobuf::TextFormat::PrintToString(version,&output);
	std::clog<< output <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::Reject& rejectMsg){
	std::clog<< "Reject" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ServerSync& syncMsg){
	std::clog<< "ServerSync" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ChannelState& stateMsg){
	std::clog<< "ChannelState" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::UserState& stateMsg){
	std::clog<< "UserState" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::TextMessage& textMsg){
	std::clog<< "TextMessage" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::PermissionDenied& deniedMsg){
	std::clog<< "PermissionDenied" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::QueryUsers& queryMsg){
	std::clog<< "QueryUsers" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::CryptSetup& cryptoMsg){
	std::clog<< "CryptSetup" <<std::endl;
	udpCrypto=cryptoMsg;
}
void MumbleConnector::handle(const MumbleProto::UserList& userListMsg){
	std::clog<< "UserList" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::PermissionQuery& queryMsg){
	std::clog<< "PermissionQuery" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::CodecVersion& codecMsg){
	std::clog<< "CodecVersion" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ServerConfig& configMsg){
	std::clog<< "ServerConfig" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::SuggestConfig& configMsg){
	std::clog<< "SuggestConfig" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::Ping& pong){
	pingPackage.set_tcp_packets(pingPackage.tcp_packets()+1);
	std::clog<< "pong" <<std::endl;
}

//unused handler
void MumbleConnector::handle(const MumbleProto::VoiceTarget& voiceMsg){
	/*future feature?*/ std::clog<< "VoiceTarget" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ChannelRemove& channelMsg){
	/*probably irrelevant*/ std::clog<< "ChannelRemove" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::UserRemove& userRemoveMsg){
	/*probably irrelevant*/ std::clog<< "UserRemove" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::BanList& banMsg){
	/*irrelevant*/ std::clog<< "BanList" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ACL& aclMsg){
	/*irrelevant*/ std::clog<< "ACL" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ContextActionModify& contextModifyMsg){
	/*irrelevant*/ std::clog<< "ContextActionAdd" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ContextAction& contextMsg){
	/*irrelevant*/ std::clog<< "ContextAction" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::UserStats& userStatMsg){
	/*irrelevant*/ std::clog<< "UserStats" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::RequestBlob& blobMsg){
	/*irrelevant*/ std::clog<< "RequestBlob" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::UDPTunnel& udpMsg){
	/*unused*/
}
void MumbleConnector::handle(const MumbleProto::Authenticate& authMsg){
	/*should not be recieved by client*/ std::clog<< "received auth message?" <<std::endl;
}
