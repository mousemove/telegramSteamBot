
#ifndef PARSER_H_
#define PARSER_H_
class NetworkDispetcher;
class AbstractBot;
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
class Parser {
private:
	NetworkDispetcher * _nd;
	AbstractBot * _bot;

public:
	std::condition_variable cvar;
	std::mutex  rMutex;
	std::queue<std::string> MessagesQueue;
	void sendMessage(const char * chat,const char * message);
	void sendTFunction();
	Parser();
	virtual ~Parser();
	void setND(NetworkDispetcher * nd);
	void setBot(AbstractBot * bot);
	void setBot();
	void readAndAnswer();
	void operator()();
};

#endif /* PARSER_H_ */
