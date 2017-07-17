#pragma once
#include "mumble.pb.h"
#include "Connector.h"
#include "ManagedSSLSocket.h"
#include <thread>
#include <iostream>

#define HEADER_SIZE 6
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
		MumbleConnector(std::string host,int port=64738); //mumble default port
		~MumbleConnector();
		void sendTextMessage(std::string message);
	private:
		ManagedSSLSocket* socket=NULL;
		bool receiveLoopRuns=false;
		std::thread receiveThread;
		void handleReceives();
		void connect();
		void auth();
		void disconnect();
		void sendProtoMessage(const MumbleMessageType& msgType,const std::string& message);
		void dispatchMessage(const MumbleHeader& header, const std::string& message);

		void handlePing(MumbleProto::Ping pingMsg);
		//void handleText(MumbleProto::TextMessage textMsg);
		void handleCrypto(MumbleProto::CryptSetup cryptMsg);
		//void handleServerSync(MumbleProto::ServerSync syncMsg);
		//void handleReject(MumbleProto::Reject rejectMsg);
		//void handleChannelState(MumbleProto::ChannelState stateMsg);
		//void handleUserState(MumbleProto::UserState stateMsg);
		//void handlePermissionDenied(MumbleProto::PermissionDenied deniedMsg);
		//void handlePermissionQuery(MumbleProto::PermissionQuery queryMsg);
		//void handleQueryUsers(MumbleProto::QueryUsers queryMsg);
		//void handleCodec(MumbleProto::CodecVersion codecMsg);
		//void handleServerConfig(MumbleProto::ServerConfig configMsg);
		//void handleSuggestConfig(MumbleProto::SuggestConfig configMsg);
};
