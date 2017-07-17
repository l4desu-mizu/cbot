#pragma once
#include "Connector.h"
#include "ManagedSSLSocket.h"
#include <thread>
#include <iostream>

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
	Version,
	UDPTunnel,
	Authenticate,
	Ping,
	Reject,
	ServerSync,
	ChannelRemove,
	ChannelState,
	UserRemove,
	UserState,
	BanList,
	TextMessage,
	PermissionDenied,
	ACL,
	QueryUsers,
	CryptSetup,
	ContextActionModify,
	ContextAction,
	UserList,
	VoiceTarget,
	PermissionQuery,
	CodecVersion,
	UserStats,
	RequestBlob,
	ServerConfig,
	SuggestConfig
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
		void receiveLoop();
		void connect();
		void auth();
		void disconnect();
};
