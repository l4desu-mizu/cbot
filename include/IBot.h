#pragma once

class IBot{
	public:
		virtual ~IBot(){}
		virtual void run()=0;
};
