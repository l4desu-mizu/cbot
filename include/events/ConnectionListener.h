#pragma once

enum ConnectionEvent{
	Connect,
	Kick,
	Ban,
	Disconnect
};
class ConnectionListener{
	public:
		virtual ~ConnectionListener(){}
		virtual void notify(const ConnectionEvent e)=0;
};
