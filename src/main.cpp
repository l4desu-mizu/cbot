#include "SSLClientSocket.h"
#include "SimpleMumbleBot.h"
#include "HttpMumbleBot.h"
#include "mumble/MumbleConnector.h"
#include <iniparser.h>
#include <string>
#include <iostream>
#include <exception>

#define DEFAULT_MUMBLE_PORT 64738
#define DEFAULT_USERNAME "cbot"
#define DEFAULT_SERVER "localhost"
#define INI_NAME "example.ini"

#define SIMPLE_MUMBLE_BOT "simplemumblebot"
#define HTTP_MUMBLE_BOT "httpmumblebot"

//ini keys
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
void readBotInfo(dictionary* ini, BotInfo& bot){
	//check if any configurations were made
	int botcount=iniparser_getnsec(ini);
	if(botcount<1){
		throw std::runtime_error("No bot specified.");
	}else if(botcount>1){
		std::cout << "Only creating one bot." << std::endl;
	}

	bot.type=iniparser_getsecname(ini,0);
	if(bot.type!=HTTP_MUMBLE_BOT&&bot.type!=SIMPLE_MUMBLE_BOT){
		std::cout << bot.type << std::endl;
		throw std::runtime_error("Dont know this bot.");
	}

	int keycount=iniparser_getsecnkeys(ini,bot.type.c_str());

	//allocate memory for keys and get them
	const char* keys[keycount];
	iniparser_getseckeys(ini,bot.type.c_str(),keys);

	//construct full keys section:key
	const std::string username_key=bot.type+":"+USERNAME_KEY;
	const std::string server_key=bot.type+":"+SERVER_KEY;
	const std::string password_key=bot.type+":"+PASSWORD_KEY;
	const std::string port_key=bot.type+":"+PORT_KEY;
	const std::string certFile_key=bot.type+":"+CERTFILE_KEY;
	const std::string certKey_key=bot.type+":"+CERTKEYFILE_KEY;
	const std::string channel_key=bot.type+":"+CHANNEL_KEY;
	const std::string script_key=bot.type+":"+SCRIPT_KEY;

	//fill botinfo with new or old data if failing
	bot.username=iniparser_getstring(ini,username_key.c_str(),bot.username.c_str());
	bot.server=iniparser_getstring(ini,server_key.c_str(),bot.server.c_str());
	bot.password=iniparser_getstring(ini,password_key.c_str(),"");
	bot.port=iniparser_getint(ini,port_key.c_str(),bot.port);
	bot.certFile=iniparser_getstring(ini,certFile_key.c_str(),"");
	bot.certKey=iniparser_getstring(ini,certKey_key.c_str(),"");
	bot.channel=iniparser_getstring(ini,channel_key.c_str(),"");
	bot.script=iniparser_getstring(ini,script_key.c_str(),"");
}
//}}} config read end

void stopBotLoop(int signal){
	runBot=false;
}

int main(int argc,char** argv){

	//read ini file
	dictionary* ini=NULL;
	if(argc>1){//argv[0] is the binary itself
		ini=iniparser_load(argv[1]);
	}else{
		std::cout << "Using default ini-File: " << INI_NAME << std::endl;
		ini=iniparser_load(INI_NAME);
	}
	if(ini==NULL){
		std::cout << "Something wrong with the ini file." << std::endl;
		iniparser_freedict(ini);
		return 1;
	}

	//config: fill botInfo with default values
	BotInfo botInfo;
	botInfo.type=SIMPLE_MUMBLE_BOT;
	botInfo.username=DEFAULT_USERNAME;
	botInfo.server=DEFAULT_SERVER;
	botInfo.port=DEFAULT_MUMBLE_PORT;
	try{
		readBotInfo(ini,botInfo);
		iniparser_freedict(ini);//final use of ini
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
			bot = new HttpMumbleBot(connector,botInfo.script,botInfo.channel);
		}else{
			bot = new SimpleMumbleBot(connector,botInfo.channel);
		}

		//run the bot
		bot->preRun();
		runBot=true;
		while(runBot){
			bot->run();
		}
	}catch(std::runtime_error& error){//catch errors in run or connection
		std::cout << "Shutting down because: " << error.what() << std::endl;
	}
	delete bot;
	delete connector;
	delete mySocket;
	return 0;
}
