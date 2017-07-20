#include "extensions/http/HttpRelay.h"
#include <string>
#include <sstream>

static size_t httpCallback(void *contents, size_t size, size_t nmemb, void *userp){
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
};

HttpRelay::HttpRelay(const std::string url):url(url){
	curl=curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &httpCallback);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);//set Post
}
HttpRelay::~HttpRelay(){
	if(curl!=NULL){
		curl_easy_cleanup(curl);
		curl=NULL;
	}
}
Text HttpRelay::relayMessage(const Text& text){
	std::stringstream queryBuilder;
	std::string query;
	CURLcode res;
	std::string resultBuffer;

	//post message
	queryBuilder << "command=" << text.message << "&";
	queryBuilder << "user=" << text.from.getName() << "&";
	queryBuilder << "private_message=" << "false" << std::flush;

	query=queryBuilder.str();

	//what to post
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());
	//answer
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultBuffer);
	res = curl_easy_perform(curl);
	return {resultBuffer,text.to,text.from};
}
