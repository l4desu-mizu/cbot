#pragma once

class IBot{
	public:
		virtual ~IBot(){}
		virtual void preRun()=0;
		virtual void run()=0;
};
