#pragma once
#include "audio/AudioData.h"

class CodecAdapter{
	public:
		virtual ~CodecAdapter(){}
		virtual int encode(const AudioData& in,AudioData& out)=0;
		virtual int decode(const AudioData& in,AudioData& out)=0;
};
