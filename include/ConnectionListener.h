#pragma once

enum ConnectionEvent{
	Connect,
	Disconnect
};
class ConnectionListener{
	public:
		virtual ~ConnectionListener(){}
		virtual void notify(const ConnectionEvent e)=0;
};
