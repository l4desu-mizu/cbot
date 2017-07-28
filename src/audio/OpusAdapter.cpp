#include "audio/OpusAdapter.h"

#define SAMPLING_RATE 48000
#define CHANNELS 1
#define DECODE_CORRECTION_FLAG 0

OpusAdapter::OpusAdapter(){
	int error=0;
	encoder=opus_encoder_create(SAMPLING_RATE,CHANNELS,OPUS_APPLICATION_AUDIO,&error);
	decoder=opus_decoder_create(SAMPLING_RATE,CHANNELS,&error);
}
OpusAdapter::~OpusAdapter(){
	if(encoder!=nullptr){
		opus_encoder_destroy(encoder);
		encoder=nullptr;
	}
	if(decoder!=nullptr){
		opus_decoder_destroy(decoder);
		decoder=nullptr;
	}
}

int OpusAdapter::encode(const AudioData& in,AudioData& out){
	int result=0;

	//AudioData.data is char which is half the size of opus_int16
	const opus_int16* inData=reinterpret_cast<opus_int16*>(in.data);
	result=opus_encode(encoder,inData,in.size/2,out.data,out.size);

	return result;
}

int OpusAdapter::decode(const AudioData& in,AudioData& out){
	int result=0;

	//AudioData.data is char which is half the size of opus_int16
	opus_int16* outData=reinterpret_cast<opus_int16*>(out.data);
	result=opus_decode(decoder,in.data,in.size,outData,out.size/2,DECODE_CORRECTION_FLAG);

	return result;
}
