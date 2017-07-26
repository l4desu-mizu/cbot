#pragma once

class IBot{
	public:
		virtual ~IBot(){}
		virtual bool preRun()=0;
		virtual bool run()=0;
};
