#ifdef HTTP_ENABLED
#include "HttpMumbleBot.h"
#endif

#include "SSLClientSocket.h"
#include "MoveMumbleBot.h"
#include "SimpleMumbleBot.h"
#include "mumble/MumbleConnector.h"
#include <libconfig.h++>
#include <string>
#include <vector>
#include <iostream>
#include <exception>

#define DEFAULT_MUMBLE_PORT 64738
#define DEFAULT_USERNAME "cbot"
#define DEFAULT_SERVER "localhost"
#define INI_NAME "example.cfg"

#define SIMPLE_MUMBLE_BOT "SimpleMumbleBot"
#define HTTP_MUMBLE_BOT "HttpMumbleBot"
#define MOVE_MUMBLE_BOT "MoveMumbleBot"

//cfg keys
#define TYPE_KEY "type"
#define CONNECTOR_KEY "connector"
#define USERNAME_KEY "username"
#define PASSWORD_KEY "password"
#define SERVER_KEY "server"
#define PORT_KEY "port"
#define CERTFILE_KEY "certificate"
#define CERTKEYFILE_KEY "certificateKey"
#define CHANNEL_KEY "channel"
#define SCRIPT_KEY "script"

struct BotInfo{
	std::string type;
	std::string username;
	std::string password;
	std::string server;
	int port;
	std::string certFile;
	std::string certKey;
	std::string script;
	std::string channel;
};

//used in main to run the bot
bool runBot=false;

//{{{ config read begin
std::vector<BotInfo> readBotInfo(libconfig::Config& cfg, const BotInfo& templateInfo={SIMPLE_MUMBLE_BOT,DEFAULT_USERNAME,"",DEFAULT_SERVER,DEFAULT_MUMBLE_PORT,"","","",""}){
	std::vector<BotInfo> botInfos;
	libconfig::Setting& set=cfg.getRoot();

	for(auto configIt=set.begin();configIt!=set.end();configIt++){
		if(configIt->isGroup()){
			BotInfo currentInfo=templateInfo;
			for(auto groupIt=configIt->begin();groupIt!=configIt->end();groupIt++){
				std::string currentName(groupIt->getName());

				if(currentName==USERNAME_KEY&&groupIt->getType()==libconfig::Setting::Type::TypeString){
					currentInfo.username=set.lookup(groupIt->getPath()).c_str();
				}else if(currentName==PASSWORD_KEY&&groupIt->getType()==libconfig::Setting::Type::TypeString){
					currentInfo.password=set.lookup(groupIt->getPath()).c_str();
				}else if(currentName==CHANNEL_KEY&&groupIt->getType()==libconfig::Setting::Type::TypeString){
					currentInfo.channel=set.lookup(groupIt->getPath()).c_str();
				}else if(currentName==SCRIPT_KEY&&groupIt->getType()==libconfig::Setting::Type::TypeString){
					currentInfo.script=set.lookup(groupIt->getPath()).c_str();
				}else if(currentName==CERTFILE_KEY&&groupIt->getType()==libconfig::Setting::Type::TypeString){
					currentInfo.certFile=set.lookup(groupIt->getPath()).c_str();
				}else if(currentName==CERTKEYFILE_KEY&&groupIt->getType()==libconfig::Setting::Type::TypeString){
					currentInfo.certKey=set.lookup(groupIt->getPath()).c_str();
				}else if(currentName==CONNECTOR_KEY&&groupIt->isGroup()){
					//read connector, with server,port and bot type
					try{//try reading server, set default if wrong type or empty
						currentInfo.server=set.lookup(groupIt->getPath()+"."+SERVER_KEY).c_str();
					}catch(libconfig::ConfigException& e){
						std::cout << "Your config seems wrong. " << e.what() << std::endl;
						std::cout << "Setting default server." << std::endl;
						currentInfo.server=DEFAULT_SERVER;
					}
					try{//try reading port, set default if wrong type or empty
						currentInfo.port=set.lookup(groupIt->getPath()+"."+PORT_KEY);
					}catch(libconfig::ConfigException& e){
						std::cout << "Your config seems wrong. " << e.what() << std::endl;
						std::cout << "Setting default port." << std::endl;
						currentInfo.port=DEFAULT_MUMBLE_PORT;
					}
					try{//try reading type, set default if wrong type or empty
						currentInfo.type=set.lookup(groupIt->getPath()+"."+TYPE_KEY).c_str();
					}catch(libconfig::ConfigException& e){
						std::cout << "Your config seems wrong. " << e.what() << std::endl;
						std::cout << "Setting default type." << std::endl;
						currentInfo.type=SIMPLE_MUMBLE_BOT;
					}
				}
			}
			botInfos.push_back(currentInfo);
		}
	}
	return botInfos;
}
//}}} config read end

void stopBotLoop(int signal){
	runBot=false;
}

int main(int argc,char** argv){

	//read cfg file
	libconfig::Config cfg;
	try{
		if(argc>1){//argv[0] is the binary itself
			cfg.readFile(argv[1]);
		}else{
			std::cout << "Using default cfg-File: " << INI_NAME << std::endl;
			cfg.readFile(INI_NAME);
		}
	}catch(libconfig::ParseException& exceptio){
		std::cout << "Something wrong with the cfg file." << std::endl;
		return 1;
	}

	//config: fill botInfo with default values
	BotInfo botInfo;
	try{
		std::vector<BotInfo> infos=readBotInfo(cfg);
		if(infos.size()<1){
			throw std::runtime_error("No configuration found in configuration file");
		}
		botInfo=infos[0];
	}catch(std::runtime_error& e){
		std::cout << e.what() << std::endl;
		return 1;
	}

	//prepare bot loop exit
	signal(SIGINT,stopBotLoop);

	//bot pointers
	SSLClientSocket* mySocket=NULL;
	Connector* connector=NULL;
	IBot* bot=NULL;
	try{
		//create bot with given botInfo
		mySocket = new SSLClientSocket(botInfo.server,botInfo.port,botInfo.certFile,botInfo.certKey);
		connector = new MumbleConnector(mySocket,botInfo.username,botInfo.password);

		if(botInfo.type==HTTP_MUMBLE_BOT){
		#ifdef HTTP_ENABLED
			bot = new HttpMumbleBot(connector,botInfo.script,botInfo.channel);
		#else
			std::cout << "Http not enabled, install curl and rerun build." << std::endl;
		#endif
		}else if(botInfo.type==MOVE_MUMBLE_BOT){
			bot = new MoveMumbleBot(connector,botInfo.channel);
		}else{
			bot = new SimpleMumbleBot(connector,botInfo.channel);
		}

		//run the bot
		bot->preRun();
		runBot=true;
		while(runBot&&bot->run()){
			std::this_thread::sleep_for(std::chrono::seconds(1));//wait a bit before running again
		}
	}catch(std::runtime_error& error){//catch errors in run or connection
		std::cout << "Shutting down because: " << error.what() << std::endl;
	}
	std::cout << "Waiting for threads to die." << std::endl;
	if(bot!=NULL){
		delete bot;
	}
	if(connector!=NULL){
		delete connector;
	}
	if(mySocket!=NULL){
		delete mySocket;
	}
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
