#include "mumble/MumbleConnector.h"
#include <arpa/inet.h> //ntohl,ntohs
#include <google/protobuf/text_format.h>
#include <iostream>
#include <sstream>
#include <chrono>

#define TIME_IN_MS std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count()
#define PING_TIMEOUT 20 //max 30
#define MAX_WAIT 15

MumbleConnector::MumbleConnector(SSLClientSocket* socket, const std::string username, const std::string password):
socket(socket),
username(username),
password(password),
serverSyncd(false),
receiveLoopRuns(false),
ping(false){
}

MumbleConnector::~MumbleConnector(){
	disconnect();
}

void MumbleConnector::connect(){
	socket->connect();
	serverSyncd=false;
	receiveLoopRuns=true;
	receiveThread=std::thread(&MumbleConnector::handleReceives,this);
	std::clog << "connecting to Mumble" << std::endl;
	connection();
	auth();
	ping=true;
	pingThread=std::thread(&MumbleConnector::pingLoop,this);
	for(int i=0;i<MAX_WAIT;i++){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		if(serverSyncd){
			return;
		}
		if(!receiveLoopRuns){//socket died and receiveLoop stopped
			break;
		}
	}
	throw std::runtime_error("Connection could not be poperly established. Connection Timed out.");
}
void MumbleConnector::disconnect(){
	serverSyncd=false;
	socket->disconnect();
	receiveLoopRuns=false;
	if(receiveThread.joinable()){
		receiveThread.join();
	}
	ping=false;
	if(pingThread.joinable()){
		pingThread.join();
	}
}

void MumbleConnector::updateUserInfo(const User& user){
	MumbleProto::UserState users;
	users.set_session(user.getID());
	sendProtoMessage(MumbleMessageType::UserState, users);
}

void MumbleConnector::sendTextMessage(const std::string& message){
	MumbleProto::TextMessage text;
	text.set_actor(sessionID);
	text.add_channel_id(channelID);
	text.set_message(message);
	sendProtoMessage(MumbleMessageType::TextMessage, text);
}

void MumbleConnector::whisperTextMessage(const std::vector<User>& users,const std::string& message){
	MumbleProto::TextMessage text;
	text.set_actor(sessionID);
	for(const User& user:users){
		text.add_session(user.getID());
	}
	text.set_message(message);
	sendProtoMessage(MumbleMessageType::TextMessage, text);
}

void MumbleConnector::whisperTextMessage(const std::vector<Channel>& channels,const std::string& message){
	MumbleProto::TextMessage text;
	text.set_actor(sessionID);
	for(const Channel& c:channels){
		text.add_channel_id(c.getID());
	}
	text.set_message(message);
	sendProtoMessage(MumbleMessageType::TextMessage, text);
}

void MumbleConnector::moveToTextChat(const Entity& channel){
	MumbleProto::UserState stateChange;
	stateChange.set_session(sessionID);
	stateChange.set_channel_id(channel.getID());
	sendProtoMessage(MumbleMessageType::UserState, stateChange);
}


void MumbleConnector::handleReceives(){
	while(receiveLoopRuns){
		std::string tmp;
		try{
			tmp=(socket->receive());
		}catch(std::runtime_error& e){
			notifyListeners(ConnectionEvent::Disconnect);
			break;
		}
		const int inlength=tmp.size();
		if(inlength>0){
			std::stringstream inStream(tmp);
			char pHeader[HEADER_SIZE];
			std::string msg;
			while(inStream.tellg()>=0&&inStream.tellg()<inlength){
				inStream.read(pHeader,HEADER_SIZE);

				//construct header (should be optimized by compiler?)
				const uint16_t type=ntohs(*reinterpret_cast<uint16_t*>(pHeader));
				const uint32_t length=ntohl(*reinterpret_cast<uint32_t*>(pHeader+HEADER_TYPE_SIZE));
				const MumbleHeader header(type,length);

				msg.clear();
				msg.resize(header.getMessageLength(),'\0');
				inStream.read(&msg[0],header.getMessageLength());
				std::thread(&MumbleConnector::dispatchMessage,this,header,msg).detach();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	receiveLoopRuns=false;
}

void MumbleConnector::connection(){
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
void MumbleConnector::dispatchMessage(const MumbleHeader header, const std::string message){
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
		std::lock_guard<std::mutex> lock(pingLock);
		pingPackage.set_timestamp(TIME_IN_MS);
		sendProtoMessage(MumbleMessageType::Ping,pingPackage);
		for(int i=0;i<PING_TIMEOUT*2;i++){
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			if(!ping){
				break;
			}
		}
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
	std::string output;
	google::protobuf::TextFormat::PrintToString(rejectMsg,&output);
	std::clog<< output <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::ServerSync& syncMsg){
	std::clog<< "ServerSync" <<std::endl;
	serverSyncd=true;
	notifyListeners(ConnectionEvent::Connect);
}
void MumbleConnector::handle(const MumbleProto::ChannelState& stateMsg){
	std::clog<< "ChannelState" <<std::endl;

	//check for channelstate
	if(!stateMsg.has_channel_id()){
		return;
	}

	//create channel
	const int channelID=stateMsg.channel_id();
	std::string channelName="";
	if(stateMsg.has_name()){
		channelName=stateMsg.name();
	}
	const Channel newChannel(channelID,channelName,channelID==this->channelID);

	//update channels
	notifyListeners(newChannel);
}
void MumbleConnector::handle(const MumbleProto::ChannelRemove& channelMsg){
	std::clog<< "ChannelRemove" <<std::endl;
	if(channelMsg.has_channel_id()){
		unnotifyListeners(channelMsg.channel_id(),EntityType::Channel_type);
	}
}
void MumbleConnector::handle(const MumbleProto::UserState& stateMsg){
	std::clog<< "UserState" <<std::endl;
	//update users
	if(stateMsg.has_session()){
		const int userID=stateMsg.session();
		std::string name="";
		if(stateMsg.has_name()){
			name=stateMsg.name();
			if(sessionID==-1&&name==username){
				sessionID=userID;//TODO check for registered user_id so a name change can be observerd
			}
		}
		const User newUser(userID,name,sessionID==userID);
		notifyListeners(newUser);

		//get the id of the channel this connector is currently connected to
		if(stateMsg.has_channel_id()&&stateMsg.session()==sessionID){
			channelID=stateMsg.channel_id();
			const Channel newChannel(channelID,"",true);
			notifyListeners(newChannel);
		}
	}
}
void MumbleConnector::handle(const MumbleProto::TextMessage& textMsg){
	std::clog<< "TextMessage" <<std::endl;
	int fromID;
	bool isPrivate=false;
	if(!textMsg.has_actor()||textMsg.actor()==sessionID){//murmur does not send own messages to channels back, yet security
		return;
	}else{
		fromID=textMsg.actor();
	}
	if(textMsg.channel_id_size()<1){//if the text message does not have at least one channel, it's a private message
		isPrivate=true;
	}
	Text text(textMsg.message(),User(fromID,""),User(sessionID,username),isPrivate);
	notifyListeners(text);
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
	std::lock_guard<std::mutex> lock(pingLock);
	pingPackage.set_tcp_packets(pingPackage.tcp_packets()+1);
}

//unused handler
void MumbleConnector::handle(const MumbleProto::VoiceTarget& voiceMsg){
	/*future feature?*/ std::clog<< "VoiceTarget" <<std::endl;
}
void MumbleConnector::handle(const MumbleProto::UserRemove& userRemoveMsg){
	std::clog<< "UserRemove" <<std::endl;
	if(userRemoveMsg.session()==sessionID){
		sessionID=-1;
		if(userRemoveMsg.has_ban()&&userRemoveMsg.ban()){
			notifyListeners(ConnectionEvent::Ban);
		}else{
			notifyListeners(ConnectionEvent::Kick);
		}
	}else{
		unnotifyListeners(userRemoveMsg.session(),EntityType::User_type);
	}
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
