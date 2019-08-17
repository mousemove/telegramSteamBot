

#include "Parser.h"
#include "NetworkDispetcher.h"
#include "Parser.h"
#include "AbstractBot.h"
using namespace std;
using json = nlohmann::json;
Parser::Parser() {


}

Parser::~Parser() {

}

void Parser::setND(NetworkDispetcher * nd)
{
	_nd = nd;
}
void Parser::setBot(AbstractBot * bot)
{
	_bot = bot;
}
void Parser::readAndAnswer()
{

}


void Parser::operator()()
{
	while(1)
	{
		unique_lock<std::mutex> ulReadFromDisp(rMutex);
		cvar.wait(ulReadFromDisp,[this]()->int{return  !MessagesQueue.empty() ;});

		vector<pair<string,string>> messages;

		while(!MessagesQueue.empty())
		{
			string data =  MessagesQueue.front();
			MessagesQueue.pop();
			json j;
			try
			{
				j = json::parse(data);
			}
			catch (std::exception & e) {

				cout << "isnt json!" << endl;
				continue;
			}
			if(j.contains("message") && j["message"].contains("from") && j["message"]["from"].contains("id") && j["message"].contains("text") )
			{
				string chat = to_string(j["message"]["from"]["id"]);
				string message = j["message"]["text"];
				messages.push_back(pair<string,string>(chat,message));
			}
		}
			    	 unique_lock<std::mutex> ulNewEvent(_bot->parseMutex);
			    	 for(auto c : messages)
			    	 {
			    		 _bot->eventsQueue.push(c);
			    	 }
			    	 _bot->cvarForReading.notify_all();
	}
}


void Parser::sendMessage(const char * chat,const char * message)
{
	_nd->sendMessageCURL(chat,message);
}

void Parser::sendTFunction()
{
	while(1)
	{

		unique_lock<std::mutex> ulSend(_bot->sendMutex);
		_bot->cvarForParsing.wait(ulSend,[this]()->int{return  !_bot->sendQueue.empty() ;});
		auto message =  _bot->sendQueue.front();
		_bot->sendQueue.pop();
		_nd->sendMessageCURL(message.first.c_str(),message.second.c_str());

	}
}

