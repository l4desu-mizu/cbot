#pragma once
#include "mumble.pb.h"
#include "io/Connector.h"
#include "io/SSLClientSocket.h"
#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>

#define HEADER_SIZE 6
#define HEADER_TYPE_SIZE 2
#define HEADER_LENGTH_SIZE 4
#define MUMBLE_MESSAGE_TYPES_ALL \
    MUMBLE_MESSAGE_TYPE(Version) \
    MUMBLE_MESSAGE_TYPE(UDPTunnel) \
    MUMBLE_MESSAGE_TYPE(Authenticate) \
    MUMBLE_MESSAGE_TYPE(Ping) \
    MUMBLE_MESSAGE_TYPE(Reject) \
    MUMBLE_MESSAGE_TYPE(ServerSync) \
    MUMBLE_MESSAGE_TYPE(ChannelRemove) \
    MUMBLE_MESSAGE_TYPE(ChannelState) \
    MUMBLE_MESSAGE_TYPE(UserRemove) \
    MUMBLE_MESSAGE_TYPE(UserState) \
    MUMBLE_MESSAGE_TYPE(BanList) \
    MUMBLE_MESSAGE_TYPE(TextMessage) \
    MUMBLE_MESSAGE_TYPE(PermissionDenied) \
    MUMBLE_MESSAGE_TYPE(ACL) \
    MUMBLE_MESSAGE_TYPE(QueryUsers) \
    MUMBLE_MESSAGE_TYPE(CryptSetup) \
    MUMBLE_MESSAGE_TYPE(ContextActionModify) \
    MUMBLE_MESSAGE_TYPE(ContextAction) \
    MUMBLE_MESSAGE_TYPE(UserList) \
    MUMBLE_MESSAGE_TYPE(VoiceTarget) \
    MUMBLE_MESSAGE_TYPE(PermissionQuery) \
    MUMBLE_MESSAGE_TYPE(CodecVersion) \
    MUMBLE_MESSAGE_TYPE(UserStats) \
    MUMBLE_MESSAGE_TYPE(RequestBlob) \
    MUMBLE_MESSAGE_TYPE(ServerConfig) \
    MUMBLE_MESSAGE_TYPE(SuggestConfig)
#undef MUMBLE_MESSAGE_TYPE //undefine to possibly redefine for magical use

class MumbleHeader{
	public:
		MumbleHeader(uint16_t messageType,uint32_t messageLength);
		~MumbleHeader();
		std::ostream& writeToStream(std::ostream& os) const;
		std::ostream& writeToStreamInNetworkFormat(std::ostream& os) const;
		friend std::ostream& operator<<(std::ostream& os, const MumbleHeader& mh);
		uint16_t getMessageType() const;
		uint32_t getMessageLength() const;
	private:
		const uint16_t messageType;
		const uint32_t messageLength;
};
enum MumbleMessageType{//Maybe macros?
#define MUMBLE_MESSAGE_TYPE(x) x, //define macro to define list
	MUMBLE_MESSAGE_TYPES_ALL
#undef MUMBLE_MESSAGE_TYPE //undefine to possibly redefine for magical use
};

class MumbleConnector: public Connector{
	public:
		MumbleConnector(SSLClientSocket* socket, const std::string username, const std::string password="");
		~MumbleConnector();
		void connect();
		void disconnect();
		void updateUserInfo(const User& user);
		void moveToTextChat(const Channel& c);
		void moveToTextChat(const User& u, const Channel& c);
		void sendTextMessage(const std::string& message);
		void whisperTextMessage(const User& user, const std::string& message);
		void whisperTextMessage(const Channel& channel, const std::string& message);
		void whisperTextMessage(const std::vector<User>& users, const std::string& message);
		void whisperTextMessage(const std::vector<Channel>& channels, const std::string& message);
	private:
		const std::string username;
		const std::string password;
		std::atomic<bool> serverSyncd;
		std::atomic<bool> receiveLoopRuns;
		std::atomic<bool> ping;
		std::mutex dispatchLock;
		std::atomic<int> dispatchThreads;
		int channelID;
		int sessionID;
		SSLClientSocket* socket=NULL;
		MumbleProto::CryptSetup udpCrypto;
		std::mutex pingLock;
		MumbleProto::Ping pingPackage;
		std::thread receiveThread;
		std::thread pingThread;
		void handleReceives();
		void connection();
		void auth();
		void sendProtoMessage(const MumbleMessageType& msgType,const std::string& message);
		void sendProtoMessage(const MumbleMessageType& msgType,const ::google::protobuf::Message& message);
		void dispatchMessage(const MumbleHeader header, const std::string message);

		void pingLoop();

		void logProtoMessage(const ::google::protobuf::Message& message);

		void handle(const MumbleProto::Version& version);
		void handle(const MumbleProto::Reject& rejectMsg);
		void handle(const MumbleProto::ServerSync& syncMsg);
		void handle(const MumbleProto::ChannelState& stateMsg);
		void handle(const MumbleProto::ChannelRemove& channelMsg);
		void handle(const MumbleProto::UserState& stateMsg);
		void handle(const MumbleProto::TextMessage& textMsg);
		void handle(const MumbleProto::PermissionDenied& deniedMsg);
		void handle(const MumbleProto::QueryUsers& queryMsg);
		void handle(const MumbleProto::CryptSetup& cryptoMsg);
		void handle(const MumbleProto::UserList& userListMsg);
		void handle(const MumbleProto::PermissionQuery& queryMsg);
		void handle(const MumbleProto::CodecVersion& codecMsg);
		void handle(const MumbleProto::ServerConfig& configMsg);
		void handle(const MumbleProto::SuggestConfig& configMsg);
		void handle(const MumbleProto::Ping& pong);

		//these are currently ignored, therefore the implementation
		void handle(const MumbleProto::VoiceTarget& voiceMsg);
		void handle(const MumbleProto::UserRemove& userRemoveMsg);
		void handle(const MumbleProto::BanList& banMsg);
		void handle(const MumbleProto::ACL& aclMsg);
		void handle(const MumbleProto::ContextActionModify& contextModifyMsg);
		void handle(const MumbleProto::ContextAction& contextMsg);
		void handle(const MumbleProto::UserStats& userStatMsg);
		void handle(const MumbleProto::RequestBlob& blobMsg);
		void handle(const MumbleProto::UDPTunnel& udpMsg);
		void handle(const MumbleProto::Authenticate& authMsg);
};
