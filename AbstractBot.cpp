
//* На деле класс должен быть абстрактным и оператор () чисто виртуальным, но был дополнен в качестве примера, это бот, повторяющий за вами ответы *//
#include "AbstractBot.h"

#include <algorithm>
#include <chrono>
using namespace std;

AbstractBot::AbstractBot() {

}

AbstractBot::~AbstractBot() {
}

void AbstractBot::operator()()
{

	while(1)
	{
		unique_lock<std::mutex> ulRead(parseMutex);
		cvarForReading.wait(ulRead,[this]()->int{return  !eventsQueue.empty() ;});
		auto message =  eventsQueue.front();
		eventsQueue.pop();
		if (message.second != "/start" )
		{
			unique_lock<std::mutex> ulSend(sendMutex);
			sendQueue.push(std::pair<string,string>(message.first,message.second));
		}
		else
		{
			sendQueue.push(std::pair<string,string>(message.first,"Привет! Напиши мне что-нибудь и я тебе отвечу тоже самое!"));
		}
		cvarForParsing.notify_all();
	}
}
