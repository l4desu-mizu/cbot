#pragma once

#include "audio/CodecAdapter.h"
#include <opus.h>

class OpusAdapter:public CodecAdapter{
	public:
		OpusAdapter();
		~OpusAdapter();
		int encode(const AudioData& in,AudioData& out);
		int decode(const AudioData& in,AudioData& out);
	private:
		OpusEncoder* encoder=nullptr;
		OpusDecoder* decoder=nullptr;
};
