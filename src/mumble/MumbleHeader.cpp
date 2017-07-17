#include "mumble/MumbleConnector.h"

MumbleHeader::MumbleHeader(uint16_t messageType,uint32_t messageLength):messageType(messageType),messageLength(messageLength){
}
MumbleHeader::~MumbleHeader(){
}
uint16_t MumbleHeader::getMessageType() const{
	return this->messageType;
}
uint32_t MumbleHeader::getMessageLength() const{
	return this->messageLength;
}
std::ostream& MumbleHeader::writeToStream(std::ostream& os) const{
	os.write(reinterpret_cast<const char*>(&this->messageType),2);
	os.write(reinterpret_cast<const char*>(&this->messageLength),4);
	return os;
}
std::ostream& MumbleHeader::writeToStreamInNetworkFormat(std::ostream& os) const{
	const uint16_t typeValue=htons(this->messageType);
	const uint32_t lengthValue=htonl(this->messageLength);
	os.write(reinterpret_cast<const char*>(&typeValue),2);
	os.write(reinterpret_cast<const char*>(&lengthValue),4);
	return os;
}
std::ostream& operator<<(std::ostream& os, MumbleHeader const mh){
	return mh.writeToStream(os);
}
